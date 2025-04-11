// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "LyraAnimInstance.generated.h"

class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class LYRAALS_API ULyraAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
protected:
	UFUNCTION(BlueprintCallable, Category="Common", meta=(BlueprintThreadSafe, BlueprintPure))
	UCharacterMovementComponent* GetLyraCharacterMovementComponent();
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="VelocityData")
	FVector CharacterVelocity;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="VelocityData")
	FVector CharacterVelocity2D;
	
	UFUNCTION(BlueprintCallable, Category="VelocityData", meta=(BlueprintThreadSafe))
	void GetVelocityData();
};
