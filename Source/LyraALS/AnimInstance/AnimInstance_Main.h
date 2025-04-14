// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enums/Gait.h"
#include "AnimInstance_Main.generated.h"

class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class LYRAALS_API UAnimInstance_Main : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Gait", meta=(DisplayName="当前步态"))
	EGait CurrentGait = EGait::Walking;
protected:
	UFUNCTION(BlueprintCallable, Category="Common", meta=(BlueprintThreadSafe, BlueprintPure))
	UCharacterMovementComponent* GetCharacterMovementComponent() const;

	// Velocity Data
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Velocity")
	FVector CharacterVelocity;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Velocity")
	FVector CharacterVelocity2D;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Velocity")
	float CharacterSpeed;
	
	UFUNCTION(BlueprintCallable, Category="Data | Velocity", meta=(BlueprintThreadSafe))
	void GetVelocityData();

	// Location Data
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Location")
	FVector WorldLocation;
	
	UFUNCTION(BlueprintCallable, Category="Data | Location", meta=(BlueprintThreadSafe))
	void GetLocationData();

};
