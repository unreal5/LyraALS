// Lyra高级运动系统

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimInterface/AnimationBlueprint.h"

#include "Kismet/KismetMathLibrary.h"

#include "LyraAnimInstance.generated.h"

struct FAnimNodeReference;
struct FAnimUpdateContext;
class UCharacterMovementComponent;
class ALyraCharacter;
/**
 * 
 */
UCLASS()
class LYRAALS_API ULyraAnimInstance : public UAnimInstance, public IAnimationBlueprint
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
#pragma region 调试
	UFUNCTION(BlueprintImplementableEvent)
	void DrawDebugMessage(float DeltaSeconds);
#pragma endregion
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	UCharacterMovementComponent* GetCharacterMovement();

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	ALyraCharacter* GetLyraCharacter();

#pragma region IAnimationBlueprint
	virtual void ReceiveEquippedGun_Implementation(EGuns NewEquippedGun) override;
	virtual void ReceiveCurrentGait_Implementation(EGait NewGait) override;
#pragma endregion

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "基础|属性")
	ALyraCharacter* LyraCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "基础|属性")
	UCharacterMovementComponent* LyraCharacterMovement;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "基础|状态")
	EGuns EquippedGun = EGuns::Unarmed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "基础|状态")
	EGait CurrentGait = EGait::Walking;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "基础|状态")
	EGait LastFrameGait = EGait::Walking;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "基础|状态")
	bool bIsGaitChanged = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "基础|状态")
	bool bIsCrouching = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "基础|状态")
	bool LastFrameIsCrouching = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "基础|状态")
	bool CrouchStateChanged = false;
	// 不向蓝图曝露
	EGait IncomingGait = EGait::Walking;

	UFUNCTION(BlueprintCallable, Category = "状态", meta=(BlueprintThreadSafe))
	void GetCharacterStates();

	// 速度
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "速度")
	FVector CharacterVelocity = FVector::ZeroVector;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "速度")
	FVector CharacterVelocity2D = FVector::ZeroVector;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "速度")
	float CharacterSpeed2D = 0.0f;
	UFUNCTION(BlueprintCallable, Category = "速度", meta=(BlueprintThreadSafe))
	void GetVelocityData();

	UFUNCTION(BlueprintCallable, Category = "加速度", meta=(BlueprintThreadSafe))
	void GetAccelerationData(float DeltaTime);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "加速度")
	FVector Acceleration = FVector::ZeroVector;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "加速度")
	FVector Acceleration2D = FVector::ZeroVector;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "加速度")
	bool IsAccelerating = false;

	UFUNCTION(BlueprintCallable, Category = "基类|位置", meta=(BlueprintThreadSafe))
	void GetLocationData();
	UFUNCTION(BlueprintCallable, Category = "基类|位置", meta=(BlueprintThreadSafe, ToolTip="必须在获取角色旋转角度后调用"))
	void UpdateOrientationData();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "基础|位置",
		meta=(ToolTip="角色运动方向（速度）与角色前方的夹角", ShortTooltip="角色运动方向与角色前方的夹角"))
	float VelocityLocomotionAngle = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "基础|位置",
		meta=(ToolTip="角色运动方向（速度）与角色前方的夹角", ShortTooltip="（带有补偿的）角色运动方向与角色前方的夹角"))
	float VelocityLocomotionAngleWithOffset = 0.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "基础|位置",
		meta=(ToolTip="角色加速度与角色前方的夹角", ShortTooltip="加速度与角色前方的夹角"))
	float AccelerationLocomotionAngle = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "基础|位置")
	FVector WorldLocation = FVector::ZeroVector;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "基础|位置")
	float DeltaLocation = 0.0f;

	UFUNCTION(BlueprintCallable, Category = "基类|旋转", meta=(BlueprintThreadSafe))
	void GetRotationData(float DeltaTime);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "基础|旋转")
	FRotator WorldRotation = FRotator::ZeroRotator;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "基础|旋转")
	float DeltaActorYaw = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "基础|旋转")
	float LeanAngle = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "基础|位置")
	ELocomotionDirection LocomotionDirection = ELocomotionDirection::Forward;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "基础|位置")
	ELocomotionDirection LastFrameLocomotionDirection = ELocomotionDirection::Forward;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "基础|位置")
	ELocomotionDirection AccelerationLocomotionDirection = ELocomotionDirection::Forward;


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "基础|RootYawOffset")
	float RootYawOffset = 0.0f;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "基础|RootYawOffset")
	ERootYawOffsetMode RootYawOffsetMode = ERootYawOffsetMode::Hold;

	UFUNCTION(BlueprintCallable, Category = "基类|RootYawOffset", meta=(BlueprintThreadSafe, ToolTip="计算角色运动方向"))
	void UpdateRootYawOffset(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "基类|位置", meta=(BlueprintThreadSafe, ToolTip="计算角色运动方向"))
	ELocomotionDirection CalculateLocomotionDirection(float CurrentLocomotionAngle,
	                                                  ELocomotionDirection CurrentDirection, float DeadZone = 10.f,
	                                                  float ForwardThreshold = 50.f, float BackwardThreshold = 130.f);

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "基础|Pivot")
	FVector NormalizedPivotAcceleration2D = FVector::ZeroVector;

private:
	float ActorYaw = 0.0f;

	/** Pivot 绑定函数 */
	// Pivot 需要记录进入该状态时加速度，并且动画结点计算顺序是从右至左，因此这个结点最先执行。
	UFUNCTION(BlueprintCallable, Category = "Pivot|绑定函数", meta = (BlueprintThreadSafe, AllowPrivateAccess = true))
	void PivotOutputNode_OnBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "Pivot|绑定函数", meta = (BlueprintThreadSafe, AllowPrivateAccess = true))
	void IdleOutputNode_OnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	
	void ProcessTurnYawCurve();
	/* 转身时的Yaw曲线值 */
	float TurnYawCurveValue = 0.0f;
	/* 上一帧的Yaw曲线值 */
	float LastFrameTurnYawCurveValue = 0.0f;
	
	void SetRootYawOffset(float Angle);
	FFloatSpringState RootYawOffsetSpringState;
};
