// 版权没有，拷贝自由。

#pragma once

#include "CoreMinimal.h"
#include "EnumTypes.generated.h"

UENUM(BlueprintType, Blueprintable)
enum class EGunType : uint8
{
	UnArmed UMETA(DisplayName = "徙手"),
	Pistol UMETA(DisplayName = "手枪"),
	Rifle UMETA(DisplayName = "步枪"),
};

UENUM(BlueprintType, Blueprintable)
enum class EGaitType : uint8
{
	Walking UMETA(DisplayName = "行走"),
	Jogging UMETA(DisplayName = "慢跑"),
};

UENUM(BlueprintType, Blueprintable)
enum class ELocomotionDirection : uint8
{
	Forward UMETA(DisplayName = "前进"),
	Backward UMETA(DisplayName = "后退"),
	Left UMETA(DisplayName = "左移"),
	Right UMETA(DisplayName = "右移"),
};