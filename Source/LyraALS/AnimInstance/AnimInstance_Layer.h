// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "Enums/Gait.h"
#include "Enums/LocomotionDirection.h"

#include "Structs/DirectionalAnimation.h"

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

	UFUNCTION(BlueprintCallable, Category="Common", meta=(BlueprintThreadSafe, BlueprintPure))
	class UCharacterMovementComponent* GetCharacterMovementComponent() const;
	
	/* Idle category */
	UPROPERTY(EditDefaultsOnly, Category = "Idle")
	UAnimSequenceBase* IdleAnim;
	
	UFUNCTION(BlueprintCallable, Category = "Idle", meta = (BlueprintThreadSafe))
	void IdleOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	/* Cycle category */
	UPROPERTY(EditDefaultsOnly, Category = "Cycle")
	TMap<EGait, FDirectionalAnimation> CycleAnimations;

	//UFUNCTION(BlueprintCallable, Category = "Cycle", meta = (BlueprintThreadSafe))
	//void CycleOnBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	
	UFUNCTION(BlueprintCallable, Category = "Cycle", meta = (BlueprintThreadSafe))
	void CycleOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	/* Stop category */
	UPROPERTY(EditDefaultsOnly, Category = "Stop")
	TMap<EGait, FDirectionalAnimation> StopAnimations;
	
	UFUNCTION(BlueprintCallable, Category = "Stop", meta = (BlueprintThreadSafe))
	void StopOnBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "Stop", meta = (BlueprintThreadSafe))
	void StopOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	// Start category
	UPROPERTY(EditDefaultsOnly, Category = "Start")
	TMap<EGait, FDirectionalAnimation> StartAnimations;
	UFUNCTION(BlueprintCallable, Category = "Start", meta = (BlueprintThreadSafe))
	void StartOnBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable, Category = "Start", meta = (BlueprintThreadSafe))
	void StartOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);


	// Pivot category
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pivot")
	//FVector EnterPivotAcceleration2D;
	bool bIsStopPhase;
	
	UPROPERTY(EditDefaultsOnly, Category = "Pivot")
	TMap<EGait, FDirectionalAnimation> PivotAnimations;
	
	UFUNCTION(BlueprintCallable, Category = "Pivot", meta = (BlueprintThreadSafe))
	void PivotOnBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "Pivot", meta = (BlueprintThreadSafe))
	void PivotOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
private:
	UAnimSequenceBase* SelectAnimByGaitAndDirection(
		const EGait& CurrentGait, const ELocomotionDirection& CurrentLocomotionDirection, const TMap<EGait, FDirectionalAnimation>& Animations) const;
};
