// Lyra高级运动系统

#pragma once

#include "CoreMinimal.h"
#include "Enums/Enums.h"
#include "GameFramework/Character.h"
#include "Struct/Structs.h"
#include "LyraCharacter.generated.h"

class UAnimInst_BaseLayer;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class LYRAALS_API ALyraCharacter : public ACharacter
{
	GENERATED_BODY()
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

public:
	ALyraCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;

	void SetEquippedGun(EGuns NewEquippedGuns);
	void UpdateGait(EGait Gait);
	void SetAiming(bool bAiming);
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "基础属性")
	EGuns EquippedGun = EGuns::EG_Max; // 非正常状态

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "基础属性")
	EGait CurrentGait = EGait::Walking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "基础属性")
	TMap<EGait, FGaitSettings> GaitSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "动画层")
	TSubclassOf<UAnimInst_BaseLayer> UnarmedAnimLayer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "动画层")
	TSubclassOf<UAnimInst_BaseLayer> PistolAnimLayer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "动画层")
	TSubclassOf<UAnimInst_BaseLayer> RifleAnimLayer;
};