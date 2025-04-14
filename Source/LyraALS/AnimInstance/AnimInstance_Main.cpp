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
}
