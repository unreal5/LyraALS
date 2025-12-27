// 版权没有，拷贝自由。

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enumeration/EnumTypes.h"
#include "Interface/CombatInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/LyraCharacterMovementComponent.h"
#include "Struct/StructTypes.h"
#include "LyraAnimInst.generated.h"

/**
 * 动画代理
 */

class UCharacterMovementComponent;
enum class EGunType : uint8;

UCLASS()
class LYRAALS_API ULyraAnimInst : public UAnimInstance, public ICombatInterface
{
	GENERATED_BODY()

public:
	virtual void ReceiveEquipWeapon_Implementation(EGunType NewGunType) override;
	virtual void ReceiveCurrentGait_Implementation(EGaitType NewGait,
	                                               const FPredictGroundMovementStopLocationParams&
	                                               GaitSettings) override;

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category="武器")
	EGunType EquippedGun{EGunType::UnArmed};

	// 步态相关数据
	// 事件触发的进入步态
	UPROPERTY(Transient)
	EGaitType InComingGait{EGaitType::Walking};

	UPROPERTY(Transient, BlueprintReadOnly, Category="步态")
	EGaitType LastFrameGait{EGaitType::Walking};

	UPROPERTY(Transient, BlueprintReadOnly, Category="步态")
	EGaitType CurrentGait{EGaitType::Walking};

	UPROPERTY(Transient, BlueprintReadOnly, Category="步态")
	bool IsGaitChanged{false};

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
	// PivotAcceleration2D由蓝图记录进入Pivot状态时的加速度。cpp中不更新该值。
	UPROPERTY(Transient, BlueprintReadWrite, Category="AccelerationData")
	FVector PivotAcceleration2D{FVector::ZeroVector};
	UPROPERTY(Transient, BlueprintReadOnly, Category="AccelerationData")
	bool bIsAccelerating{false};
	UPROPERTY(Transient, BlueprintReadOnly, Category="AccelerationData")
	float NormalizedDotProductBetweenAccelerationAndVelocity{0.f};

	// 位置相关数据
	UPROPERTY(Transient, BlueprintReadOnly, Category="LocationData")
	FVector LastFrameWorldLocation;
	UPROPERTY(Transient, BlueprintReadOnly, Category="LocationData")
	FVector WorldLocation;
	UPROPERTY(Transient, BlueprintReadOnly, Category="LocationData", meta=(ShortTooltip="每帧位移"))
	float DisplacementPerFrame{0.f};
	// 旋转相关数据
	UPROPERTY(Transient, BlueprintReadOnly, Category="RotationData")
	FRotator WorldRotation;
	UPROPERTY(Transient, BlueprintReadOnly, Category="RotationData")
	float LeanAngle{0.f};

	float DeltaActorYaw = 0.f;
	float LastFrameActorYaw = 0.f;


	// 方向相关数据
	UPROPERTY(Transient, BlueprintReadOnly, Category="OrientationData")
	float VelocityLocomotionAngle{0.f};
	UPROPERTY(Transient, BlueprintReadOnly, Category="OrientationData")
	float VelocityLocomotionAngleWithOffset{0.f};
	UPROPERTY(Transient, BlueprintReadOnly, Category="OrientationData")
	ELocomotionDirection LastFrameVelocityLocomotionDirection{ELocomotionDirection::Forward};
	UPROPERTY(Transient, BlueprintReadOnly, Category="OrientationData")
	ELocomotionDirection VelocityLocomotionDirection{ELocomotionDirection::Forward};
	UPROPERTY(Transient, BlueprintReadOnly, Category="OrientationData")
	ELocomotionDirection AccelLocomotionDirection;
	UPROPERTY(Transient, BlueprintReadOnly, Category="OrientationData")
	float AccelLocomotionAngle;

	// RootYawOffset
	UPROPERTY(Transient, BlueprintReadWrite, Category="RootYawOffset")
	float RootYawOffset;
	UPROPERTY(Transient, BlueprintReadWrite, Category="RootYawOffset")
	ERootYawOffsetMode RootYawOffsetMode{ERootYawOffsetMode::BlendOut};

	UPROPERTY(Transient, BlueprintReadWrite, Category="Crouching")
	bool IsCrouching;
	UPROPERTY(Transient, BlueprintReadWrite, Category="Crouching")
	bool LastFrameIsCrouching;
	UPROPERTY(Transient, BlueprintReadWrite, Category="Crouching")
	bool CrouchStateChanged;

	// jump 
	UPROPERTY(Transient, BlueprintReadOnly, Category="Jumping")
	bool IsJumping = false;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Jumping")
	bool IsFalling = false;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Jumping")
	bool IsInAir = false;

	/* 每帧初始值，也是计算的依据 */
	EMovementMode CurrentFrameMovementMode = EMovementMode::MOVE_Walking;
	float GravityZ;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Jumping")
	float TimeToJumpApex = 0.f;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Jumping")
	float GroundDistance;

private:
	UPROPERTY()
	TWeakObjectPtr<ACharacter> OwningCharacter;

	UPROPERTY()
	TWeakObjectPtr<UCharacterMovementComponent> CharacterMovementComponent;

	FFloatSpringState FloatSpringState;

	void GetVelocityData();
	void GetAccelerationData();
	void GetLocationData(float DeltaTime);
	void GetRotationData(float DeltaTime);
	void UpdateOrientationData();
	void GetCharacterStates();
	void UpdateRootYawOffset(float DeltaTime);
};
