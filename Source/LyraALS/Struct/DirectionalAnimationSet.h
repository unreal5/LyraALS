#pragma once
#include "CoreMinimal.h"
#include "DirectionalAnimationSet.generated.h"

USTRUCT(BlueprintType)
struct FDirectionalAnimationSet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimSequenceBase> ForwardAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimSequenceBase> BackwardAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimSequenceBase> LeftAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimSequenceBase> RightAnim;
};
