// Lyra高级运动系统

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimInterface/AnimationBlueprint.h"
#include "LyraAnimInstance.generated.h"

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

	// 速度
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "速度")
	FVector CharacterVelocity = FVector::ZeroVector;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "速度")
	FVector CharacterVelocity2D = FVector::ZeroVector;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "速度")
	float CharacterSpeed2D = 0.0f;
	UFUNCTION(BlueprintCallable, Category = "速度", meta=(BlueprintThreadSafe))
	void GetVelocityData();

	UFUNCTION(BlueprintCallable, Category = "基类|位置", meta=(BlueprintThreadSafe))
	void GetLocationData();
	UFUNCTION(BlueprintCallable, Category = "基类|位置", meta=(BlueprintThreadSafe, ToolTip="必须在获取角色旋转角度后调用"))
	void UpdateOrientationData();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "基础|位置", meta=(ToolTip="角色运动方向（速度）与角色前方的夹角", ShortTooltip="角色运动方向与角色前方的夹角"))
	float VelocityLocomotionAngle = 0.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "基础|位置")
	FVector WorldLocation = FVector::ZeroVector;

	UFUNCTION(BlueprintCallable, Category = "基类|旋转", meta=(BlueprintThreadSafe))
	void GetRotationData();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "基础|旋转")
	FRotator WorldRotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "基础|位置")
	ELocomotionDirection LocomotionDirection = ELocomotionDirection::Forward;

	UFUNCTION(BlueprintCallable, Category = "基类|位置", meta=(BlueprintThreadSafe, ToolTip="计算角色运动方向"))
	void CalculateLocomotionDirection(float CurrentLocomotionAngle, ELocomotionDirection CurrentDirection, float DeadZone = 10.f, float ForwardThreshold = 50.f, float BackwardThreshold = 130.f);
};