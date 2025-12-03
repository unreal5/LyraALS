// 版权没有，拷贝自由。


#include "LyraPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Enumeration/EnumTypes.h"


ALyraPlayerController::ALyraPlayerController()
{
}


void ALyraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (!IsLocalPlayerController()) return;

	auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!Subsystem) return;

	Subsystem->AddMappingContext(DefaultMappingContext, 0);
	// Bind Inputs
	auto EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent) return;

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALyraPlayerController::Move);
	EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this,
	                                   &ALyraPlayerController::MouseLook);
	EnhancedInputComponent->BindAction(SwitchWeaponAction, ETriggerEvent::Started, this,
	                                   &ALyraPlayerController::SwitchWeapon);


	auto AimBinding = [this](const FInputActionValue& ActionValue, bool bIsAiming)
	{
		EGaitType NewGait = bIsAiming ? EGaitType::Walking : EGaitType::Jogging;
		OnGaitChanged.Broadcast(NewGait);
	};
	EnhancedInputComponent->BindActionValueLambda(AimAction, ETriggerEvent::Triggered, AimBinding, true);
	EnhancedInputComponent->BindActionValueLambda(AimAction, ETriggerEvent::Completed, AimBinding, false);
}

void ALyraPlayerController::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	FVector MovementDirection{MovementVector.Y, MovementVector.X, 0.f};

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;
	FRotator YawRotation{0.f, GetControlRotation().Yaw, 0.f};
	MovementDirection = YawRotation.RotateVector(MovementDirection);
	ControlledPawn->AddMovementInput(MovementDirection, 1.f, false);
}

void ALyraPlayerController::MouseLook(const FInputActionValue& Value)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void ALyraPlayerController::SwitchWeapon(const FInputActionValue& Value)
{
	uint8 WeaponIndex = static_cast<uint8>(Value.Get<float>()) - 1;
	EGunType Gun = static_cast<EGunType>(WeaponIndex);
	OnSwitchWeapon.Broadcast(Gun);
}
