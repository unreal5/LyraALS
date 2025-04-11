// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimInstanceBaseLayer.generated.h"

struct FAnimNodeReference;
struct FAnimUpdateContext;
/**
 * 
 */
UCLASS()
class LYRAALS_API UAnimInstanceBaseLayer : public UAnimInstance
{
	GENERATED_BODY()
protected:
	/* Idle category */
	UPROPERTY(EditDefaultsOnly, Category = "Idle")
	UAnimSequenceBase* IdleAnim;
	
	UFUNCTION(BlueprintCallable, Category = "Idle", meta = (BlueprintThreadSafe))
	void IdleOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
};
