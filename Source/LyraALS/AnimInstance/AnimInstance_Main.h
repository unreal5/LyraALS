// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enums/Gait.h"
#include "Enums/LocomotionDirection.h"
#include "AnimInstance_Main.generated.h"

class UCharacterMovementComponent;
/**
 * 
 */
UCLASS(DisplayName = "主动画实例", Blueprintable, BlueprintType)
class LYRAALS_API UAnimInstance_Main : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Gait", meta=(DisplayName="当前步态"))
	EGait CurrentGait = EGait::Walking;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Locomotion")
	ELocomotionDirection VelocityLocomotionDirection = ELocomotionDirection::Forward;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Rotation")
	float ActorYaw;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Rotation")
	float LastFrameActorYaw;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Rotation")
	float DeltaActorYaw;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Rotation")
	float LeanAngle;


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

	// Acceleration Data
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Acceleration")
	FVector Acceleration;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Acceleration")
	FVector Acceleration2D;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Acceleration")
	bool IsAcceleration;
	UFUNCTION(BlueprintCallable, Category="Data | Acceleration", meta=(BlueprintThreadSafe))
	void GetAccelerationData();
	
	// Location Data
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Location")
	FVector WorldLocation;
	
	UFUNCTION(BlueprintCallable, Category="Data | Location", meta=(BlueprintThreadSafe))
	void GetLocationData();

	// Rotation Data
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Rotation")
	FRotator WorldRotation;

	UFUNCTION(BlueprintCallable, Category="Data | Rotation", meta=(BlueprintThreadSafe))
	void GetRotationData(float DeltaTime);

	// Orientation Data
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Locomotion")
	float VelocityLocomotionangle;
	
	UFUNCTION(BlueprintCallable, Category="Data | Locomotion", meta=(BlueprintThreadSafe))
	void UpdateOrientationData();

	//UFUNCTION(BlueprintCallable, Category="Data | Locomotion", meta=(BlueprintThreadSafe))
	ELocomotionDirection CalculateLocomotionDirection(const ELocomotionDirection& CurrentLocomotionDirection, float CurrentLocomotionAngle, float BackwardThreshold = 135.f, float ForwardThreshold = 45.f, const float DeadZone = 10.f);
};
