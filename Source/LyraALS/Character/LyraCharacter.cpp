// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/LyraCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Interface/LyraAnimationInterface.h"
#include "Kismet/KismetSystemLibrary.h"
// Sets default values
ALyraCharacter::ALyraCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character
	CameraBoom->SocketOffset = FVector{0.f, 100.f, 50.f}; // The camera is at this offset from the boom
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// 创建武器
	PistolMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PistolMesh"));
	PistolMesh->SetupAttachment(GetMesh(), TEXT("PistolUnEquipped"));
	PistolMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RifleMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RifleMesh"));
	RifleMesh->SetupAttachment(GetMesh(), TEXT("RifleUnEquipped"));
	RifleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ALyraCharacter::BeginPlay()
{
	Super::BeginPlay();
	// 强制根据当前的状态切换武器
	OnEquippedGunChanged();
	// 强制根据当前的状态切换速度相关的设置
	UpdateGait(CurrentGait);
}

void ALyraCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ALyraCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALyraCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALyraCharacter::Look);

		auto WeaponKeyLambda = [this](const FInputActionValue& Value, const EGunTypes GunType)
		{
			// 类型没有改变，不需要切换
			if (GunType == EquippedGunType) return;
			EquippedGunType = GunType;
			OnEquippedGunChanged();
			ChangeWeapon(EquippedGunType);
			/*
			const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EGunTypes"), true);
			auto DisplayName = EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(EquippedGunType)).ToString();
			UE_LOG(LogTemp, Warning, TEXT("当前武器类型：%s"), *DisplayName);
			*/
		};
		// 键盘1，2，3
		EnhancedInputComponent->BindActionValueLambda(Key1Action, ETriggerEvent::Started, WeaponKeyLambda,
		                                              EGunTypes::UnArmed);
		EnhancedInputComponent->BindActionValueLambda(Key2Action, ETriggerEvent::Started, WeaponKeyLambda,
		                                              EGunTypes::Pistol);
		EnhancedInputComponent->BindActionValueLambda(Key3Action, ETriggerEvent::Started, WeaponKeyLambda,
		                                              EGunTypes::Rifle);

		// Crouch
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ThisClass::OnCrouch);

		auto AimingLambda = [this](auto, bool bIsAiming)
		{
			auto Gait = bIsAiming ? EGait::Walking : EGait::Jogging;
			if (Gait != CurrentGait)
			{
				UpdateGait(Gait);
			}
		};
		EnhancedInputComponent->BindActionValueLambda(AimAction, ETriggerEvent::Started, AimingLambda, true);
		EnhancedInputComponent->BindActionValueLambda(AimAction, ETriggerEvent::Completed, AimingLambda, false);


		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ThisClass::OnFire);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' 查找增强型输入组件失败，请确保在项目设置中启用增强型输入插件。"), *GetNameSafe(this));
	}
}

bool ALyraCharacter::OnEquippedGunChanged()
{
	if (auto AnimInterface = GetMesh()->GetAnimInstance())
	{
		if (auto LinkAnimClassItem = LinkAnimClassMap.Find(EquippedGunType))
		{
			GetMesh()->LinkAnimClassLayers(*LinkAnimClassItem);

			ILyraAnimationInterface::Execute_ReceiveEquippedGun(AnimInterface, EquippedGunType);
			return true;
		}
	}
	checkf(false, TEXT("AnimInstance is null or link anim class item is empty"));
	return false;
}

bool ALyraCharacter::UpdateGait(EGait NewGait)
{
	CurrentGait = NewGait;
	if (auto AnimInstance = GetMesh()->GetAnimInstance())
	{
		ILyraAnimationInterface::Execute_ReceiveCurrentGait(AnimInstance, CurrentGait);
	}
	if (auto GaitSetting = GaitSettingsMap.Find(CurrentGait))
	{
		if (auto CharMovementComp = GetCharacterMovement())
		{
			CharMovementComp->MaxWalkSpeed = GaitSetting->MaxWalkSpeed;
			CharMovementComp->MaxAcceleration = GaitSetting->MaxAcceleration;
			CharMovementComp->BrakingDecelerationWalking = GaitSetting->BrakingDeceleration;
			CharMovementComp->BrakingFrictionFactor = GaitSetting->BrakingFrictionFactor;
			CharMovementComp->BrakingFriction = GaitSetting->BrakingFriction;
			CharMovementComp->bUseSeparateBrakingFriction = GaitSetting->UseSeparateBrakingFriction;
			// Crouch时的速度
			CharMovementComp->MaxWalkSpeedCrouched = GaitSetting->MaxWalkSpeed;
			return true;
		}
	}
	return false;
}

void ALyraCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ALyraCharacter::ChangeWeapon(EGunTypes GunType)
{
	// 以下代码较笨拙，只是为了快速实现功能
	// 卸下所有武器
	PistolMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
	                              WeaponSockets.PistolUnEquipped);
	RifleMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
	                             WeaponSockets.RifleUnEquipped);

	FName SocketName;
	USkeletalMeshComponent* WeaponEquipped = nullptr;
	switch (GunType)
	{
	case EGunTypes::UnArmed:
		// 已经卸下所有武器
		break;
	case EGunTypes::Pistol:
		SocketName = WeaponSockets.WeaponEquipped;
		WeaponEquipped = PistolMesh;
		break;
	case EGunTypes::Rifle:
		SocketName = WeaponSockets.WeaponEquipped;
		WeaponEquipped = RifleMesh;
		break;
	default:
		check(0 && TEXT("不支持的武器类型"));
		break;
	}
	if (WeaponEquipped)
	{
		WeaponEquipped->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		                                  SocketName);
	}
}

void ALyraCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	FVector Direction = FVector(MovementVector.Y, MovementVector.X, 0);
	if (Direction.IsNearlyZero() || !Controller) return;

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// transform direction from camera space to world space
	Direction = YawRotation.RotateVector(Direction);
	AddMovementInput(Direction);
}

void ALyraCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ALyraCharacter::OnCrouch(const FInputActionValue& Value)
{
	switch (CurrentGait)
	{
	case EGait::Crouch:
		UpdateGait(EGait::Jogging);
		UnCrouch();
		break;
	case EGait::Jogging:
	// fall through
	case EGait::Walking:
		UpdateGait(EGait::Crouch);
		Crouch();
		break;
	}
}

void ALyraCharacter::OnFire(const FInputActionValue& Value)
{
	// 在步行和蹲下状态下，并持枪才可以开枪
	if (CurrentGait == EGait::Jogging || EquippedGunType == EGunTypes::UnArmed) return;
	if (!CanFire) return;

	CanFire = false;
	const float FireRate = EquippedGunType == EGunTypes::Pistol ? 0.5f : 0.1f;

	GetWorldTimerManager().SetTimer(FireTimer, [this]()
	                                {
		                                CanFire = true;
		                                /*FString Msg = EquippedGunType == EGunTypes::Pistol
			                                              ? TEXT("手枪开火")
			                                              : TEXT("步枪开火");
		                                UKismetSystemLibrary::PrintString(
			                                this, Msg, true, false, FLinearColor::Red, 2.0f);
			                                		                                */
		                                UAnimMontage* FireMontage = EquippedGunType == EGunTypes::Pistol
			                                                            ? PistolFireMontage
			                                                            : RifleFireMontage;
		                                if (FireMontage)
		                                {
			                                PlayAnimMontage(FireMontage);
		                                }
	                                },
	                                FireRate,
	                                false);
}
