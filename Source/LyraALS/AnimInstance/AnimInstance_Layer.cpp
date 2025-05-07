// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/AnimInstance_Layer.h"

#include "AnimInstance_Main.h"
#include "SequenceEvaluatorLibrary.h"
#include "SequencePlayerLibrary.h"
#include "AnimCharacterMovementLibrary.h"
#include "AnimDistanceMatchingLibrary.h"
#include "AnimExecutionContextLibrary.h"
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
	auto Velocity = CharacterMovementComp->Velocity;
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
	}
	else
	{
		// 找帧，推进
		//UE_LOG(LogTemp, Warning, TEXT("AdvanceTime: %f"), StopDistance);
		UAnimDistanceMatchingLibrary::DistanceMatchToTarget(SequenceEvaluator, StopDistance, FName("Distance"));
	}
}

void UAnimInstance_Layer::StartOnBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	auto ABPBase = GetABPBase();
	if (!ABPBase) return;
	// 获取当前步态
	const EGait CurrentGait = ABPBase->CurrentGait;
	// 获取当前运动方向
	const ELocomotionDirection CurrentLocomotionDirection = ABPBase->VelocityLocomotionDirection;
	FSequenceEvaluatorReference SequenceEvaluator;
	bool Result;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, Result);
	if (!Result) return;
	auto SelectedAnim = SelectAnimByGaitAndDirection(CurrentGait, CurrentLocomotionDirection, StartAnimations);
	if (!SelectedAnim)
	{
		checkf(false, TEXT("检查开始动画是否设置"));
		// do nothing.
		return;
	}
	USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, SelectedAnim);
	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, 0.f);
}

void UAnimInstance_Layer::StartOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	auto ABPBase = GetABPBase();
	if (!ABPBase) return;

	FSequenceEvaluatorReference SequenceEvaluator;
	bool Result;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, Result);
	if (!Result) return;

	//USequenceEvaluatorLibrary::AdvanceTime(Context, SequenceEvaluator);
	float Distance = ABPBase->DeltaLocation;
	UAnimDistanceMatchingLibrary::AdvanceTimeByDistanceMatching(Context, SequenceEvaluator, Distance,
	                                                            FName("Distance"));
}

void UAnimInstance_Layer::PivotOnBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	auto ABPBase = GetABPBase();
	if (!ABPBase) return;

	FSequenceEvaluatorReference SequenceEvaluator;
	bool Result;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, Result);
	if (!Result) return;

	// 获取当前步态、运动方向
	const EGait CurrentGait = ABPBase->CurrentGait;
	const ELocomotionDirection AccelDirection = ABPBase->AccelerationLocomotionDirection;
	// 计算出应选择的动画
	if (auto SelectedAnim = SelectAnimByGaitAndDirection(CurrentGait, AccelDirection, PivotAnimations))
	{
		USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context, SequenceEvaluator, SelectedAnim);
		USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, 0.f);
	}
}

void UAnimInstance_Layer::PivotOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	auto ABPBase = GetABPBase();
	if (!ABPBase) return;

	FSequenceEvaluatorReference SequenceEvaluator;
	bool Result;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, Result);
	if (!Result) return;

	// 二步计算：1、位于哪个阶段？2、stop则进行距离匹配，推进动画；start阶段则推进动画

	// 通过计算加速度和速度的点积，得出目前角色位于哪个阶段（停止？前进？）
	auto Velocity2D = ABPBase->CharacterVelocity2D;
	const auto Acceleration2D = ABPBase->Acceleration2D;
	const FName CurveName = FName("Distance");

	if (FVector::DotProduct(Velocity2D, Acceleration2D) < 0.f) // 停止阶段，进行距离匹配
	{
		auto CharacterMovementComp = GetCharacterMovementComponent();
		if (!CharacterMovementComp) return;

		const EGait CurrentGait = ABPBase->CurrentGait;
		const ELocomotionDirection CurrentAccelerationLocomotionDirection = ABPBase->AccelerationLocomotionDirection;
		auto PreviousAnim = USequenceEvaluatorLibrary::GetSequence(SequenceEvaluator);
		auto SelectedAnim = SelectAnimByGaitAndDirection(CurrentGait, CurrentAccelerationLocomotionDirection,
		                                                 PivotAnimations);
		if (SelectedAnim != PreviousAnim)
		{
			// 如果不一致，需要重新设置动画，否则动作会很滑稽。
			USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context, SequenceEvaluator, SelectedAnim);
			// 在折返运动时，会经常发生这种情况，并不需要重置时间；因为随后我们会根据折返点匹配到相应的动画帧。
		}
		// 预测折返点位置
		auto Velocity = ABPBase->CharacterVelocity;
		auto GroundFriction = CharacterMovementComp->GroundFriction;
		auto StopDistance = UAnimCharacterMovementLibrary::PredictGroundMovementPivotLocation(
			Acceleration2D, Velocity, GroundFriction).Size2D();
		UAnimDistanceMatchingLibrary::DistanceMatchToTarget(SequenceEvaluator, StopDistance, CurveName);
	}
	else // 加速阶段，按距离推进
	{
		const auto DeltaLocation = ABPBase->DeltaLocation;
		UAnimDistanceMatchingLibrary::AdvanceTimeByDistanceMatching(Context, SequenceEvaluator, DeltaLocation,
		                                                            CurveName);
	}
}

void UAnimInstance_Layer::SetUpTurnInPlaceEntry(const FAnimUpdateContext& Context,
                                                const FAnimNodeReference& Node)
{
	auto ABPBase = GetABPBase();
	if (!ABPBase) return;
	bShouldTurnLeft = FMath::Sign(ABPBase->RootYawOffset) > 0.f;
}

void UAnimInstance_Layer::TurnInPlaceOnBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequenceEvaluatorReference SequenceEvaluator;
	bool Result;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, Result);
	if (!Result) return;

	FinalTurnAnimation = SelectTurnInPlaceAnimation(bShouldTurnLeft);

	USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context, SequenceEvaluator, FinalTurnAnimation);
	TurnInPlaceTime = 0.f;
	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, TurnInPlaceTime);
}

void UAnimInstance_Layer::TurnInPlaceOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequenceEvaluatorReference SequenceEvaluator;
	bool Result;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, Result);
	if (!Result) return;

	TurnInPlaceTime += UAnimExecutionContextLibrary::GetDeltaTime(Context);
	
	
	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, TurnInPlaceTime);
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

UAnimSequenceBase* UAnimInstance_Layer::SelectTurnInPlaceAnimation(const bool bTurnLeftOrRight) const
{
	auto ABPBase = GetABPBase();
	if (!ABPBase) return nullptr;
	bool GreaterThan90 = FMath::Abs(ABPBase->RootYawOffset) >= 90.f ? true : false;
	if (bTurnLeftOrRight)
	{
		return GreaterThan90 ? TurnLeftAnim180 : TurnLeftAnim90;
	}
	else
	{
		return GreaterThan90 ? TurnRightAnim180 : TurnRightAnim90;
	}
}
