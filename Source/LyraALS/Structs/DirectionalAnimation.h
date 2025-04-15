#pragma once

#include "CoreMinimal.h"
#include "DirectionalAnimation.generated.h"

USTRUCT(BlueprintType)
struct FDirectionalAnimation
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(DisplayName = "向前动画"))
	TObjectPtr<UAnimSequenceBase> ForwardAnim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(DisplayName = "向后动画"))
	TObjectPtr<UAnimSequenceBase> BackwardAnim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(DisplayName = "向右动画"))
	TObjectPtr<UAnimSequenceBase> RightAnim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(DisplayName = "向左动画"))
	TObjectPtr<UAnimSequenceBase> LeftAnim;
};
