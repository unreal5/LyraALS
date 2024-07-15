// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
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
	TObjectPtr<UAnimSequenceBase> CycleWalkAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cycle")
	TObjectPtr<UAnimSequenceBase> CycleJoggingAnim;
	
	UFUNCTION(Category="Cycle", BlueprintCallable, meta=(BlueprintThreadSafe))
	void Cycle_OnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
};
