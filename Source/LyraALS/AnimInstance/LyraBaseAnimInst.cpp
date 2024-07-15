// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/LyraBaseAnimInst.h"

#include "KismetAnimationLibrary.h"
#include "SequencePlayerLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void ULyraBaseAnimInst::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	GetVelocityData();
	GetLocationData();
	GetRotationData();

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
}

void ULyraBaseAnimInst::GetLocationData()
{
	AActor* OwningActor = GetOwningActor();
	if (!OwningActor) return;

	WorldLocation = OwningActor->GetActorLocation();
}

void ULyraBaseAnimInst::GetRotationData()
{
	AActor* OwningActor = GetOwningActor();
	if (!OwningActor) return;

	WorldRotation = OwningActor->GetActorRotation();
}

void ULyraBaseAnimInst::UpdateOrientation(float DeltaTime)
{
	VelocityLocomotionAngle = UKismetAnimationLibrary::CalculateDirection(CharacterVelocity2D, WorldRotation);
}
