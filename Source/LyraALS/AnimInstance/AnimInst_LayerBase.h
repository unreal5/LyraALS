// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enum/Gait.h"
#include "Enum/LocomotionDirection.h"
#include "Struct/DirectionalAnimationSet.h"
#include "AnimInst_LayerBase.generated.h"

struct FAnimUpdateContext;
struct FAnimNodeReference;

/**
 * 
 */
UCLASS()
class LYRAALS_API UAnimInst_LayerBase : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(Category="主实例", BlueprintPure, meta=(BlueprintThreadSafe))
	class ULyraBaseAnimInst* GetABPBase() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idle")
	TObjectPtr<UAnimSequenceBase> IdleAnimation;

	
	
	UFUNCTION(Category="Idle", BlueprintCallable, meta=(BlueprintThreadSafe))
	void Idle_UpdateAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	// Start
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Start")
	FDirectionalAnimationSet WalkStartAnimationSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Start")
	FDirectionalAnimationSet JogStartAnimationSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Start")
	FDirectionalAnimationSet CrouchStartAnimationSet;
	// start function
	UFUNCTION(Category="Start", BlueprintCallable, meta=(BlueprintThreadSafe))
	void Start_BecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(Category="Start", BlueprintCallable, meta=(BlueprintThreadSafe))
	void Start_OnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	
	// Cycle
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cycle")
	FDirectionalAnimationSet WalkCycleAnimationSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cycle")
	FDirectionalAnimationSet JogCycleAnimationSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cycle")
	FDirectionalAnimationSet CrouchCycleAnimationSet;
	
	UFUNCTION(Category="Cycle", BlueprintCallable, meta=(BlueprintThreadSafe))
	void Cycle_OnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	// Stop
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stop")
	FDirectionalAnimationSet WalkStopAnimationSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stop")
	FDirectionalAnimationSet JogStopAnimationSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stop")
	FDirectionalAnimationSet CrouchStopAnimationSet;
	
	UFUNCTION(Category="Stop", BlueprintCallable, meta=(BlueprintThreadSafe))
	void Stop_BecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(Category="Stop", BlueprintCallable, meta=(BlueprintThreadSafe))
	void Stop_OnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	// Pivot
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pivot")
	FVector InitialAccWhenEnterPivot;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pivot")
	FDirectionalAnimationSet WalkPivotAnimationSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pivot")
	FDirectionalAnimationSet JogPivotAnimationSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pivot")
	FDirectionalAnimationSet CrouchPivotAnimationSet;
	
	UFUNCTION(Category="Pivot", BlueprintCallable, meta=(BlueprintThreadSafe))
	void Pivot_BecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(Category="Pivot", BlueprintCallable, meta=(BlueprintThreadSafe))
	void Pivot_OnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	// 原地转身-站立
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "原地转身")
	UAnimSequenceBase* TurnLeft90;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "原地转身")
	UAnimSequenceBase* TurnLeft180;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "原地转身")
	UAnimSequenceBase* TurnRight90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "原地转身")
	UAnimSequenceBase* TurnRight180;

	// 原地转身-蹲下
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "原地转身")
	UAnimSequenceBase* CrouchTurnLeft90;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "原地转身")
	UAnimSequenceBase* CrouchTurnLeft180;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "原地转身")
	UAnimSequenceBase* CrouchTurnRight90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "原地转身")
	UAnimSequenceBase* CrouchTurnRight180;
	
	bool bIsGreaterThan90;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "原地转身")
	bool ShouldTurnLeft;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "原地转身")
	float TurnInPlaceTime;
	
	UPROPERTY()
	UAnimSequenceBase* FinalTurnAnimation;
	
	UFUNCTION(Category="原地转身", BlueprintCallable, meta=(BlueprintThreadSafe))
	void TurnInPlace_Output_BecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(Category="原地转身", BlueprintCallable, meta=(BlueprintThreadSafe))
	void TurnInPlace_BecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(Category="原地转身", BlueprintCallable, meta=(BlueprintThreadSafe))
	void TurnInPlace_OnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(Category="原地转身", BlueprintPure, meta=(BlueprintThreadSafe))
	UAnimSequenceBase* SelectTurnInPlaceAnimation() const;

	// 蹲下-起立
	UFUNCTION(Category=" 蹲下-起立", BlueprintCallable, meta=(BlueprintThreadSafe))
	void SetupStanceTransitionAnim(const FAnimUpdateContext& UpdateContext, const FAnimNodeReference& Node) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idle")
	TObjectPtr<UAnimSequenceBase> CrouchIdleAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idle")
	TObjectPtr<UAnimSequenceBase> CrouchEntryAnim;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idle")
	TObjectPtr<UAnimSequenceBase> CrouchExitAnim;

	// 跳
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump")
	TObjectPtr<UAnimSequenceBase> JumpStartAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump")
	TObjectPtr<UAnimSequenceBase> JumpLoopAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump")
	TObjectPtr<UAnimSequenceBase> JumpApexAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump")
	TObjectPtr<UAnimSequenceBase> JumpFallAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump")
	TObjectPtr<UAnimSequenceBase> JumpLandAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump")
	TObjectPtr<UAnimSequenceBase> JumpRecoveryAnim;
private:
	UAnimSequenceBase* SelectAnimSequeceFromAnimSets(const FDirectionalAnimationSet& WalkAnimSet,
	                                     const FDirectionalAnimationSet& JogAnimSet,
	                                     const FDirectionalAnimationSet& CrouchAnimSet,
	                                     EGait CurrentGait, ELocomotionDirection CurrentDirection) const;

	UAnimSequenceBase* SelectPivotAnim();

	// turn in place
	
};
