// 版权没有，拷贝自由。


#include "LyraAnimInst.h"

#include "KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/LyraCharacter.h"

FLyraAnimInstProxy::FLyraAnimInstProxy(UAnimInstance* InAnimInstance) : FAnimInstanceProxy(InAnimInstance)
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
	// 速度相关。
	CharacterVelocity = MovementComp->Velocity;
	// 加速度相关。
	CurrentAcceleration = MovementComp->GetCurrentAcceleration();
	// 在动画图层中使用PropertyAccess的变量，必须在此处同步
	LastFrameWorldLocation = WorldLocation;
	WorldLocation = LyraCharacter->GetActorLocation();

	// 旋转相关。
	WorldRotation = LyraCharacter->GetActorRotation();
}

void FLyraAnimInstProxy::Update(float DeltaSeconds)
{
	FAnimInstanceProxy::Update(DeltaSeconds);

	GetVelocityData();
	GetAccelerationData();
	GetLocationData(DeltaSeconds);
	GetRotationData(DeltaSeconds);
	UpdateOrientationData();
}

void FLyraAnimInstProxy::GetVelocityData()
{
	// 速度相关的必须以CharacterVelocity为基础。
	CharacterVelocity2D = FVector{CharacterVelocity.X, CharacterVelocity.Y, 0.f};
	GroundSpeed = CharacterVelocity2D.Size();
	HasVelocity = GroundSpeed > KINDA_SMALL_NUMBER;
}

void FLyraAnimInstProxy::GetAccelerationData()
{
	CurrentAcceleration2D = FVector{CurrentAcceleration.X, CurrentAcceleration.Y, 0.f};
	bHasAcceleration = CurrentAcceleration2D.SizeSquared() > KINDA_SMALL_NUMBER;
	// 此时已经有速度和加速度数据，可用来计算速度和加速度的夹角等数据。
	NormalizedDotProductBetweenAccelerationAndVelocity = CurrentAcceleration2D.GetSafeNormal().Dot(CharacterVelocity2D.GetSafeNormal());
}

void FLyraAnimInstProxy::GetLocationData(float DeltaTime)
{
	DisplacementPerFrame = FVector::Dist(WorldLocation, LastFrameWorldLocation);
}

void FLyraAnimInstProxy::GetRotationData(float DeltaTime)
{
	// 此时代理的数据已经过时，是上一帧的数据。
	float DeltaActorYaw = WorldRotation.Yaw - LastFrameActorYaw;
	DeltaActorYaw = UKismetMathLibrary::SafeDivide(DeltaActorYaw, DeltaTime * 6.f);
	DeltaActorYaw = FMath::ClampAngle(DeltaActorYaw, -90.f, 90.f);
	// 更新当前帧数据
	LeanAngle = DeltaActorYaw;
	//根据上一帧方向调整LeanAngle;
	if (VelocityLocomotionDirection == ELocomotionDirection::Backward)
	{
		LeanAngle *= -1.f;
	}
	// 保存当前帧数据，供下一帧使用
	LastFrameActorYaw = WorldRotation.Yaw;
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
	
	// 计算加速度和角色朝向的夹角，用于判断角色做pivot运动时选择哪个方向的动画
	AccelLocomotionAngle = UKismetAnimationLibrary::CalculateDirection(CurrentAcceleration2D, WorldRotation);
	AccelLocomotionDirection = CalculateLocomotionDirection(AccelLocomotionAngle,AccelLocomotionDirection);
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

void ULyraAnimInst::ReceiveCurrentGait_Implementation(EGaitType NewGait,
                                                      const FPredictGroundMovementStopLocationParams& GaitSettings)
{
	InComingGait = NewGait;
	CurrentGaitPredictParams = GaitSettings;
}


// 动画评估后调用，运行于游戏线程
void ULyraAnimInst::NativePostEvaluateAnimation()
{
	Super::NativePostEvaluateAnimation();
	FLyraAnimInstProxy& Proxy = GetProxyOnGameThread<FLyraAnimInstProxy>();
	// 从动画代理中同步数据到动画实例中
	// 速度相关
	CharacterVelocity = Proxy.CharacterVelocity;
	CharacterVelocity2D = Proxy.CharacterVelocity2D;
	GroundSpeed = Proxy.GroundSpeed;
	HasVelocity = Proxy.HasVelocity;

	// 加速度相关
	CurrentAcceleration = Proxy.CurrentAcceleration;
	CurrentAcceleration2D = Proxy.CurrentAcceleration2D;
	bIsAccelerating = Proxy.bHasAcceleration;
	NormalizedDotProductBetweenAccelerationAndVelocity = Proxy.NormalizedDotProductBetweenAccelerationAndVelocity;

	// 位置相关
	WorldLocation = Proxy.WorldLocation;
	DisplacementPerFrame = Proxy.DisplacementPerFrame;
	// 旋转相关
	WorldRotation = Proxy.WorldRotation;
	LeanAngle = Proxy.LeanAngle;

	// 方向相关
	VelocityLocomotionAngle = Proxy.VelocityLocomotionAngle;
	LastFrameVelocityLocomotionDirection = VelocityLocomotionDirection;
	VelocityLocomotionDirection = Proxy.VelocityLocomotionDirection;
	AccelLocomotionDirection = Proxy.AccelLocomotionDirection;
	AccelLocomotionAngle = Proxy.AccelLocomotionAngle;
	// 步态相关
	GetCharacterStates();
}

void ULyraAnimInst::GetCharacterStates()
{
	LastFrameGait = CurrentGait;
	CurrentGait = InComingGait;
	IsGaitChanged = (CurrentGait != LastFrameGait);
}
