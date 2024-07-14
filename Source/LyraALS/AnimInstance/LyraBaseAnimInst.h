// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enum/Gun.h"
#include "LyraBaseAnimInst.generated.h"

struct FAnimUpdateContext;
/**
 * 
 */
UCLASS()
class LYRAALS_API ULyraBaseAnimInst : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	class UCharacterMovementComponent* GetCharacterMovementComponent() const;

	UPROPERTY(BlueprintReadWrite, Category = "Character")
	EGun EquippedGun;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idle")
	TObjectPtr<UAnimSequenceBase> IdleSequence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idle")
	TObjectPtr<UAnimSequenceBase> PistolIdleSequence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idle")
	TObjectPtr<UAnimSequenceBase> RifleIdleSequence;
	
	UFUNCTION(Category="Idle", BlueprintCallable, meta=(BlueprintThreadSafe))
	void IdleOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
};
