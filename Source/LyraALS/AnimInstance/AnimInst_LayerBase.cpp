// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/AnimInst_LayerBase.h"

#include "AnimCharacterMovementLibrary.h"
#include "AnimDistanceMatchingLibrary.h"
#include "LyraALS.h"
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
	EAnimNodeReferenceConversionResult Result;
	FSequenceEvaluatorReference SeqEvaRef = USequenceEvaluatorLibrary::ConvertToSequenceEvaluator(Node, Result);
	if (Result != EAnimNodeReferenceConversionResult::Succeeded) return;

	ULyraBaseAnimInst* ABPBase = GetABPBase();
	if (!ABPBase) return;

	// 选择动画
	EGait CurrentGait = ABPBase->CurrentGait;
	ELocomotionDirection CurrentDirection = ABPBase->VelocityLocomotionDirection;
	auto Anim = SelectAnimSequeceFromAnimSets(WalkStartAnimationSet, JogStartAnimationSet, CurrentGait,
	                                          CurrentDirection);
	USequenceEvaluatorLibrary::SetSequence(SeqEvaRef, Anim);
	USequenceEvaluatorLibrary::SetExplicitTime(SeqEvaRef, 0.f);
}

void UAnimInst_LayerBase::Start_OnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult Result;
	FSequenceEvaluatorReference SeqEvaRef = USequenceEvaluatorLibrary::ConvertToSequenceEvaluator(Node, Result);
	if (Result != EAnimNodeReferenceConversionResult::Succeeded) return;

	ULyraBaseAnimInst* ABPBase = GetABPBase();
	if (!ABPBase) return;

	// 进行距离匹配

	UAnimDistanceMatchingLibrary::AdvanceTimeByDistanceMatching(Context, SeqEvaRef, ABPBase->DeltaLocation,
	                                                            TEXT("Distance"));
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

void UAnimInst_LayerBase::Pivot_BecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult Result;
	FSequenceEvaluatorReference SeqEvaRef = USequenceEvaluatorLibrary::ConvertToSequenceEvaluator(Node, Result);
	if (Result != EAnimNodeReferenceConversionResult::Succeeded) return;

	UAnimSequenceBase* AnimSequence = SelectPivotAnim();
	if (AnimSequence)
	{
		USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context, SeqEvaRef, AnimSequence);
	}
	USequenceEvaluatorLibrary::SetExplicitTime(SeqEvaRef, 0.f);

	if(const auto ABPBase = GetABPBase())
	{
		InitialAccWhenEnterPivot = ABPBase->Acceleration2D;
		UE_LOG(LogLyraALS, Log, TEXT("Pivot_BecomeRelevant->加速度 = %s"), *InitialAccWhenEnterPivot.ToString());
	}
	// 调试
	// const auto Msg =FString::Printf(TEXT("Pivot_BecomeRelevant->%s"), *Sequence->GetName());
	// UE_LOG(LogLyraALS, Log, TEXT("%s"), *Msg);
}

void UAnimInst_LayerBase::Pivot_OnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult Result;
	FSequenceEvaluatorReference SeqEvaRef = USequenceEvaluatorLibrary::ConvertToSequenceEvaluator(Node, Result);
	if (Result != EAnimNodeReferenceConversionResult::Succeeded) return;


	// Pivto动画分两个个部分：前半部分是停止动画，后半部分是Start动画；
	// 前半部分的时间是根据预测的停止位置匹配（DistanceMatchToTarget），后半部分的时间是根据每帧位移进行匹配(AdvanceTimeByDistanceMatching)。
	// 判断前半部分是否结束，可以利用加速度和速度的点积判定：小于0则两者不一致，位于停止部分（前半部分）。
	// 否则，位于后半部分。
	ULyraBaseAnimInst* ABPBase = GetABPBase();
	if (!ABPBase) return;
	const FVector Acc2D = ABPBase->Acceleration2D.GetSafeNormal();
	const FVector Vel2D = ABPBase->CharacterVelocity2D.GetSafeNormal();
	const float DotResult = FVector::DotProduct(Acc2D, Vel2D);
	if (DotResult < 0.f)
	{
		/* 位于前半部分（停止动画）*/

		/** 这种方式似乎比官方Lyra动画解决方案复杂 */

		// 1、检测在Pivot_BecomeRelevant中设置的动画是否与期望的动画一致，如果不一致则重新设置
		UAnimSequenceBase* CurrentAnimSeq = USequenceEvaluatorLibrary::GetSequence(SeqEvaRef);
		UAnimSequenceBase* SelectedAnimSeq = SelectPivotAnim();
		if (SelectedAnimSeq != CurrentAnimSeq)
		{
			USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context, SeqEvaRef, SelectedAnimSeq);
		}

		// 2、获取预测的停止位置
		UCharacterMovementComponent* MovementComponent = ABPBase->GetCharacterMovementComponent();
		if (!MovementComponent) return;

		FVector PivotLocaton = UAnimCharacterMovementLibrary::PredictGroundMovementPivotLocation(
			Acc2D, Vel2D, MovementComponent->GroundFriction);

		const float StopDistance = PivotLocaton.Size2D();

		UAnimDistanceMatchingLibrary::DistanceMatchToTarget(SeqEvaRef, StopDistance, TEXT("Distance"));
	}
	else
	{
		UAnimDistanceMatchingLibrary::AdvanceTimeByDistanceMatching(Context, SeqEvaRef, ABPBase->DeltaLocation,
		                                                            TEXT("Distance"));
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

UAnimSequenceBase* UAnimInst_LayerBase::SelectPivotAnim()
{
	ULyraBaseAnimInst* ABPBase = GetABPBase();
	if (!ABPBase) return nullptr;

	EGait CurrentGait = ABPBase->CurrentGait;
	ELocomotionDirection CurrentDirection = ABPBase->AccelerationLocomotionDirection;

	UAnimSequenceBase* Sequence = SelectAnimSequeceFromAnimSets(WalkPivotAnimationSet, JogPivotAnimationSet,
	                                                            CurrentGait, CurrentDirection);
	return Sequence;
}
