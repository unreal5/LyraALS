#pragma once
#include "CoreMinimal.h"
#include "Structs.generated.h"

USTRUCT(BlueprintType)
struct FGaitSettings
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gait")
	float MaxWalkSpeed = 400.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gait")
	float MaxAcceleration = 2048.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gait")
	float BrakingDeceleration = 2048.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gait")
	float BrakingFrictionFactor = 2.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gait")
	float BrakingFriction = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gait")
	bool UseSeparateBrakingFriction = false;
};