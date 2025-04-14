#pragma once
#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ELocomotionDirection : uint8
{
	Forward UMETA(DisplayName = "前"),
	Backward UMETA(DisplayName = "后"),
	Right UMETA(DisplayName = "右"),
	Left UMETA(DisplayName = "左"),
};