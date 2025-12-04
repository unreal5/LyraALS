// 版权没有，拷贝自由。

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enumeration/EnumTypes.h"
#include "Struct/StructTypes.h"
#include "BaseAnimLayer.generated.h"

class ULyraAnimInst;
/**
 * 
 */
UCLASS()
class LYRAALS_API UBaseAnimLayer : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Lyra|Animation", meta=(DisplayName="获取主动画实例", BlueprintThreadSafe))
	ULyraAnimInst* GetABPBase() const;

	UFUNCTION(BlueprintPure, Category="Lyra|Animation", meta=(DisplayName="基于步态和方向选择动画", BlueprintThreadSafe))
	UAnimSequenceBase* SelectAnimationBasedOnGaitAndLocomotionDirection(EGaitType InGait, ELocomotionDirection InLocomotionDirection,
	                                              const TMap<EGaitType, FDirectionalAnimation>& InAnimationMap) const;
};
