// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimInstance_Layer.generated.h"

struct FAnimNodeReference;
struct FAnimUpdateContext;
/**
 * 
 */
UCLASS()
class LYRAALS_API UAnimInstance_Layer : public UAnimInstance
{
	GENERATED_BODY()
protected:
	/* Common category */
	UFUNCTION(BlueprintCallable, Category="Common", meta=(BlueprintThreadSafe, BlueprintPure))
	class UAnimInstance_Main* GetABPBase() const;
	
	/* Idle category */
	UPROPERTY(EditDefaultsOnly, Category = "Idle")
	UAnimSequenceBase* IdleAnim;
	
	UFUNCTION(BlueprintCallable, Category = "Idle", meta = (BlueprintThreadSafe))
	void IdleOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	/* Cycle category */
	UPROPERTY(EditDefaultsOnly, Category = "Cycle")
	UAnimSequenceBase* CycleWalkingAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Cycle")
	UAnimSequenceBase* CycleJoggingAnim;

	//UFUNCTION(BlueprintCallable, Category = "Cycle", meta = (BlueprintThreadSafe))
	//void CycleOnBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	
	UFUNCTION(BlueprintCallable, Category = "Cycle", meta = (BlueprintThreadSafe))
	void CycleOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
};
