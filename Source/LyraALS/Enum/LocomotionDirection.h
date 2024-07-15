#pragma once
#include "CoreMinimal.h"
#include "LocomotionDirection.generated.h"

UENUM(BlueprintType)
enum class ELocomotionDirection : uint8
{
	Forward,
	Backward,
	Left,
	Right,
};
