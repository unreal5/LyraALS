// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LyraCharacter.h"
#include "LyraALS.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"

#include "Components/CapsuleComponent.h"
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
}

// Called when the game starts or when spawned
void ALyraCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALyraCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	}
	else
	{
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

			                                              EquippedGun = static_cast<EGun>(SwitchWeaponAxis - 1);
		                                              	auto AnimInst=GetMesh()->GetAnimInstance();
		                                              	if(AnimInst)
		                                              	{
		                                              		check(AnimInst->GetClass()->ImplementsInterface(UAnimBPInterface::StaticClass()));
		                                              		IAnimBPInterface::Execute_ReceivedEquippedGun(AnimInst,EquippedGun);
		                                              	}
		                                              });
	}
}
