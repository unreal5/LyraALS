#pragma once
#include "CoreMinimal.h"
#include "Gait.Generated.h"

UENUM(BlueprintType)
enum class EGait : uint8
{
	Walking = 0,
	Jogging = 1,
	Crouch = 2,
};
