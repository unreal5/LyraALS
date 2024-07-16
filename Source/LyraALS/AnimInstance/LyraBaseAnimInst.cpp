// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/LyraBaseAnimInst.h"

#include "KismetAnimationLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void ULyraBaseAnimInst::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	GetVelocityData();
	GetLocationData();
	GetRotationData(DeltaSeconds);

	UpdateOrientation(DeltaSeconds);
}

UCharacterMovementComponent* ULyraBaseAnimInst::GetCharacterMovementComponent() const
{
	ACharacter* Character = Cast<ACharacter>(GetOwningActor());
	return Character ? Character->GetCharacterMovement() : nullptr;
}

void ULyraBaseAnimInst::GetVelocityData()
{
	auto CharacterMovementComponent = GetCharacterMovementComponent();
	if (!CharacterMovementComponent) return;

	CharacterVelocity = CharacterMovementComponent->Velocity;
	CharacterVelocity2D = FVector(CharacterVelocity.X, CharacterVelocity.Y, 0.f);
	CharacterSpeed2D = CharacterVelocity2D.Size2D();
}

void ULyraBaseAnimInst::GetLocationData()
{
	AActor* OwningActor = GetOwningActor();
	if (!OwningActor) return;

	WorldLocation = OwningActor->GetActorLocation();
}

void ULyraBaseAnimInst::GetRotationData(float DeltaSeconds)
{
	AActor* OwningActor = GetOwningActor();
	if (!OwningActor) return;

	float LastFrameActorYaw = WorldRotation.Yaw;
	WorldRotation = OwningActor->GetActorRotation();
	float DeltaActorYaw = UKismetMathLibrary::SafeDivide(WorldRotation.Yaw - LastFrameActorYaw, DeltaSeconds);
	// 如果向后运动，则角度取反。
	if(VelocityLocomotionDirection == ELocomotionDirection::Backward)
	{
		DeltaActorYaw = -DeltaActorYaw;
	}
	LeanAngle = UKismetMathLibrary::ClampAngle(DeltaActorYaw, -90.f, 90.f);
}

void ULyraBaseAnimInst::UpdateOrientation(float DeltaTime)
{
	VelocityLocomotionAngle = UKismetAnimationLibrary::CalculateDirection(CharacterVelocity2D, WorldRotation);

	VelocityLocomotionDirection = CalculateLocomotionDirection(VelocityLocomotionAngle, VelocityLocomotionDirection);
}

ELocomotionDirection ULyraBaseAnimInst::CalculateLocomotionDirection(float CurrentLocomotionAngle,
                                                                     ELocomotionDirection CurrentLocomotionDirection,
                                                                     float ForwardMin, float ForwardMax,
                                                                     float BackwardMin,
                                                                     float BackwardMax, float DeadZone)
{
	if (CurrentLocomotionDirection == ELocomotionDirection::Forward)
	{
		ForwardMin -= DeadZone;
		ForwardMin += DeadZone;
	}
	if (CurrentLocomotionDirection == ELocomotionDirection::Backward)
	{
		BackwardMin += DeadZone;
		BackwardMax -= DeadZone;
	}

	if (CurrentLocomotionAngle < BackwardMin || CurrentLocomotionAngle > BackwardMax)
	{
		return ELocomotionDirection::Backward;
	}

	if (CurrentLocomotionAngle > ForwardMin && CurrentLocomotionAngle < ForwardMax)
	{
		return ELocomotionDirection::Forward;
	}

	return CurrentLocomotionAngle > 0.f ? ELocomotionDirection::Right : ELocomotionDirection::Left;
}
