// Lyra高级运动系统


#include "AnimLayer/AnimInst_BaseLayer.h"

#include "AnimCharacterMovementLibrary.h"
#include "AnimDistanceMatchingLibrary.h"
#include "AnimExecutionContextLibrary.h"
#include "SequenceEvaluatorLibrary.h"
#include "SequencePlayerLibrary.h"
#include "Animation/LyraAnimInstance.h"
#include "Character/LyraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimNodeBase.h"

#include "AnimNodes/AnimNode_SequenceEvaluator.h"

namespace
{
	UAnimSequenceBase* SelectAnimBy(const TArray<TObjectPtr<UAnimSequenceBase>>& InWalkAnims,
	                                const TArray<TObjectPtr<UAnimSequenceBase>>& InJoggingAnims,
	                                const TArray<TObjectPtr<UAnimSequenceBase>>& InCrouchingAnims,
	                                EGait Gait,
	                                ELocomotionDirection Direction)
	{
		const TArray<TObjectPtr<UAnimSequenceBase>>* AnimSet;
		switch (Gait)
		{
		case EGait::Walking:
			AnimSet = &InWalkAnims;
			break;
		case EGait::Jogging:
			AnimSet = &InJoggingAnims;
			break;
		case EGait::Crouching:
			AnimSet = &InCrouchingAnims;
			break;
		default:
			checkf(false, TEXT("位于不可能状态"))
			return nullptr;
		}
		check(AnimSet);
		const int32 Index = static_cast<int32>(Direction);
		if (AnimSet->IsValidIndex(Index))
		{
			return (*AnimSet)[Index];
		}
		checkf(0, TEXT("动画集合中没有对应的动画"));
		return nullptr;
	}

	UAnimSequenceBase* SelectTurnInPlaceAnim(bool bShouldTurnLeft,
	                                         float RootYawOffset,
	                                         UAnimSequenceBase* TurnInPlaceLeftAnim90,
	                                         UAnimSequenceBase* TurnInPlaceRightAnim90,
	                                         UAnimSequenceBase* TurnInPlaceLeftAnim180,
	                                         UAnimSequenceBase* TurnInPlaceRightAnim180)
	{
		const float absRootYawOffset = FMath::Abs(UKismetMathLibrary::NormalizeAxis(RootYawOffset));
		if (UKismetMathLibrary::InRange_FloatFloat(absRootYawOffset, 90.f, 180.f, true, true))
		{
			//UE_LOG(LogTemp, Warning, TEXT("选择180度转身动画"));
			return bShouldTurnLeft ? TurnInPlaceLeftAnim180 : TurnInPlaceRightAnim180;
		}
		else
		{
			return bShouldTurnLeft ? TurnInPlaceLeftAnim90 : TurnInPlaceRightAnim90;
		}
	}
}

ALyraCharacter* UAnimInst_BaseLayer::GetLyraCharacter()
{
	if (auto LyraCharacter = Cast<ALyraCharacter>(GetOwningActor()))
	{
		return LyraCharacter;
	}
	return nullptr;
}

ULyraAnimInstance* UAnimInst_BaseLayer::GetABPBase()
{
	if (auto OwningComp = GetOwningComponent())
	{
		return Cast<ULyraAnimInstance>(OwningComp->GetAnimInstance());
	}
	return nullptr;
}

void UAnimInst_BaseLayer::IdleOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequencePlayerReference SequencePlayer;
	bool Result;
	USequencePlayerLibrary::ConvertToSequencePlayerPure(Node, SequencePlayer, Result);
	if (!Result) return;

	auto LyraAnimInst = GetABPBase();
	if (!LyraAnimInst) return;
	bool bIsCrouching = LyraAnimInst->bIsCrouching;
	auto SelectedIdleAnim = bIsCrouching ? CrouchingIdleAnim : IdleAnim;
	USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, SequencePlayer, SelectedIdleAnim, 0.2f);
}

void UAnimInst_BaseLayer::CycleOnBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
}

void UAnimInst_BaseLayer::CycleOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequencePlayerReference SequencePlayer;
	bool Result;
	USequencePlayerLibrary::ConvertToSequencePlayerPure(Node, SequencePlayer, Result);
	if (!Result) return;

	if (auto LyraAnimInst = GetABPBase())
	{
		auto Gait = LyraAnimInst->CurrentGait;
		auto Direction = LyraAnimInst->LocomotionDirection;
		UAnimSequenceBase* Anim = SelectAnimBy(WalkCycleAnims, JoggingCycleAnims, CrouchCycleAnims, Gait, Direction);
		USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, SequencePlayer, Anim, 0.2f);
	}
}

void UAnimInst_BaseLayer::StopOnBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequenceEvaluatorReference SequenceEvaluator;
	bool Result;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, Result);
	if (!Result) return;

	auto LyraAnimInst = GetABPBase();
	if (!LyraAnimInst) return;

	auto Gait = LyraAnimInst->CurrentGait;
	auto Direction = LyraAnimInst->LocomotionDirection;
	UAnimSequenceBase* Anim = SelectAnimBy(WalkStopAnims, JoggingStopAnims, CrouchStopAnims, Gait, Direction);
	USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context, SequenceEvaluator, Anim, 0.2f);
	// 从头播放
	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, 0.0f);
}

void UAnimInst_BaseLayer::StopOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	auto LyraChar = GetLyraCharacter();
	if (!LyraChar) return;
	auto CharMovementComp = LyraChar->GetCharacterMovement();
	if (!CharMovementComp) return;

	auto LyraAnimInst = GetABPBase();
	if (!LyraAnimInst) return;

	FSequenceEvaluatorReference SequenceEvaluator;
	bool Result;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, Result);
	if (!Result) return;

	// 检测设置的动画
	/* 经测试，动画停滞的原因在于Deceleration的设置过滤，导致预测的停止位置太长，远远超过"Distance"曲线的距离。
	const auto AnimSequence = USequenceEvaluatorLibrary::GetSequence(SequenceEvaluator);
	UE_LOG(LogTemp, Warning, TEXT("动画序列名:%s"), *AnimSequence->GetName());
	*/
	// 预测停止位置
	FVector Velocity = LyraAnimInst->CharacterVelocity2D;
	bool bUseSeparateBrakingFriction = CharMovementComp->bUseSeparateBrakingFriction;
	float BrakingFriction = CharMovementComp->BrakingFriction;
	float GroundFriction = CharMovementComp->GroundFriction;
	float BrakingFrictionFactor = CharMovementComp->BrakingFrictionFactor;
	float BrakingDecelerationWalking = CharMovementComp->BrakingDecelerationWalking;

	FVector PredictStopLocation = UAnimCharacterMovementLibrary::PredictGroundMovementStopLocation(
		Velocity, bUseSeparateBrakingFriction, BrakingFriction, GroundFriction, BrakingFrictionFactor,
		BrakingDecelerationWalking);
	float StopDistance = PredictStopLocation.Size2D();

	// 根据StopDistance，推进时间

	// 如果没到目标位置，需要计算，否则直接播放剩余动画（推进赶时间即可）
	if (StopDistance > 0.0f)
	{
		// DistanceMatchToTarget决定播放动画的哪一帧。例如动画位移2m，目标位移1m，那么播放动画的一半
		// 问题：应该是从动画停止的位置，向前1m，这样动画才与停止后完全连续。
		// 阅读源码，分析DistanceMatchToTarget的实现原理。
		UAnimDistanceMatchingLibrary::DistanceMatchToTarget(SequenceEvaluator, StopDistance, TEXT("Distance"));
	}
	else
	{
		// SequenceEvaluator与SequencePlayer的区别在于SequenceEvaluator不会自动播放，需要手动推进时间
		USequenceEvaluatorLibrary::AdvanceTime(Context, SequenceEvaluator);
	}
	// 解释：停止动画使用RootMotion,其中后半段（停止后），根运动没有位移，只推进播放即可。
}

void UAnimInst_BaseLayer::StartOnBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequenceEvaluatorReference SequenceEvaluator;
	bool Result;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, Result);
	if (!Result) return;

	auto LyraAnimInst = GetABPBase();
	if (!LyraAnimInst) return;

	auto Gait = LyraAnimInst->CurrentGait;
	auto Direction = LyraAnimInst->LocomotionDirection;
	UAnimSequenceBase* Anim = SelectAnimBy(WalkStartAnims, JoggingStartAnims, CrouchStartAnims, Gait, Direction);
	USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context, SequenceEvaluator, Anim, 0.2f);
	// 从头播放
	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, 0.0f);
}

void UAnimInst_BaseLayer::StartOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	auto LyraAnimInst = GetABPBase();
	if (!LyraAnimInst) return;

	FSequenceEvaluatorReference SequenceEvaluator;
	bool Result;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, Result);
	if (!Result) return;

	// UE5.4已经修正设置的动画与更新的动画不一致的问题，不需要教程的补丁。
	// 启动时进行距离匹配
	//USequenceEvaluatorLibrary::AdvanceTime(Context, SequenceEvaluator);
	float StartDistance = LyraAnimInst->DeltaLocation;
	UAnimDistanceMatchingLibrary::AdvanceTimeByDistanceMatching(Context, SequenceEvaluator, StartDistance,
	                                                            ("Distance"));
}

void UAnimInst_BaseLayer::PivotOnBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequenceEvaluatorReference SequenceEvaluator;
	bool Result;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, Result);
	if (!Result) return;

	auto LyraAnimInst = GetABPBase();
	if (!LyraAnimInst) return;

	auto Gait = LyraAnimInst->CurrentGait;
	auto Direction = LyraAnimInst->AccelerationLocomotionDirection;
	UAnimSequenceBase* Anim = SelectAnimBy(WalkPivotAnims, JoggingPivotAnims, CrouchPivotAnims, Gait, Direction);
	USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context, SequenceEvaluator, Anim, 0.2f);
	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, 0.0f);

	auto AnimName = Anim->GetName();
	UE_LOG(LogTemp, Warning, TEXT("PivotOnBecomeRelevant: %s"), *AnimName);
}

void UAnimInst_BaseLayer::PivotOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequenceEvaluatorReference SequenceEvaluator;
	bool Result;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, Result);
	if (!Result) return;

	auto LyraAnimInst = GetABPBase();
	if (!LyraAnimInst) return;
	auto LyraChar = GetLyraCharacter();
	if (!LyraChar) return;
	auto CharMovementComp = LyraChar->GetCharacterMovement();
	if (!CharMovementComp) return;

	// 按照Lyra做法，必须进行动画检查是否与进入状态时选择的动画相同，否则应重新设置动画
	auto Gait = LyraAnimInst->CurrentGait;
	auto Direction = LyraAnimInst->AccelerationLocomotionDirection;
	UAnimSequenceBase* Anim = SelectAnimBy(WalkPivotAnims, JoggingPivotAnims, CrouchPivotAnims, Gait, Direction);
	if (Anim != USequenceEvaluatorLibrary::GetSequence(SequenceEvaluator))
	{
		USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context, SequenceEvaluator, Anim, 0.2f);
		USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, 0.0f);
		//return;
	}

	// 检查结束
	// Pivot 转身，其实分两个阶段，一个是停止，另一个是启动。
	// 可以通过加速度和速度的夹角来判断位于那个阶段。
	auto Velocity = LyraAnimInst->CharacterVelocity2D.GetSafeNormal();
	auto Acceleration = LyraAnimInst->Acceleration2D.GetSafeNormal();
	// 判断是否是停止阶段
	const bool bIsStopPhase = FVector::DotProduct(Velocity, Acceleration) < 0.0f;
	if (bIsStopPhase) // 停止阶段要使用与预测的停止目标匹配
	{
		// 根据当前速度、加速度……计算折返点位置
		const float GroundFriction = CharMovementComp->GroundFriction;
		FVector PivotLocation = UAnimCharacterMovementLibrary::PredictGroundMovementPivotLocation(
			LyraAnimInst->Acceleration, LyraAnimInst->CharacterVelocity, GroundFriction);
		float PivotDistance = PivotLocation.Length();
		UAnimDistanceMatchingLibrary::DistanceMatchToTarget(SequenceEvaluator, PivotDistance, TEXT("Distance"));
	}
	else // 启动阶段要从动画开始匹配
	{
		UAnimDistanceMatchingLibrary::AdvanceTimeByDistanceMatching(Context, SequenceEvaluator,
		                                                            LyraAnimInst->DeltaLocation,
		                                                            ("Distance"));
	}
}

void UAnimInst_BaseLayer::SetupTurnInPlaceEntryState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	auto LyraAnimInst = GetABPBase();
	if (!LyraAnimInst) return;
	// 根据状态切换条件：进入Turn In Place Entry State时，RootYawOffset绝对值必定>=50.f
	bShouldTurnLeft = LyraAnimInst->RootYawOffset > 0.0f;
}

void UAnimInst_BaseLayer::TurnInPlaceBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	auto LyraAnimInst = GetABPBase();
	if (!LyraAnimInst) return;
	float RootYawOffset = LyraAnimInst->RootYawOffset;
	// 选择动画
	FSequenceEvaluatorReference SequenceEvaluator;
	bool Result;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, Result);
	if (!Result) return;


	FinalTurnInPlaceAnim = SelectTurnInPlaceAnim(bShouldTurnLeft, RootYawOffset, TurnInPlaceLeftAnim90,
	                                             TurnInPlaceRightAnim90, TurnInPlaceLeftAnim180,
	                                             TurnInPlaceRightAnim180);
	USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context, SequenceEvaluator, FinalTurnInPlaceAnim, 0.2f);
	TurnInPlaceTime = 0.0f;
	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, TurnInPlaceTime);
}

void UAnimInst_BaseLayer::TurnInPlaceOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequenceEvaluatorReference SequenceEvaluator;
	bool Result;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, Result);
	if (!Result) return;

	//要注意FAnimUpdateContext 和  FAnimationUpdateContext的区别
	const float DeltaTime = UAnimExecutionContextLibrary::GetDeltaTime(Context);
	TurnInPlaceTime += DeltaTime;
	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, TurnInPlaceTime);
}
