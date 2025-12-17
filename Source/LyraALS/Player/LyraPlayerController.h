// 版权没有，拷贝自由。

#pragma once

#include "CoreMinimal.h"
#include "Enumeration/EnumTypes.h"
#include "GameFramework/PlayerController.h"
#include "LyraPlayerController.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;


// 武器改变
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchWeaponSignature, EGunType, NewGunType);
// 步态改变
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGaitChangedSignature, EGaitType, NewGaitType);
UCLASS()
class LYRAALS_API ALyraPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MouseLookAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SwitchWeaponAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> AimAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> CrouchingAction;
	
	virtual void SetupInputComponent() override;

public:
	ALyraPlayerController();

	UPROPERTY(BlueprintAssignable)
	FOnSwitchWeaponSignature OnSwitchWeapon;
	UPROPERTY(Blueprintable)
	FOnGaitChangedSignature OnGaitChanged;

private:
	void Move(const FInputActionValue& Value);
	void MouseLook(const FInputActionValue& Value);
	void SwitchWeapon(const FInputActionValue& Value);
};
