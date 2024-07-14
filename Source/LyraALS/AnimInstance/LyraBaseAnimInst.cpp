// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/LyraBaseAnimInst.h"

#include "SequencePlayerLibrary.h"
#include "GameFramework/Character.h"

void ULyraBaseAnimInst::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}

UCharacterMovementComponent* ULyraBaseAnimInst::GetCharacterMovementComponent() const
{
	ACharacter* Character = Cast<ACharacter>(GetOwningActor());
	return Character ? Character->GetCharacterMovement() : nullptr;
}

void ULyraBaseAnimInst::IdleOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult Result;
	auto SequencePlayer = USequencePlayerLibrary::ConvertToSequencePlayer(Node, Result);
	if (Result == EAnimNodeReferenceConversionResult::Succeeded)
	{
		//USequencePlayerLibrary::SetSequence(SequencePlayer, IdleSequence);
		UAnimSequenceBase* SelectedSequence = nullptr;
		switch (EquippedGun)
		{
			case EGun::UnArmed:
				SelectedSequence = IdleSequence;
				break;
			case EGun::Pistol:
				SelectedSequence = PistolIdleSequence;
				break;
			case EGun::Rifle:
				SelectedSequence = RifleIdleSequence;
				break;
		}
		USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, SequencePlayer, SelectedSequence, 0.2f);
	}
}
