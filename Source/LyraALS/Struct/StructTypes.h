// 版权没有，拷贝自由。

#pragma once

#include "CoreMinimal.h"
#include "StructTypes.generated.h"

USTRUCT(BlueprintType)
struct LYRAALS_API FGaitSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxWalkingSpeed{600.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxAcceleration{2048.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BrakingDeceleration{2048.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BrakingFrictionFactor{2.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BrakingFriction{0.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool UseSeperateBrakingFriction{false};
};
