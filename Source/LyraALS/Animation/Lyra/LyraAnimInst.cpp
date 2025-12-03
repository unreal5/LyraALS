// 版权没有，拷贝自由。


#include "LyraAnimInst.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Player/LyraCharacter.h"

// 动画实例代理
void FLyraAnimInstProxy::PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds)
{
	FAnimInstanceProxy::PreUpdate(InAnimInstance, DeltaSeconds);

	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(InAnimInstance->GetOwningActor());
	if (!LyraCharacter) return;
	UCharacterMovementComponent* MovementComp = LyraCharacter->GetCharacterMovement();
	if (!MovementComp) return;
	// 同步需要的数据到动画代理中
	// 在动画图层中使用PropertyAccess的变量，必须在此处同步
	CharacterVelocity = MovementComp->Velocity;
	
}

void FLyraAnimInstProxy::Update(float DeltaSeconds)
{
	FAnimInstanceProxy::Update(DeltaSeconds);
	
	GetVelocityData();
}
void FLyraAnimInstProxy::GetVelocityData() 
{
	// 速度相关的必须以CharacterVelocity为基础。
	CharacterVelocity2D = FVector{CharacterVelocity.X, CharacterVelocity.Y, 0.f};
	GroundSpeed = CharacterVelocity2D.Size();
	HasVelocity = GroundSpeed > KINDA_SMALL_NUMBER;
}

/****************************************************************************************************/
/*										动画实例														*/
/****************************************************************************************************/

FAnimInstanceProxy* ULyraAnimInst::CreateAnimInstanceProxy()
{
	return new FLyraAnimInstProxy{this};
}

void ULyraAnimInst::DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy)
{
	Super::DestroyAnimInstanceProxy(InProxy);
}



void ULyraAnimInst::ReceiveEquipWeapon_Implementation(EGunType NewGunType)
{
	EquippedGun = NewGunType;
}

void ULyraAnimInst::ReceiveGaitChanged_Implementation(EGaitType NewGait, const FGaitSettings& GaitSettings)
{
	CurrentGait = NewGait;
	CurrentGaitSettings = GaitSettings;
}

// 动画评估后调用，运行于游戏线程
void ULyraAnimInst::NativePostEvaluateAnimation()
{
	Super::NativePostEvaluateAnimation();
	FLyraAnimInstProxy& Proxy = GetProxyOnGameThread<FLyraAnimInstProxy>();
	// 从动画代理中同步数据到动画实例中
	CharacterVelocity = Proxy.CharacterVelocity;
	CharacterVelocity2D = Proxy.CharacterVelocity2D;
	GroundSpeed = Proxy.GroundSpeed;
	HasVelocity = Proxy.HasVelocity;
}


