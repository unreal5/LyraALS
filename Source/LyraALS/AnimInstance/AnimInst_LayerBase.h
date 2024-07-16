// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Struct/DirectionalAnimationSet.h"
#include "AnimInst_LayerBase.generated.h"

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
};
