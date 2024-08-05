// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LyraCharacter.h"
#include "LyraALS.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "KismetTraceUtils.h"
#include "Camera/CameraComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interface/AnimBPInterface.h"

// Sets default values
ALyraCharacter::ALyraCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(
		TEXT("/Game/Characters/Heroes/Mannequin/Meshes/SKM_Manny"));
	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}
	GetCapsuleComponent()->SetCapsuleHalfHeight(90.f);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));


	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// 可以蹲下
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(60.f);

	// 设置跳参数
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 2048.f;
	GetCharacterMovement()->AirControl = 0.35f;

	// 添加武器组件
	Pistol = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Pistol"));
	Pistol->SetupAttachment(GetMesh(), TEXT("PistolUnEquip"));
	Pistol->SetSkeletalMesh(ConstructorHelpers::FObjectFinder<USkeletalMesh>(
		TEXT("/Game/ALS/SkeletalMesh/Pistol/SK_Pistol")).Object);
	Rifle = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Rifle"));
	Rifle->SetupAttachment(GetMesh(),TEXT("RifleUnEquip"));
	Rifle->SetSkeletalMesh(ConstructorHelpers::FObjectFinder<USkeletalMesh>(
		TEXT("/Game/ALS/SkeletalMesh/Rifle/SK_Rifle")).Object);
}

// Called when the game starts or when spawned
void ALyraCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateGait(CurrentGait);
}

void ALyraCharacter::UpdateGait(EGait Gait)
{
	CurrentGait = Gait;
	auto GaitSetting = GaitSettings.Find(Gait);
	const auto CharMovement = GetCharacterMovement();
	if (GaitSetting && CharMovement)
	{
		if (CurrentGait == EGait::Crouch)
		{
			CharMovement->MaxWalkSpeedCrouched = GaitSetting->MaxWalkSpeed;
		}
		else
		{
			CharMovement->MaxWalkSpeed = GaitSetting->MaxWalkSpeed;
		}
		CharMovement->MaxAcceleration = GaitSetting->MaxAcceleration;
		CharMovement->BrakingDecelerationWalking = GaitSetting->BrakingDeceleration;
		CharMovement->BrakingFrictionFactor = GaitSetting->BrakingFrictionFactor;
		CharMovement->BrakingFriction = GaitSetting->BrakingFriction;
		CharMovement->bUseSeparateBrakingFriction = GaitSetting->UseSeperateBrakingFriction;
	}
	if (auto AnimInst = GetMesh()->GetAnimInstance())
	{
		if (AnimInst->GetClass()->ImplementsInterface(UAnimBPInterface::StaticClass()))
		{
			IAnimBPInterface::Execute_ReceivedCurrentGait(AnimInst, Gait);
		}
	}

	// 避免使用PropertyAccess结点
	// GetLinkedAnimInstances非常量重载为private，强制转换成const，以调用public版本
	// 现在使用常规做法，避免以下代码（过度复杂）
	/*
	const TArray<UAnimInstance*>& LinkedAnimInstances = ((const USkeletalMeshComponent*)GetMesh())->GetLinkedAnimInstances();
	for(auto li : LinkedAnimInstances)
	{
		if(li->GetClass()->ImplementsInterface(UAnimBPInterface::StaticClass()))
		{
			IAnimBPInterface::Execute_ReceivedCurrentGait(li, Gait);
		}
	}
	*/
}

void ALyraCharacter::ChangeWeapon()
{
	const auto& AttachmentRule = FAttachmentTransformRules::SnapToTargetIncludingScale;
	if (EquippedGun == EGun::UnArmed)
	{
		Pistol->AttachToComponent(GetMesh(), AttachmentRule, WeaponSocketName.PistolUnEquipped);
		Rifle->AttachToComponent(GetMesh(), AttachmentRule, WeaponSocketName.RifleUnEquipped);
	}
	else if (EquippedGun == EGun::Pistol)
	{
		Pistol->AttachToComponent(GetMesh(), AttachmentRule, WeaponSocketName.WeaponEquipped);
		Rifle->AttachToComponent(GetMesh(), AttachmentRule, WeaponSocketName.RifleUnEquipped);
	}
	else if (EquippedGun == EGun::Rifle)
	{
		Pistol->AttachToComponent(GetMesh(), AttachmentRule, WeaponSocketName.PistolUnEquipped);
		Rifle->AttachToComponent(GetMesh(), AttachmentRule, WeaponSocketName.WeaponEquipped);
	}
}

// Called every frame
void ALyraCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 只在空中才查询与地面的距离
	UWorld* World = GetWorld();
	if (GetCharacterMovement()->IsFalling() && World)
	{
		// 查询与地面的距离
		FHitResult HitResult;
		// 找到胶囊体的最底部
		const FVector Start = GetActorLocation() - FVector(
			0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		const FVector End = Start + FVector(0.f, 0.f, -10000.f);
		const float Radius = 5.f;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);

		// 用球形查询离地面的距离
		//UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, 50.f, ECollisionChannel::ECC_Visibility, false,
		//	                                        TArray<AActor*>(), EDrawDebugTrace::ForDuration, HitResult, true);

		bool const bHit = World->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, ECC_Visibility,
		                                              FCollisionShape::MakeSphere(Radius), CollisionParams);

#if ENABLE_DRAW_DEBUG
		DrawDebugSphereTraceSingle(World, Start, End, Radius, EDrawDebugTrace::ForOneFrame, bHit, HitResult,
		                           FColor::Green, FColor::Red, 0.f);
#endif
		if (bHit)
		{
			// 计算与地面的距离
			const float DistanceToGround = HitResult.Distance;
			UE_LOG(LogLyraALS, Log, TEXT("与地面的距离：%f"), DistanceToGround);
			if (auto AnimInst = GetMesh()->GetAnimInstance())
			{
				if (AnimInst->GetClass()->ImplementsInterface(UAnimBPInterface::StaticClass()))
				{
					IAnimBPInterface::Execute_ReceivedGroundDistance(AnimInst, DistanceToGround);
				}
			}
		}
	}
}

// Called to bind functionality to input
void ALyraCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC_ALS, 0);
		}
	}

	// Set up action bindings
	auto EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent)
	{
		UE_LOG(LogLyraALS, Error, TEXT("'%s' 增强输入组件未绑定到玩家输入组件。请确保在编辑器中启用了增强输入系统。"), *GetNameSafe(this));
		return;
	}


	// Jumping
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

	// Moving
	EnhancedInputComponent->BindActionValueLambda(MoveAction, ETriggerEvent::Triggered,
	                                              [this](const FInputActionValue& Value)
	                                              {
		                                              FVector2D MovementVector = Value.Get<FVector2D>();
		                                              const float Displacement = MovementVector.Size();
		                                              if (Controller == nullptr ||
			                                              FMath::IsNearlyZero(Displacement))
			                                              return;

		                                              // find out which way is forward
		                                              const FRotator Rotation = Controller->
			                                              GetControlRotation();
		                                              const FRotator YawRotation(0, Rotation.Yaw, 0);
		                                              auto MoveDirection = YawRotation.RotateVector(FVector{
			                                              MovementVector.Y, MovementVector.X, 0.f
		                                              });

		                                              // add movement 
		                                              AddMovementInput(MoveDirection, Displacement);
	                                              });
	// Looking
	EnhancedInputComponent->BindActionValueLambda(LookAction, ETriggerEvent::Triggered,
	                                              [this](const FInputActionValue& Value)
	                                              {
		                                              // input is a Vector2D
		                                              FVector2D LookAxisVector = Value.Get<FVector2D>();

		                                              if (Controller != nullptr)
		                                              {
			                                              // add yaw and pitch input to controller
			                                              AddControllerYawInput(LookAxisVector.X);
			                                              AddControllerPitchInput(LookAxisVector.Y);
		                                              }
	                                              });

	// Switching weapons
	EnhancedInputComponent->BindActionValueLambda(SwitchWeaponAction, ETriggerEvent::Started,
	                                              [this](const FInputActionValue& Value)
	                                              {
		                                              // input is a float
		                                              int SwitchWeaponAxis = static_cast<int>(Value.Get<float>());
		                                              check(SwitchWeaponAxis == 1 || SwitchWeaponAxis == 2 ||
			                                              SwitchWeaponAxis == 3);
		                                              SwitchWeaponAxis -= 1;
		                                              GetMesh()->LinkAnimClassLayers(AnimLayers[SwitchWeaponAxis]);
		                                              EquippedGun = static_cast<EGun>(SwitchWeaponAxis);
		                                              if (auto AnimInst = GetMesh()->GetAnimInstance())
		                                              {
			                                              check(AnimInst->GetClass()->ImplementsInterface(
				                                              UAnimBPInterface::StaticClass()));
			                                              IAnimBPInterface::Execute_ReceivedEquippedGun(
				                                              AnimInst, EquippedGun);
		                                              }

		                                              ChangeWeapon();
	                                              });

	// Aiming
	EnhancedInputComponent->BindActionValueLambda(AimAction, ETriggerEvent::Started,
	                                              [this](const FInputActionValue& Value)
	                                              {
		                                              UpdateGait(EGait::Walking);
	                                              });

	EnhancedInputComponent->BindActionValueLambda(AimAction, ETriggerEvent::Completed,
	                                              [this](const FInputActionValue& Value)
	                                              {
		                                              UpdateGait(EGait::Jogging);
	                                              });

	// Crouching
	EnhancedInputComponent->BindActionValueLambda(CrouchAction, ETriggerEvent::Started,
	                                              [this](const FInputActionValue& Value)
	                                              {
		                                              if (CurrentGait == EGait::Crouch)
		                                              {
			                                              UpdateGait(EGait::Jogging);
			                                              UnCrouch();
		                                              }
		                                              else
		                                              {
			                                              UpdateGait(EGait::Crouch);
			                                              Crouch();
		                                              }
	                                              });
}
