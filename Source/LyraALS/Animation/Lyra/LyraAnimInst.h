// 版权没有，拷贝自由。

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimInstanceProxy.h"
#include "Enumeration/EnumTypes.h"
#include "Interface/CombatInterface.h"
#include "Struct/StructTypes.h"
#include "LyraAnimInst.generated.h"

/**
 * 动画代理
 */

enum class EGunType : uint8;

USTRUCT()
struct FLyraAnimInstProxy final : public FAnimInstanceProxy
{
	GENERATED_BODY()
	FLyraAnimInstProxy() = default;
	explicit FLyraAnimInstProxy(UAnimInstance* InAnimInstance);

protected:
	virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;
	virtual void Update(float DeltaSeconds) override;

	// Velocity
	FVector CharacterVelocity = FVector::ZeroVector;
	FVector CharacterVelocity2D = FVector::ZeroVector;
	float GroundSpeed = 0.f;
	bool HasVelocity = false;

	// Acceleration
	FVector CurrentAcceleration{FVector::ZeroVector};
	FVector CurrentAcceleration2D{FVector::ZeroVector};
	bool bHasAcceleration{false};

	// Location
	FVector WorldLocation = FVector::ZeroVector;

	// Rotation
	float LeanAngle = 0.f;
	float LastFrameActorYaw = 0.f;
	FRotator WorldRotation = FRotator::ZeroRotator;

	// Orientation
	float VelocityLocomotionAngle = 0.f;
	ELocomotionDirection VelocityLocomotionDirection{ELocomotionDirection::Forward};

private:
	void GetVelocityData();
	void GetAccelerationData();
	void GetLocationData();
	void GetRotationData(float DeltaTime);
	void UpdateOrientationData();
	ELocomotionDirection CalculateLocomotionDirection(float InAngle, ELocomotionDirection CurrentDirection,
	                                                  float DeadZone = 20.f, float BackwardMin = -130.f,
	                                                  float BackwardMax = 130.f, float ForwardMin = -50.f,
	                                                  float ForwardMax = 50.f);

	friend class ULyraAnimInst;
};

UCLASS()
class LYRAALS_API ULyraAnimInst : public UAnimInstance, public ICombatInterface
{
	GENERATED_BODY()

public:
	virtual void ReceiveEquipWeapon_Implementation(EGunType NewGunType) override;
	virtual void ReceiveCurrentGait_Implementation(EGaitType NewGait, const FPredictGroundMovementStopLocationParams& GaitSettings) override;

	virtual void NativePostEvaluateAnimation() override;

protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category="武器")
	EGunType EquippedGun{EGunType::UnArmed};

	UPROPERTY(Transient, BlueprintReadOnly, Category="步态")
	EGaitType CurrentGait{EGaitType::Walking};

	UPROPERTY(Transient, BlueprintReadOnly, Category="步态")
	FPredictGroundMovementStopLocationParams CurrentGaitPredictParams;
	
	// 速度相关数据
	UPROPERTY(Transient, BlueprintReadOnly, Category="VelocityData")
	FVector CharacterVelocity;
	UPROPERTY(Transient, BlueprintReadOnly, Category="VelocityData")
	FVector CharacterVelocity2D;
	UPROPERTY(Transient, BlueprintReadOnly, Category="VelocityData")
	float GroundSpeed{0.f};
	UPROPERTY(Transient, BlueprintReadOnly, Category="VelocityData")
	bool HasVelocity{false};

	// 加速度相关数据
	UPROPERTY(Transient, BlueprintReadOnly, Category="AccelerationData")
	FVector CurrentAcceleration;
	UPROPERTY(Transient, BlueprintReadOnly, Category="AccelerationData")
	FVector CurrentAcceleration2D;
	UPROPERTY(Transient, BlueprintReadOnly, Category="AccelerationData")
	bool bIsAccelerating{false};

	// 位置相关数据
	UPROPERTY(Transient, BlueprintReadOnly, Category="LocationData")
	FVector WorldLocation;

	// 旋转相关数据
	UPROPERTY(Transient, BlueprintReadOnly, Category="RotationData")
	FRotator WorldRotation;
	UPROPERTY(Transient, BlueprintReadOnly, Category="RotationData")
	float LeanAngle{0.f};


	// 方向相关数据
	UPROPERTY(Transient, BlueprintReadOnly, Category="OrientationData")
	float VelocityLocomotionAngle{0.f};
	UPROPERTY(Transient, BlueprintReadOnly, Category="OrientationData")
	ELocomotionDirection LastFrameVelocityLocomotionDirection{ELocomotionDirection::Forward};
	UPROPERTY(Transient, BlueprintReadOnly, Category="OrientationData")
	ELocomotionDirection VelocityLocomotionDirection{ELocomotionDirection::Forward};

private:
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;
	virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy) override;
};
