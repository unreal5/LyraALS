#pragma once
#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EGunTypes : uint8
{
	UnArmed = 0 UMETA(DisplayName = "空手"), 
	Pistol = 1 UMETA(DisplayName = "手枪"),
	Rifle = 2 UMETA(DisplayName = "步枪"),
	Max UMETA(Hidden)
};