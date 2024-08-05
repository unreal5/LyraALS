#pragma once

#include "CoreMinimal.h"
#include "WeaponSocket.generated.h"

USTRUCT(BlueprintType)
struct FWeaponSocketName
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName PistolUnEquipped = TEXT("PistolUnEquip");

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName RifleUnEquipped = TEXT("RifleUnEquip");

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName WeaponEquipped = TEXT("WeaponEquipped");
};
