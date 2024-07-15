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
	UAnimSequenceBase* Sequence = CurrentGait == EGait::Walking ? CycleWalkAnim : CycleJoggingAnim;
	USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, SequencePlayer, Sequence, 0.2f);
}
