// Lyra高级运动系统

#pragma once

#include "CoreMinimal.h"
#include "Enums/Enums.h"
#include "UObject/Interface.h"
#include "AnimationBlueprint.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAnimationBlueprint : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LYRAALS_API IAnimationBlueprint
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "动画蓝图接口",  meta=(BlueprintThreadSafe))
	void ReceiveEquippedGun(EGuns NewEquippedGun);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "动画蓝图接口",  meta=(BlueprintThreadSafe))
	void ReceiveCurrentGait(EGait NewGait);
};