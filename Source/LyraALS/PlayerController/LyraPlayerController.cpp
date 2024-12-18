// Lyra高级运动系统


#include "PlayerController/LyraPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/LyraCharacter.h"
#include "GameFramework/Character.h"

void ALyraPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	LyraCharacter = Cast<ALyraCharacter>(aPawn);
	checkf(LyraCharacter, TEXT("ALyraPlayerController::OnPossess: LyraCharacter is nullptr"));
}

void ALyraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//鼠标相关
	//bShowMouseCursor = true;
	//DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);


	auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	//check(Subsystem);
	// 对于Dedicated Server，Subsystem是空的
	if (Subsystem != nullptr)
	{
		Subsystem->AddMappingContext(DefaultInputContext, 0);
	}
}

void ALyraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	auto IC = CastChecked<UEnhancedInputComponent>(InputComponent);

	check(MoveAction);
	// lambda表达式
	auto MoveFunctor = [this](const FInputActionValue& Value)-> void
	{
		// 通过Value来控制移动
		const auto InputAxisVector = Value.Get<FVector2D>();
		// 禁止对角移动比前后左右移动更快
		const auto Distance = FMath::Clamp(InputAxisVector.Size(), 0.f, 1.f);
		FRotator YawRotation{0.f, GetControlRotation().Yaw, 0.f};
		FVector Direction{InputAxisVector.Y, InputAxisVector.X, 0.f};
		Direction = YawRotation.RotateVector(Direction);

		if (auto ControlledPawn = GetPawn<APawn>())
		{
			ControlledPawn->AddMovementInput(Direction, Distance);
		}
	};
	IC->BindActionValueLambda(MoveAction, ETriggerEvent::Triggered, MoveFunctor);


	check(LookUpAction);
	// lambda表达式
	auto LookUpFunctor = [this](const FInputActionValue& Value)-> void
	{
		FVector2D LookAxisVector = Value.Get<FVector2D>();
		// add yaw and pitch input to controller
		AddYawInput(LookAxisVector.X);
		AddPitchInput(LookAxisVector.Y);
	};
	IC->BindActionValueLambda(LookUpAction, ETriggerEvent::Triggered, LookUpFunctor);

	// Jumping
	IC->BindActionValueLambda(JumpAction, ETriggerEvent::Started, [this](auto)
	{
		if (auto ControlledChar = GetPawn<ACharacter>())
		{
			ControlledChar->Jump();
		}
	});
	IC->BindActionValueLambda(JumpAction, ETriggerEvent::Completed, [this](auto)
	{
		if (auto ControlledChar = GetPawn<ACharacter>())
		{
			ControlledChar->StopJumping();
		}
	});

	// UE5.5中，只有ETriggerEvent::Triggered触发，才可以获取到Value！！！
	// 5.4中使用ETriggerEvent::Started，在UE5.5中，永远得到0
	// 查看代码5.4.4中，代码和5.5.0中的代码一样
	/*
	 *	FInputActionValue GetValue() const { return TriggerEvent == ETriggerEvent::Triggered ? Value : FInputActionValue(Value.GetValueType(), FInputActionValue::Axis3D::ZeroVector); }
	 */
	IC->BindActionInstanceLambda(SwitchWeaponAction, ETriggerEvent::Triggered, [this](const FInputActionInstance& Value)
	{
		//auto RealValue = Value.GetValue();
		FVector SwitchAxisVector = Value.GetValue().Get<FVector>();
		uint8 WeaponIndex = uint8(FMath::RoundToInt32(SwitchAxisVector.X) - 1);
		if (WeaponIndex >= uint8(EGuns::EG_Max) || WeaponIndex < 0)
		{
			check(0);
			return;
		}
		EGuns NewWeapon = EGuns(WeaponIndex);

		if (auto LyraChar = GetPawn<ALyraCharacter>())
		{
			LyraChar->SetEquippedGun(NewWeapon);
		}
	});

	// 绑定瞄准
	IC->BindActionValueLambda(AimAction, ETriggerEvent::Started, [this](const FInputActionValue& Value)
	{
		if (auto LyraChar = GetPawn<ALyraCharacter>())
		{
			LyraChar->SetAiming(true);
		}
	});
	IC->BindActionValueLambda(AimAction, ETriggerEvent::Completed, [this](const FInputActionValue& Value)
	{
		if (auto LyraChar = GetPawn<ALyraCharacter>())
		{
			LyraChar->SetAiming(false);
		}
	});

	// 蹲下/起立
	IC->BindActionValueLambda(CrouchAction, ETriggerEvent::Started, [this](const FInputActionValue& Value)
	{
		if (auto LyraChar = GetPawn<ALyraCharacter>())
		{
			LyraChar->UpdateGait(EGait::Crouching);
		}
	});
}
