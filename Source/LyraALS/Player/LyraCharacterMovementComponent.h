// 版权没有，拷贝自由。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LyraCharacterMovementComponent.generated.h"

USTRUCT(BlueprintType)
struct FLyraCharacterGroundInfo
{
	GENERATED_BODY()

	FLyraCharacterGroundInfo()
		: LastUpdateFrame(0)
		  , GroundDistance(0.0f)
	{
	}

	uint64 LastUpdateFrame;

	UPROPERTY(BlueprintReadOnly)
	float GroundDistance;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LYRAALS_API ULyraCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULyraCharacterMovementComponent();
	// Returns the current ground info.  Calling this will update the ground info if it's out of date.
	UFUNCTION(BlueprintCallable, Category = "Lyra|CharacterMovement")
	const FLyraCharacterGroundInfo& GetGroundInfo();

protected:
	// Cached ground info for the character.  Do not access this directly!  It's only updated when accessed via GetGroundInfo().
	FLyraCharacterGroundInfo CachedGroundInfo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|CharacterMovement")
	float GroundTraceDistance = 1000.f;
	
	UPROPERTY(Transient)
	bool bHasReplicatedAcceleration = false;
};
