#pragma once
#include "CoreMinimal.h"
#include "LocomotionDirection.generated.h"

UENUM(BlueprintType)
enum class ELocomotionDirection : uint8
{
	Forward,
	Right,
	Backward,
	Left
};