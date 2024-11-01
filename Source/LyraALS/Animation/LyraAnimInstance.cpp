// Lyra高级运动系统


#include "Animation/LyraAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "Character/LyraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void ULyraAnimInstance::NativeInitializeAnimation()
{
	LyraCharacter = Cast<ALyraCharacter>(GetOwningActor());
	if (LyraCharacter)
	{
		LyraCharacterMovement = LyraCharacter->GetCharacterMovement();
	}
}

void ULyraAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	DrawDebugMessage(DeltaSeconds);
}

UCharacterMovementComponent* ULyraAnimInstance::GetCharacterMovement()
{
	if (nullptr != LyraCharacterMovement)
	{
		return LyraCharacterMovement;
	}
	// 尝试重新获取
	if (nullptr == LyraCharacter)
	{
		if (GetLyraCharacter())
		{
			LyraCharacterMovement = LyraCharacter->GetCharacterMovement();
		}
	}

	return LyraCharacterMovement;
}

ALyraCharacter* ULyraAnimInstance::GetLyraCharacter()
{
	if (nullptr == LyraCharacter)
	{
		LyraCharacter = Cast<ALyraCharacter>(GetOwningActor());
	}
	return LyraCharacter;
}

void ULyraAnimInstance::ReceiveEquippedGun_Implementation(EGuns NewEquippedGun)
{
	EquippedGun = NewEquippedGun;
}

void ULyraAnimInstance::ReceiveCurrentGait_Implementation(EGait NewGait)
{
	CurrentGait = NewGait;
	UE_LOG(LogTemp, Warning, TEXT("当前步态: %s"), *UEnum::GetValueAsString(CurrentGait));
}

void ULyraAnimInstance::GetVelocityData()
{
	if (const auto CharacterMovement = GetCharacterMovement())
	{
		CharacterVelocity = CharacterMovement->Velocity;
		CharacterVelocity2D = FVector(CharacterVelocity.X, CharacterVelocity.Y, 0.f);
		CharacterSpeed2D = CharacterVelocity2D.Size();
	}
}

void ULyraAnimInstance::GetLocationData()
{
	auto LyraChar = GetLyraCharacter();
	if (LyraChar)
	{
		WorldLocation = LyraChar->GetActorLocation();
	}
}

void ULyraAnimInstance::UpdateOrientationData()
{
	VelocityLocomotionAngle = UKismetAnimationLibrary::CalculateDirection(CharacterVelocity2D, WorldRotation);
}

void ULyraAnimInstance::GetRotationData()
{
	auto LyraChar = GetLyraCharacter();
	if (LyraChar)
	{
		WorldRotation = LyraChar->GetActorRotation();
	}
}

void ULyraAnimInstance::CalculateLocomotionDirection(float CurrentLocomotionAngle,
                                                     ELocomotionDirection CurrentDirection, float DeadZone,
                                                     float ForwardThreshold,
                                                     float BackwardThreshold)
{
	const auto AbsLocomotionAngle = FMath::Abs(CurrentLocomotionAngle);
	// 修正死区
	if (CurrentDirection == ELocomotionDirection::Forward && AbsLocomotionAngle <= (ForwardThreshold + DeadZone))
	{
		ForwardThreshold += DeadZone;
	}
	else if (AbsLocomotionAngle >= (BackwardThreshold + DeadZone))
	{
		BackwardThreshold -= DeadZone;
	}

	if (AbsLocomotionAngle <= ForwardThreshold)
	{
		LocomotionDirection = ELocomotionDirection::Forward;
	}
	else if (AbsLocomotionAngle >= BackwardThreshold)
	{
		LocomotionDirection = ELocomotionDirection::Backward;
	}
	else if (CurrentLocomotionAngle > 0.f)
	{
		LocomotionDirection = ELocomotionDirection::Right;
	}
	else
	{
		LocomotionDirection = ELocomotionDirection::Left;
	}
}