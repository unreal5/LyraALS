// Lyra高级运动系统


#include "Character/LyraCharacter.h"

#include "AnimInterface/AnimationBlueprint.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "AnimLayer/AnimInst_BaseLayer.h"
#include "GameFramework/CharacterMovementComponent.h"
// Sets default values
ALyraCharacter::ALyraCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 700.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.0f, 50.0f, 70.0f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// 启用角色下蹲
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->CrouchedHalfHeight = 60.f;
}

// Called when the game starts or when spawned
void ALyraCharacter::BeginPlay()
{
	Super::BeginPlay();
	// 默认没有装备武器
	SetEquippedGun(EGuns::Unarmed);
	// 默认步态为jogging
	UpdateGait(EGait::Jogging);
}

void ALyraCharacter::SetEquippedGun(EGuns NewEquippedGuns)
{
	if (EquippedGun == NewEquippedGuns)
	{
		return;
	}

	EquippedGun = NewEquippedGuns;
	IAnimationBlueprint::Execute_ReceiveEquippedGun(GetMesh()->GetAnimInstance(), NewEquippedGuns);
	switch (EquippedGun)
	{
	case EGuns::Unarmed:
		GetMesh()->LinkAnimClassLayers(UnarmedAnimLayer);
		break;
	case EGuns::Pistol:
		GetMesh()->LinkAnimClassLayers(PistolAnimLayer);
		break;
	case EGuns::Rifle:
		GetMesh()->LinkAnimClassLayers(RifleAnimLayer);
		break;
	default:
		//组合键会触发
		check(0);
	}
}

void ALyraCharacter::UpdateGait(EGait Gait)
{
	if (Gait == EGait::Crouching)
	{
		// 当前如果是蹲下，就切换到jogging，否则切换到crouching
		CurrentGait = CurrentGait == EGait::Crouching ? EGait::Jogging : EGait::Crouching;
		CurrentGait == EGait::Crouching ? Crouch() : UnCrouch();
	}
	else
	{
		CurrentGait = Gait;
	}
	IAnimationBlueprint::Execute_ReceiveCurrentGait(GetMesh()->GetAnimInstance(), CurrentGait);

	FGaitSettings* Settings = GaitSettings.Find(CurrentGait);
	check(Settings);

	GetCharacterMovement()->MaxWalkSpeed = Settings->MaxWalkSpeed;
	GetCharacterMovement()->MaxAcceleration = Settings->MaxAcceleration;
	GetCharacterMovement()->BrakingDecelerationWalking = Settings->BrakingDeceleration;
	GetCharacterMovement()->BrakingFrictionFactor = Settings->BrakingFrictionFactor;
	GetCharacterMovement()->BrakingFriction = Settings->BrakingFriction;
	GetCharacterMovement()->bUseSeparateBrakingFriction = Settings->UseSeparateBrakingFriction;
}

// 响应瞄准按键
void ALyraCharacter::SetAiming(bool bAiming)
{
	EGait NewGait = bAiming ? EGait::Walking : EGait::Jogging;
	UpdateGait(NewGait);
}

// Called every frame
void ALyraCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ALyraCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
