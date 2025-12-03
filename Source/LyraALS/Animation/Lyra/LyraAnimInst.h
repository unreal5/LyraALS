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
	explicit FLyraAnimInstProxy(UAnimInstance* InAnimInstance) : FAnimInstanceProxy(InAnimInstance) {}

protected:
	virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;
	virtual void Update(float DeltaSeconds) override;

	FVector CharacterVelocity;
	FVector CharacterVelocity2D;
	float GroundSpeed;
	bool HasVelocity;

private:
	void GetVelocityData();
	
	friend class ULyraAnimInst;
};

UCLASS()
class LYRAALS_API ULyraAnimInst : public UAnimInstance, public ICombatInterface
{
	GENERATED_BODY()

public:
	virtual void ReceiveEquipWeapon_Implementation(EGunType NewGunType) override;
	virtual void ReceiveGaitChanged_Implementation(EGaitType NewGait, const FGaitSettings& GaitSettings) override;
	
	virtual void NativePostEvaluateAnimation() override;

protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category="武器")
	EGunType EquippedGun{EGunType::UnArmed};
	
	UPROPERTY(Transient, BlueprintReadOnly, Category="步态")
	EGaitType CurrentGait{EGaitType::Walking};
	UPROPERTY(Transient, BlueprintReadOnly, Category="步态")
	FGaitSettings CurrentGaitSettings;
	
	// 速度相关数据
	UPROPERTY(Transient, BlueprintReadOnly, Category="VelocityData")
	FVector CharacterVelocity;
	UPROPERTY(Transient, BlueprintReadOnly, Category="VelocityData")
	FVector CharacterVelocity2D;
	UPROPERTY(Transient, BlueprintReadOnly, Category="VelocityData")
	float GroundSpeed{0.f};
	UPROPERTY(Transient, BlueprintReadOnly, Category="VelocityData")
	bool HasVelocity{false};
	

private:
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;
	virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy) override;
};
