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