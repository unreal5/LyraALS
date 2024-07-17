// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enum/Gait.h"
#include "Enum/Gun.h"
#include "Enum/LocomotionDirection.h"
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

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	class UCharacterMovementComponent* GetCharacterMovementComponent() const;

	UPROPERTY(BlueprintReadWrite, Category = "WeaponState")
	EGun EquippedGun;

	UPROPERTY(BlueprintReadWrite, Category = "GaitState")
	EGait CurrentGait;

	UPROPERTY(BlueprintReadWrite, Category = "GaitState")
	EGait LastFrameGait;
	
	UPROPERTY(BlueprintReadWrite, Category = "GaitState")
	EGait InComingGait;

	UPROPERTY(BlueprintReadWrite, Category = "GaitState")
	bool bIsGaitChanged;

	UPROPERTY(BlueprintReadWrite, Category = "LocationData")
	FVector WorldLocation;

	UPROPERTY(BlueprintReadWrite, Category = "VelocityData")
	FVector CharacterVelocity;

	UPROPERTY(BlueprintReadWrite, Category = "VelocityData")
	FVector CharacterVelocity2D;
	UPROPERTY(BlueprintReadWrite, Category = "VelocityData")
	float CharacterSpeed2D;

	UPROPERTY(BlueprintReadWrite, Category = "RotationData")
	FRotator WorldRotation;

	// Delta Yaw between last frame and current frame per second
	UPROPERTY(BlueprintReadWrite, Category = "RotationData")
	float LeanAngle;

	UPROPERTY(BlueprintReadWrite, Category = "LocomotionData")
	float VelocityLocomotionAngle;

	UPROPERTY(BlueprintReadWrite, Category = "LocomotionData")
	ELocomotionDirection VelocityLocomotionDirection;

	UPROPERTY(BlueprintReadWrite, Category = "LocomotionData")
	ELocomotionDirection LastFrameLocomotionDirection;

	UPROPERTY(BlueprintReadOnly, Category = "AccelerationData")
	FVector Acceleration;

	UPROPERTY(BlueprintReadOnly, Category = "AccelerationData")
	FVector Acceleration2D;

	UPROPERTY(BlueprintReadOnly, Category = "AccelerationData")
	bool bIsAccelerating;

private:
	void GetVelocityData();
	void GetAccelerationData();
	void GetLocationData();
	void GetRotationData(float DeltaSeconds);


	void UpdateOrientation(float DeltaTime);
	ELocomotionDirection CalculateLocomotionDirection(float CurrentLocomotionAngle,
	                                                  ELocomotionDirection CurrentLocomotionDirection,
	                                                  float ForwardMin = -50.f, float ForwardMax = 50.f,
	                                                  float BackwardMin = -130.f, float BackwardMax = 130.f,
	                                                  float DeadZone = 20.f);

	void GetCharacterStates();
};
