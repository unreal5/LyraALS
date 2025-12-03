// 版权没有，拷贝自由。

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimInstanceProxy.h"
#include "LyraAnimInst.generated.h"

/**
 * 动画代理
 */

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
class LYRAALS_API ULyraAnimInst : public UAnimInstance
{
	GENERATED_BODY()

protected:
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;
	virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy) override;

public:
	virtual void NativePostEvaluateAnimation() override;
private:
	UPROPERTY(Transient)
	FLyraAnimInstProxy LyraAnimInstProxy;
};
