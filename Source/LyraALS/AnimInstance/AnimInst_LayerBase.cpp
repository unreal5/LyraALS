// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/AnimInst_LayerBase.h"

#include "AnimCharacterMovementLibrary.h"
#include "AnimDistanceMatchingLibrary.h"
#include "LyraBaseAnimInst.h"
#include "SequenceEvaluatorLibrary.h"
#include "SequencePlayerLibrary.h"
#include "Animation/AnimNodeReference.h"
#include "GameFramework/CharacterMovementComponent.h"

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

void UAnimInst_LayerBase::Start_BecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
}

void UAnimInst_LayerBase::Start_OnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
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
	UAnimSequenceBase* Sequence = SelectAnimSequeceFromAnimSets(WalkCycleAnimationSet, JogCycleAnimationSet,
	                                                            CurrentGait, CurrentDirection);
	USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, SequencePlayer, Sequence, 0.2f);
}

void UAnimInst_LayerBase::Stop_BecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	ULyraBaseAnimInst* ABPBase = GetABPBase();
	if (!ABPBase) return;

	EAnimNodeReferenceConversionResult Result;
	FSequenceEvaluatorReference SequenceEvaluatorReference =
		USequenceEvaluatorLibrary::ConvertToSequenceEvaluator(Node, Result);
	if (Result != EAnimNodeReferenceConversionResult::Succeeded) return;


	// 选择动画
	EGait CurrentGait = ABPBase->CurrentGait;
	ELocomotionDirection CurrentDirection = ABPBase->VelocityLocomotionDirection;

	UAnimSequenceBase* Sequence = SelectAnimSequeceFromAnimSets(WalkStopAnimationSet, JogStopAnimationSet,
	                                                            CurrentGait, CurrentDirection);

	USequenceEvaluatorLibrary::SetSequence(SequenceEvaluatorReference, Sequence);
	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluatorReference, 0.f);

	// UE_LOG(LogLyraALS, Log, TEXT("Stop_BecomeRelevant: %s"), *Sequence->GetName());
}

void UAnimInst_LayerBase::Stop_OnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult Result;
	FSequenceEvaluatorReference SequenceEvaluatorReference =
		USequenceEvaluatorLibrary::ConvertToSequenceEvaluator(Node, Result);
	if (Result != EAnimNodeReferenceConversionResult::Succeeded) return;

	ULyraBaseAnimInst* ABPBase = GetABPBase();
	if (!ABPBase) return;
	UCharacterMovementComponent* MovementComponent = ABPBase->GetCharacterMovementComponent();
	if (!MovementComponent) return;
	// 进行距离匹配

	// 预测相对停止位置
	FVector StopLocaton = UAnimCharacterMovementLibrary::PredictGroundMovementStopLocation(ABPBase->CharacterVelocity2D,
		MovementComponent->bUseSeparateBrakingFriction,
		MovementComponent->BrakingFriction,
		MovementComponent->GroundFriction,
		MovementComponent->BrakingFrictionFactor,
		MovementComponent->BrakingDecelerationWalking);

	float StopDistance = StopLocaton.Size2D();
	if (StopDistance > 0.f) // 距离匹配
	{
		UAnimDistanceMatchingLibrary::DistanceMatchToTarget(SequenceEvaluatorReference, StopDistance,TEXT("Distance"));
	}
	else
	{
		// 已经到达目标，正常播放动画
		USequenceEvaluatorLibrary::AdvanceTime(Context, SequenceEvaluatorReference);
	}
}

UAnimSequenceBase* UAnimInst_LayerBase::SelectAnimSequeceFromAnimSets(const FDirectionalAnimationSet& WalkAnimSet,
                                                                      const FDirectionalAnimationSet& JogAnimSet,
                                                                      EGait CurrentGait,
                                                                      ELocomotionDirection CurrentDirection) const
{
	auto&& SelectedAnimSet = CurrentGait == EGait::Walking ? WalkAnimSet : JogAnimSet;
	switch (CurrentDirection)
	{
	case ELocomotionDirection::Forward:
		return SelectedAnimSet.ForwardAnim.Get();
	case ELocomotionDirection::Backward:
		return SelectedAnimSet.BackwardAnim.Get();
	case ELocomotionDirection::Left:
		return SelectedAnimSet.LeftAnim.Get();
	case ELocomotionDirection::Right:
		return SelectedAnimSet.RightAnim.Get();
	default:
		return nullptr;
	}
}
