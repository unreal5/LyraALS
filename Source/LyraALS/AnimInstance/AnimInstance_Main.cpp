// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/AnimInstance_Main.h"

#include "KismetAnimationLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UCharacterMovementComponent* UAnimInstance_Main::GetCharacterMovementComponent() const
{
	if (auto Char = Cast<ACharacter>(GetOwningActor()))
	{
		return Char->GetCharacterMovement();
	}
	return nullptr;
}

void UAnimInstance_Main::GetVelocityData()
{
	auto CharMovementComp = GetCharacterMovementComponent();
	if (!CharMovementComp) return;

	CharacterVelocity = CharMovementComp->Velocity;
	CharacterVelocity2D = FVector(CharacterVelocity.X, CharacterVelocity.Y, 0.f);
	CharacterSpeed = CharacterVelocity.Size();
}

void UAnimInstance_Main::GetLocationData()
{
	auto OwningActor = GetOwningActor();
	if (!OwningActor) return;

	WorldLocation = OwningActor->GetActorLocation();
}

void UAnimInstance_Main::GetRotationData()
{
	auto OwningActor = GetOwningActor();
	if (!OwningActor) return;

	WorldRotation = OwningActor->GetActorRotation();
}

void UAnimInstance_Main::UpdateOrientationData()
{
	VelocityLocomotionangle = UKismetAnimationLibrary::CalculateDirection(CharacterVelocity2D, WorldRotation);
	VelocityLocomotionDirection = CalculateLocomotionDirection(VelocityLocomotionDirection, VelocityLocomotionangle);
}

ELocomotionDirection UAnimInstance_Main::CalculateLocomotionDirection(
	const ELocomotionDirection& CurrentLocomotionDirection, float CurrentLocomotionAngle, float BackwardThreshold,
	float ForwardThreshold, const float DeadZone)
{
	const float AbsAngle = FMath::Abs(CurrentLocomotionAngle);

	// 对于前后向，扩大阀值
	if (CurrentLocomotionDirection == ELocomotionDirection::Backward)
	{
		BackwardThreshold -= DeadZone;
	}
	else if (CurrentLocomotionDirection == ELocomotionDirection::Forward)
	{
		ForwardThreshold += DeadZone;
	}
	else if (CurrentLocomotionDirection == ELocomotionDirection::Right)
	{
		BackwardThreshold += DeadZone;
	}
	else
	{
		ForwardThreshold -= DeadZone;
	}

	if (AbsAngle >= BackwardThreshold)
	{
		return ELocomotionDirection::Backward;
	}
	if (AbsAngle <= ForwardThreshold)
	{
		return ELocomotionDirection::Forward;
	}
	return CurrentLocomotionAngle > 0.f ? ELocomotionDirection::Right : ELocomotionDirection::Left;
}
