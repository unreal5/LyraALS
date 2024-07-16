// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/AnimInst_LayerBase.h"

#include "LyraBaseAnimInst.h"
#include "SequencePlayerLibrary.h"
#include "Animation/AnimNodeReference.h"

ULyraBaseAnimInst* UAnimInst_LayerBase::GetABPBase() const
{
	auto OwningComponent = GetOwningComponent();
	if (OwningComponent)
	{
		auto MainAnimInst = OwningComponent->GetAnimInstance();
		return Cast<ULyraBaseAnimInst>(MainAnimInst);
	}
	return nullptr;
}

void UAnimInst_LayerBase::Idle_UpdateAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult Result;
	FSequencePlayerReference SequencePlayer = USequencePlayerLibrary::ConvertToSequencePlayer(Node, Result);
	if (Result == EAnimNodeReferenceConversionResult::Succeeded)
	{
		USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, SequencePlayer, IdleAnimation, 0.2f);
	}
}

void UAnimInst_LayerBase::Cycle_OnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult Result;
	FSequencePlayerReference SequencePlayer = USequencePlayerLibrary::ConvertToSequencePlayer(Node, Result);
	if (Result != EAnimNodeReferenceConversionResult::Succeeded) return;
	ULyraBaseAnimInst* ABPBase = GetABPBase();
	if (!ABPBase) return;
	EGait CurrentGait = ABPBase->CurrentGait;
	ELocomotionDirection CurrentDirection = ABPBase->VelocityLocomotionDirection;
	const FDirectionalAnimationSet& CycleAnim = CurrentGait == EGait::Walking ? WalkCycleAnimationSet : JogCycleAnimationSet;
	UAnimSequenceBase* Sequence = nullptr;
	switch (CurrentDirection)
	{
		case ELocomotionDirection::Forward:
			Sequence = CycleAnim.ForwardAnim.Get();
			break;
		case ELocomotionDirection::Backward:
			Sequence = CycleAnim.BackwardAnim.Get();
			break;
		case ELocomotionDirection::Left:
			Sequence = CycleAnim.LeftAnim.Get();
			break;
		case ELocomotionDirection::Right:
			Sequence = CycleAnim.RightAnim.Get();
			break;
	}
	USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, SequencePlayer, Sequence, 0.2f);
}
