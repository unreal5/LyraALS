// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/AnimInstance_Layer.h"

#include "AnimExecutionContextLibrary.h"
#include "AnimInstance_Main.h"
#include "SequenceEvaluatorLibrary.h"
#include "SequencePlayerLibrary.h"
#include "Animation/AnimInstanceProxy.h"
#include "AnimNodes/AnimNode_SequenceEvaluator.h"

UAnimInstance_Main* UAnimInstance_Layer::GetABPBase() const
{
	return Cast<UAnimInstance_Main>(GetSkelMeshComponent()->GetAnimInstance());
}

void UAnimInstance_Layer::IdleOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequencePlayerReference sp;
	bool Result;
	USequencePlayerLibrary::ConvertToSequencePlayerPure(Node, sp, Result);
	if (Result)
	{
		USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, sp, IdleAnim);
	}
}

/* 如果只在BecomeRelevant中设置动画，则在Update时，如果切换状态，无法切换动画 */
/* 因此，需要在Update中，根据步态设置动画 */
/*
void UAnimInstance_Layer::CycleOnBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequencePlayerReference sp;
	bool Result;
	USequencePlayerLibrary::ConvertToSequencePlayerPure(Node, sp, Result);
	if (!Result) return;

	auto SelectedAnim = GetABPBase()->CurrentGait == EGait::Walking ? CycleWalkingAnim : CycleJoggingAnim;
	USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, sp, SelectedAnim);
	USequencePlayerLibrary::SetStartPosition(sp, 0.f);
}
*/
void UAnimInstance_Layer::CycleOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	auto ABPBase = GetABPBase();
	if (!ABPBase) return;
	// 获取当前步态
	const EGait CurrentGait = ABPBase->CurrentGait;
	// 获取当前运动方向
	const ELocomotionDirection CurrentLocomotionDirection = ABPBase->VelocityLocomotionDirection;

	FSequencePlayerReference sp;
	bool Result;
	USequencePlayerLibrary::ConvertToSequencePlayerPure(Node, sp, Result);
	if (!Result) return;

	// 获取当前动画的时间
	float CurrentTime = USequencePlayerLibrary::GetAccumulatedTime(sp);
	// 加上当前帧的DeltaTime

	// 根据步态和方向选择动画。
	const auto SelectedAnim = SelectAnimByGaitAndDirection(CurrentGait, CurrentLocomotionDirection, CycleAnimations);
	if (SelectedAnim)
	{
		USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, sp, SelectedAnim);
	}
	else
	{
		// do nothing.
	}

	//UAnimExecutionContextLibrary::GetDeltaTime(Context);
	//CurrentTime += Context.GetContext()->GetDeltaTime(); 
}

// 当相关时，判定使用哪个动画
void UAnimInstance_Layer::StopOnBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	auto ABPBase = GetABPBase();
	if (!ABPBase) return;


	// 当前使用分层动画，因此返回的是分层动画的实例，例如：UnArmed_Layer，……。
	// 仅供学习测试
	/*
	auto&& Proxy = GetProxyOnAnyThread<FAnimInstanceProxy>();
	auto LayerAnimInst = Cast<UAnimInstance_Layer>(Proxy.GetAnimInstanceObject());
	if (!LayerAnimInst) return;
	*/
	FSequenceEvaluatorReference SequenceEvaluator;
	bool Result;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, Result);
	if (!Result) return;
	auto SelectedAnim = SelectAnimByGaitAndDirection(ABPBase->CurrentGait, ABPBase->VelocityLocomotionDirection, StopAnimations);
	if (SelectedAnim)
	{
		USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context, SequenceEvaluator, SelectedAnim);
	}
	else
	{
		// do nothing.
	}
}

void UAnimInstance_Layer::StopOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
}

UAnimSequenceBase* UAnimInstance_Layer::SelectAnimByGaitAndDirection(const EGait& CurrentGait,
                                                                     const ELocomotionDirection&
                                                                     CurrentLocomotionDirection,
                                                                     const TMap<EGait, FDirectionalAnimation>&
                                                                     Animations) const
{
	const FDirectionalAnimation* DirectionAnim = Animations.Find(CurrentGait);
	if (!DirectionAnim)
	{
		return nullptr;
	}
	UAnimSequenceBase* SelectedAnim = nullptr;
	switch (CurrentLocomotionDirection)
	{
	case ELocomotionDirection::Forward:
		SelectedAnim = DirectionAnim->ForwardAnim;
		break;
	case ELocomotionDirection::Backward:
		SelectedAnim = DirectionAnim->BackwardAnim;
		break;
	case ELocomotionDirection::Left:
		SelectedAnim = DirectionAnim->LeftAnim;
		break;
	case ELocomotionDirection::Right:
		SelectedAnim = DirectionAnim->RightAnim;
		break;
	}
	check(!!SelectedAnim);
	return SelectedAnim;
}
