// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums/Gait.h"

#include "Enums/GunTypes.h"

#include "GameFramework/Character.h"
#include "Structs/GaitSetting.h"
#include "Structs/WeaponSockets.h"
#include "LyraCharacter.generated.h"

struct FInputActionValue;

UCLASS()
class LYRAALS_API ALyraCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Key1Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Key2Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Key3Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LinkAnimClassLayer, meta = (AllowPrivateAccess = "true"))
	TMap<EGunTypes, TSubclassOf<UAnimInstance>> LinkAnimClassMap;

	/* 用数据表代替 */
	/*
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GaitSettings, meta = (AllowPrivateAccess = "true"))
	TMap<EGait, FGaitSetting> GaitSettingsMap;
	*/
	// Fire
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Fire, meta = (AllowPrivateAccess = "true"))
	bool CanFire = true;

	FTimerHandle FireTimer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Fire, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* PistolFireMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Fire, meta = (AllowPrivateAccess = "true"))
	UAnimationAsset* PistolFireAnimation;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Fire, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* RifleFireMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Fire, meta = (AllowPrivateAccess = "true"))
	UAnimationAsset* RifleFireAnimation;

	// Reload
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Reload, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* PistolReloadMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Reload, meta = (AllowPrivateAccess = "true"))
	UAnimationAsset* PistolReloadAnimation;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Reload, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* RifleReloadMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Reload, meta = (AllowPrivateAccess = "true"))
	UAnimationAsset* RifleReloadAnimation;
public:
	// Sets default values for this character's properties
	ALyraCharacter();
	virtual void BeginPlay() override;

	// 武器相关
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	USkeletalMeshComponent* PistolMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	USkeletalMeshComponent* RifleMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FWeaponSockets WeaponSockets;
	
	UFUNCTION(BlueprintCallable)
	void ChangeWeapon(EGunTypes GunType);

	// 实现timeline
	UFUNCTION(BlueprintImplementableEvent)
	void OnAimingTimelineUpdate(bool bAiming);
protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void OnCrouch(const FInputActionValue& Value);

	// Left mouse button pressed
	void OnFire(const FInputActionValue& Value);
	// reload
	void OnReload(const FInputActionValue& Value);
	
	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	EGunTypes EquippedGunType = EGunTypes::UnArmed;
	EGait CurrentGait = EGait::Jogging;

	UPROPERTY(EditDefaultsOnly, Category="数据表")
	TObjectPtr<UDataTable> GaitSettingsDataTable;
private:
	bool OnEquippedGunChanged();
	bool UpdateGait(EGait NewGait);
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	virtual void Tick(float DeltaSeconds) override;
};
