// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enum/Gait.h"
#include "Enum/Gun.h"
#include "UObject/Interface.h"
#include "AnimBPInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAnimBPInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LYRAALS_API IAnimBPInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "动画蓝图接口")
	bool ReceivedEquippedGun(EGun EquippedGun);

	UFUNCTION(BlueprintImplementableEvent, Category = "动画蓝图接口")
	void ReceivedCurrentGait(EGait Gait);

	UFUNCTION(BlueprintImplementableEvent, Category = "动画蓝图接口")
	void ReceivedGroundDistance(float InComingGroundDistance);
};
