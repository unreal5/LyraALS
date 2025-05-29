#pragma once
#include "CoreMinimal.h"
#include "GaitSetting.generated.h"
USTRUCT(BlueprintType, Blueprintable)
struct FGaitSetting : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintreadWrite, Category = "GaitSetting")
	float MaxWalkSpeed = 400.f;

	UPROPERTY(EditDefaultsOnly, BlueprintreadWrite, Category = "GaitSetting")
	float MaxAcceleration = 2048.f;

	UPROPERTY(EditDefaultsOnly, BlueprintreadWrite, Category = "GaitSetting")
	float BrakingDeceleration = 2048.f;

	UPROPERTY(EditDefaultsOnly, BlueprintreadWrite, Category = "GaitSetting")
	float BrakingFrictionFactor = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintreadWrite, Category = "GaitSetting")
	float BrakingFriction = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintreadWrite, Category = "GaitSetting")
	bool UseSeparateBrakingFriction = false;
};