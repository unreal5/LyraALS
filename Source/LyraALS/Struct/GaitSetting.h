#pragma once

#include "CoreMinimal.h"
#include "GaitSetting.generated.h"

USTRUCT(BlueprintType)
struct FGaitSetting
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxWalkSpeed = 800;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxAcceleration = 2048;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BrakingDeceleration = 2048.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BrakingFrictionFactor = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BrakingFriction = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool UseSeperateBrakingFriction = false;
};
