// Lyra高级运动系统

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LyraPlayerController.generated.h"

class ALyraCharacter;
class UInputAction;
class UInputMappingContext;
/**
 * 
 */
UCLASS()
class LYRAALS_API ALyraPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* aPawn) override;

	UPROPERTY(EditDefaultsOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputMappingContext> DefaultInputContext;

	UPROPERTY(EditDefaultsOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputAction> LookUpAction;

	UPROPERTY(EditDefaultsOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputAction> SwitchWeaponAction;

	UPROPERTY(EditDefaultsOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditDefaultsOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputAction> CrouchAction;
private:
	UPROPERTY()
	TObjectPtr<ALyraCharacter> LyraCharacter;
};