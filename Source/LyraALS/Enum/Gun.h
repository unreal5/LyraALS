#pragma once
#include "CoreMinimal.h"

#include "Gun.generated.h"

UENUM(BlueprintType)
enum class EGun : uint8
{
	UnArmed = 0 UMETA(DisplayName = "Unarmed"),
	Pistol = 1 UMETA(DisplayName = "Pistol"),
	Rifle = 2 UMETA(DisplayName = "Rifle"),
};
