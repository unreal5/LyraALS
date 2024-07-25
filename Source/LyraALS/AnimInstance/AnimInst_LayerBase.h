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

	UFUNCTION(Category="Cycle", BlueprintCallable, meta=(BlueprintThreadSafe))
	void Cycle_OnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	// Stop
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stop")
	FDirectionalAnimationSet WalkStopAnimationSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stop")
	FDirectionalAnimationSet JogStopAnimationSet;

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

	UFUNCTION(Category="Pivot", BlueprintCallable, meta=(BlueprintThreadSafe))
	void Pivot_BecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(Category="Pivot", BlueprintCallable, meta=(BlueprintThreadSafe))
	void Pivot_OnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	// 原地转身
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "原地转身")
	UAnimSequenceBase* TurnLeft90;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "原地转身")
	UAnimSequenceBase* TurnLeft180;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "原地转身")
	UAnimSequenceBase* TurnRight90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "原地转身")
	UAnimSequenceBase* TurnRight180;

	bool bIsGreaterThan90;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "原地转身")
	bool ShouldTurnLeft;
	
	UFUNCTION(Category="原地转身", BlueprintCallable, meta=(BlueprintThreadSafe))
	void TurnInPlace_Output_BecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(Category="原地转身", BlueprintCallable, meta=(BlueprintThreadSafe))
	void TurnInPlace_BecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(Category="原地转身", BlueprintCallable, meta=(BlueprintThreadSafe))
	void TurnInPlace_OnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
private:
	UAnimSequenceBase* SelectAnimSequeceFromAnimSets(const FDirectionalAnimationSet& WalkAnimSet,
	                                     const FDirectionalAnimationSet& JogAnimSet,
	                                     EGait CurrentGait, ELocomotionDirection CurrentDirection) const;

	UAnimSequenceBase* SelectPivotAnim();

	// turn in place
	UAnimSequenceBase* SelectTurnInPlaceAnimation() const;
	float TurnInPlaceTime;
};
