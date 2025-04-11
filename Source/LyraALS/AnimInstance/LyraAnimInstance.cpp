// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/LyraAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UCharacterMovementComponent* ULyraAnimInstance::GetLyraCharacterMovementComponent() const
{
	if (auto Char = Cast<ACharacter>(GetOwningActor()))
	{
		return Char->GetCharacterMovement();
	}
	return nullptr;
}

void ULyraAnimInstance::GetVelocityData()
{
	auto CharMovementComp = GetLyraCharacterMovementComponent();
	if (!CharMovementComp) return;
	
	CharacterVelocity = CharMovementComp->Velocity;
	CharacterVelocity2D = FVector(CharacterVelocity.X, CharacterVelocity.Y, 0.f);
	CharacterSpeed = CharacterVelocity.Size();
}
