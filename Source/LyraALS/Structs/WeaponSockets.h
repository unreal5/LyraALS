#pragma once
#include "CoreMinimal.h"
#include "WeaponSockets.generated.h"
USTRUCT(BlueprintType, Blueprintable)
struct FWeaponSockets
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName PistolUnEquipped = "PistolUnEquipped";
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName RifleUnEquipped = "RifleUnEquipped";
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName WeaponEquipped = "WeaponEquipped";
};