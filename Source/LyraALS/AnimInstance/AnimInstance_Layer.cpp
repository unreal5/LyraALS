// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/AnimInstance_Layer.h"

#include "AnimInstance_Main.h"
#include "SequenceEvaluatorLibrary.h"
#include "SequencePlayerLibrary.h"
#include "AnimCharacterMovementLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

UAnimInstance_Main* UAnimInstance_Layer::GetABPBase() const
{
	return Cast<UAnimInstance_Main>(GetSkelMeshComponent()->GetAnimInstance());
}

UCharacterMovementComponent* UAnimInstance_Layer::GetCharacterMovementComponent() const
{
	auto ABPBase = GetABPBase();
	if (!ABPBase) return nullptr;

	return ABPBase->GetCharacterMovementComponent();
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



	// 当前使用分层动画，因此返回的是分层动画的实例，例如：UnArmed_Layer，……。
	// 仅供学习测试
	/*
	auto&& Proxy = GetProxyOnAnyThread<FAnimInstanceProxy>();
	auto LayerAnimInst = Cast<UAnimInstance_Layer>(Proxy.GetAnimInstanceObject());
	if (!LayerAnimInst) return;
	*/
	auto ABPBase = GetABPBase();
	if (!ABPBase) return;
	
	FSequenceEvaluatorReference SequenceEvaluator;
	bool Result;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, Result);
	if (!Result) return;
	auto SelectedAnim = SelectAnimByGaitAndDirection(ABPBase->CurrentGait, ABPBase->VelocityLocomotionDirection,
	                                                 StopAnimations);
	if (!SelectedAnim)
	{
		checkf(false, TEXT("检查停止动画是否设置"));
		// do nothing.
		return;
	}

	USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context, SequenceEvaluator, SelectedAnim);
	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, 0.f);
}

void UAnimInstance_Layer::StopOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	auto CharacterMovementComp = GetCharacterMovementComponent();
	if (!CharacterMovementComp)
	{
		UE_LOG(LogTemp, Error, TEXT("UAnimInstance_Layer::StopOnUpdate: CharacterMovementComp is nullptr"));
		return;
	}
	
	FSequenceEvaluatorReference SequenceEvaluator;
	bool Result;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, Result);
	if (!Result) return;

	// 预测停止位置
	auto Velocity= CharacterMovementComp->Velocity;
	bool bUseSeparateBrakingFriction = CharacterMovementComp->bUseSeparateBrakingFriction;
	auto BrakingFriction = CharacterMovementComp->BrakingFriction;
	auto GroundFriction = CharacterMovementComp->GroundFriction;
	auto BrakingFrictionFactor = CharacterMovementComp->BrakingFrictionFactor;
	auto BrakingDecelerationWalking = CharacterMovementComp->BrakingDecelerationWalking;
	// 预测停止距离(距0帧动画多远）
	auto StopDistance = UAnimCharacterMovementLibrary::PredictGroundMovementStopLocation(
		Velocity, bUseSeparateBrakingFriction, BrakingFriction, GroundFriction, BrakingFrictionFactor,
		BrakingDecelerationWalking).Size2D();
	// 知道停止的位置，然后需要在动画中找到对应的帧(从最远距离倒着找)。还有一种情况是距离为0（已经到达预测位置），则正常推进动画即可
	if (FMath::IsNearlyZero(StopDistance, 0.0001f))
	{
		USequenceEvaluatorLibrary::AdvanceTime(Context, SequenceEvaluator);
		UE_LOG(LogTemp, Warning, TEXT("AdvanceTime: %f"), StopDistance);
	}
	else
	{
		// 找帧，推进
		UE_LOG(LogTemp, Warning, TEXT("找帧->推进时间线"));
	}
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
