#pragma once
UENUM(BlueprintType)
enum class ERootYawOffsetMode : uint8
{
	Accumulate UMETA(DisplayName = "Accumulate"),
	BlendOut UMETA(DisplayName = "BlendOut"),
	Hold UMETA(DisplayName = "Hold"),
};