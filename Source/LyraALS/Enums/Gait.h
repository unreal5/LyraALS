#pragma once
#include "CoreMinimal.h"

UENUM(Blueprintable, BlueprintType)
enum class EGait : uint8
{
	Walking = 0 UMETA(DisplayName = "走路"),
	Jogging = 1 UMETA(DisplayName = "慢跑"),
};