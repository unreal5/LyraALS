// 版权没有，拷贝自由。


#include "LyraAnimInst.h"

#include "KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/LyraCharacter.h"

FLyraAnimInstProxy::FLyraAnimInstProxy(UAnimInstance* InAnimInstance) : FAnimInstanceProxy(InAnimInstance),
                                                                        VelocityLocomotionDirection()
{
}

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
	WorldLocation = LyraCharacter->GetActorLocation();
	WorldRotation = LyraCharacter->GetActorRotation();

	CharacterVelocity = MovementComp->Velocity;
}

void FLyraAnimInstProxy::Update(float DeltaSeconds)
{
	FAnimInstanceProxy::Update(DeltaSeconds);

	GetLocationData();
	GetRotationData();
	GetVelocityData();
	UpdateOrientationData();
}


void FLyraAnimInstProxy::GetLocationData() {
}

void FLyraAnimInstProxy::GetRotationData() {
}

void FLyraAnimInstProxy::GetVelocityData()
{
	// 速度相关的必须以CharacterVelocity为基础。
	CharacterVelocity2D = FVector{CharacterVelocity.X, CharacterVelocity.Y, 0.f};
	GroundSpeed = CharacterVelocity2D.Size();
	HasVelocity = GroundSpeed > KINDA_SMALL_NUMBER;
}

void FLyraAnimInstProxy::UpdateOrientationData()
{
	/*
	* Returns degree of the angle between Velocity and Rotation forward vector The range of return will be from [-180, 180]. Useful for feeding directional blendspaces.
	* Params: 
	*		Velocity — The velocity to use as direction relative to BaseRotation
	*		BaseRotation — The base rotation, e.g. of a pawn
	*/
	VelocityLocomotionAngle = UKismetAnimationLibrary::CalculateDirection(CharacterVelocity2D, WorldRotation);
	VelocityLocomotionDirection = CalculateLocomotionDirection(VelocityLocomotionAngle, VelocityLocomotionDirection);
}

ELocomotionDirection FLyraAnimInstProxy::CalculateLocomotionDirection(float InAngle,
                                                                      ELocomotionDirection CurrentDirection,
                                                                      float DeadZone,
                                                                      float BackwardMin, float BackwardMax,
                                                                      float ForwardMin, float ForwardMax)
{
	switch (CurrentDirection)
	{
	case ELocomotionDirection::Forward:
		ForwardMin -= DeadZone;
		ForwardMax += DeadZone;
		break;
	case ELocomotionDirection::Backward:
		BackwardMin += DeadZone;
		BackwardMax -= DeadZone;
		break;
	case ELocomotionDirection::Left:
		ForwardMin += DeadZone;
		BackwardMin -= DeadZone;
		break;
	case ELocomotionDirection::Right:
		ForwardMax -= DeadZone;
		BackwardMax += DeadZone;
		break;
	}

	if (InAngle < BackwardMin || InAngle > BackwardMax)
	{
		return ELocomotionDirection::Backward;
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(InAngle, ForwardMin, ForwardMax))
	{
		return ELocomotionDirection::Forward;
	}
	return InAngle > 0.f ? ELocomotionDirection::Right : ELocomotionDirection::Left;
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

void ULyraAnimInst::ReceiveCurrentGait_Implementation(EGaitType NewGait, const FGaitSettings& GaitSettings)
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
	// 位置相关
	WorldLocation = Proxy.WorldLocation;
	// 旋转相关
	WorldRotation = Proxy.WorldRotation;
	// 速度相关
	CharacterVelocity = Proxy.CharacterVelocity;
	CharacterVelocity2D = Proxy.CharacterVelocity2D;
	GroundSpeed = Proxy.GroundSpeed;
	HasVelocity = Proxy.HasVelocity;

	// 方向相关
	VelocityLocomotionAngle = Proxy.VelocityLocomotionAngle;
	VelocityLocomotionDirection = Proxy.VelocityLocomotionDirection;
}
