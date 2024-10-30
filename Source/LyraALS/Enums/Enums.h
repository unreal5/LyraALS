#pragma once
#include "CoreMinimal.h"
#include "Enums.generated.h"

UENUM(BlueprintType)
enum class EGuns : uint8
{
	Unarmed,
	Pistol,
	Rifle,
	EG_Max UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EGait : uint8
{
	Walking,
	Jogging,
	EG_Max UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ELocomotionDirection : uint8
{
	Forward UMETA(DisplayName = "前"),
	Backward UMETA(DisplayName = "后"),
	Right UMETA(DisplayName = "右"),
	Left UMETA(DisplayName = "左"),
	ELD_Max UMETA(Hidden)
};