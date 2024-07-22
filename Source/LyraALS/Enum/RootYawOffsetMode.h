#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintTypeConversions.h"
#include "RootYawOffsetMode.generated.h"

UENUM(BlueprintType)
enum class ERootYawOffsetMode : uint8
{
	Accumulate, // idle状态时要累加
	BlendOut,
	Hold,
};
