// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/LyraCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Interface/LyraAnimationInterface.h"
// Sets default values
ALyraCharacter::ALyraCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character
	CameraBoom->SocketOffset = FVector{0.f, 100.f, 150.f}; // The camera is at this offset from the boom
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

void ALyraCharacter::BeginPlay()
{
	Super::BeginPlay();
	// 强制根据当前的状态切换武器
	OnEquippedGunChanged();
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

		auto Lambda = [this](const FInputActionValue& Value, const EGunTypes GunType)
		{
			// 类型没有改变，不需要切换
			if (GunType == EquippedGunType) return;
			EquippedGunType = GunType;
			OnEquippedGunChanged();
			
			/*
			const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EGunTypes"), true);
			auto DisplayName = EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(EquippedGunType)).ToString();
			UE_LOG(LogTemp, Warning, TEXT("当前武器类型：%s"), *DisplayName);
			*/
		};
		// 键盘1，2，3
		EnhancedInputComponent->BindActionValueLambda(Key1Action, ETriggerEvent::Started, Lambda, EGunTypes::UnArmed);
		EnhancedInputComponent->BindActionValueLambda(Key2Action, ETriggerEvent::Started, Lambda, EGunTypes::Pistol);
		EnhancedInputComponent->BindActionValueLambda(Key3Action, ETriggerEvent::Started, Lambda, EGunTypes::Rifle);
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
