// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enums/Gait.h"
#include "Enums/LocomotionDirection.h"
#include "Enums/RootYawOffsetMode.h"
#include "Kismet/KismetMathLibrary.h"
#include "AnimInstance_Main.generated.h"

struct FAnimNodeReference;
struct FAnimUpdateContext;
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

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Data | Locomotion")
	ELocomotionDirection LastFrameVelocityLocomotionDirection = ELocomotionDirection::Forward;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Velocity")
	FVector CharacterVelocity2D;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Rotation")
	float ActorYaw;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Rotation")
	float LastFrameActorYaw;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Rotation")
	float DeltaActorYaw;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Rotation")
	float LeanAngle;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Location")
	float DeltaLocation = 0.f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Locomotion")
	float AccelerationLocomotionAngle = 0.f;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Locomotion")
	ELocomotionDirection AccelerationLocomotionDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="RootYawOffset")
	float RootYawOffset = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="RootYawOffset")
	ERootYawOffsetMode RootYawOffsetMode = ERootYawOffsetMode::Accumulate;
	UFUNCTION(BlueprintCallable, Category="RootYawOffset", meta=(BlueprintThreadSafe))
	void UpdateRootYawOffset(float DeltaTime);
	UFUNCTION(BlueprintCallable, Category="RootYawOffset", meta=(BlueprintThreadSafe))
	void SetRootYawOffset(float Angle);
	FFloatSpringState RootYawOffsetSpringState;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Acceleration")
	FVector PivotAcceleration2D = FVector::ZeroVector;
	
	UFUNCTION(BlueprintCallable, Category="Common", meta=(BlueprintThreadSafe, BlueprintPure))
	UCharacterMovementComponent* GetCharacterMovementComponent() const;

	// Velocity Data
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Velocity")
	FVector CharacterVelocity;

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
	UPROPERTY(Transient)
	FVector LastFrameWorldLocation;
	
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

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Data | Locomotion")
	float VelocityLocomotionAngleWithOffset;
	
	UFUNCTION(BlueprintCallable, Category="Data | Locomotion", meta=(BlueprintThreadSafe))
	void UpdateOrientationData();

	//UFUNCTION(BlueprintCallable, Category="Data | Locomotion", meta=(BlueprintThreadSafe))
	ELocomotionDirection CalculateLocomotionDirection(const ELocomotionDirection& CurrentLocomotionDirection, float CurrentLocomotionAngle, float BackwardThreshold = 135.f, float ForwardThreshold = 45.f, const float DeadZone = 10.f);

	// Pivot 相关
	UFUNCTION(BlueprintCallable, Category = "Stop", meta = (BlueprintThreadSafe))
	void PivotOnBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	// Idle-turn in place
	UFUNCTION(BlueprintCallable, Category = "Idle", meta = (BlueprintThreadSafe))
	void OnUpdateIdleState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	void ProcessTurnYawCurve();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "RootYawOffset")
	float TurnYawCurveValue;
	
	float LastFrameTurnYawCurveValue;

	// 下蹲相关
	UPROPERTY(BlueprintReadWrite, Category="Crouching")
	bool bIsCrouching = false;
	UPROPERTY(BlueprintReadWrite, Category="Crouching")
	bool LastFrameIsCrouched;
	UPROPERTY(BlueprintReadWrite, Category="Crouching")
	bool CrouchStateChanged;

	// Jump
	UPROPERTY(Transient, BlueprintReadWrite, Category="Jump")
	bool bIsOnAir = false;
	UPROPERTY(Transient, BlueprintReadWrite, Category="Jump")
	bool bIsJumping = false;
	UPROPERTY(Transient, BlueprintReadWrite, Category="Jump")
	bool bIsFalling = false;
	UPROPERTY(Transient, BlueprintReadWrite, Category="Jump")
	float TimeToJumpApex = 0.f;
public:
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
};
