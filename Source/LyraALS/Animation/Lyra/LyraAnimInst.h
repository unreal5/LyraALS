// 版权没有，拷贝自由。

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimInstanceProxy.h"
#include "Enumeration/EnumTypes.h"
#include "Interface/CombatInterface.h"
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
};

UCLASS()
class LYRAALS_API ULyraAnimInst : public UAnimInstance,public ICombatInterface
{
	GENERATED_BODY()

protected:
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;
	virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy) override;

public:
	virtual void NativePostEvaluateAnimation() override;
	virtual void ReceiveEquipWeapon_Implementation(EGunType NewGunType) override;
protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category="武器")
	EGunType EquippedGun {EGunType::UnArmed};
private:
	UPROPERTY(Transient)
	FLyraAnimInstProxy LyraAnimInstProxy;
};
