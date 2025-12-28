// 版权没有，拷贝自由。


#include "LyraAnimInst.h"

#include "KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/LyraCharacter.h"
#include "Player/LyraCharacterMovementComponent.h"

namespace
{
	ELocomotionDirection CalculateLocomotionDirection(float InAngle, ELocomotionDirection CurrentDirection,
	                                                  float DeadZone = 20.f, float BackwardMin = -130.f,
	                                                  float BackwardMax = 130.f, float ForwardMin = -50.f,
	                                                  float ForwardMax = 50.f);

	float SetRootYawOffset(float CompensationAngle);
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

void ULyraAnimInst::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwningCharacter = Cast<ALyraCharacter>(GetOwningActor());
	if (OwningCharacter.Get())
	{
		CharacterMovementComponent = OwningCharacter->GetCharacterMovement();
	}
}

void ULyraAnimInst::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!OwningCharacter.Get()) return;
	if (!CharacterMovementComponent.Get())
	{
		CharacterMovementComponent = OwningCharacter->GetCharacterMovement();
	}
	if (!CharacterMovementComponent.Get()) return;

	// 同步最基本数据---其余数据在NativeThreadSafeUpdateAnimation中更新
	CharacterVelocity = CharacterMovementComponent->Velocity;
	// 加速度相关。
	CurrentAcceleration = CharacterMovementComponent->GetCurrentAcceleration();
	// 在动画图层中使用PropertyAccess的变量，必须在此处同步
	LastFrameWorldLocation = WorldLocation;
	WorldLocation = OwningCharacter->GetActorLocation();

	// 旋转相关。
	WorldRotation = OwningCharacter->GetActorRotation();

	// 运动模式相关
	CurrentFrameMovementMode = CharacterMovementComponent->MovementMode;
	// 重力相关
	GravityZ = CharacterMovementComponent->GetGravityZ() * CharacterMovementComponent->GravityScale;

	// 更新地面数据
	ULyraCharacterMovementComponent* CharMoveComp = CastChecked<ULyraCharacterMovementComponent>(
		CharacterMovementComponent);
	const FLyraCharacterGroundInfo& GroundInfo = CharMoveComp->GetGroundInfo();
	GroundDistance = GroundInfo.GroundDistance;
}

// 线程安全的更新动画，运行于动画线程
void ULyraAnimInst::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	GetVelocityData();
	GetAccelerationData();
	GetLocationData(DeltaSeconds);
	GetRotationData(DeltaSeconds);
	UpdateOrientationData();
	GetCharacterStates(DeltaSeconds);
	UpdateRootYawOffset(DeltaSeconds);
}

void ULyraAnimInst::GetVelocityData()
{
	// 速度相关的必须以CharacterVelocity为基础。
	CharacterVelocity2D = FVector{CharacterVelocity.X, CharacterVelocity.Y, 0.f};
	GroundSpeed = CharacterVelocity2D.Size();
	HasVelocity = GroundSpeed > KINDA_SMALL_NUMBER;
}

void ULyraAnimInst::GetAccelerationData()
{
	CurrentAcceleration2D = FVector{CurrentAcceleration.X, CurrentAcceleration.Y, 0.f};
	bIsAccelerating = CurrentAcceleration2D.SizeSquared() > KINDA_SMALL_NUMBER;
	// 此时已经有速度和加速度数据，可用来计算速度和加速度的夹角等数据。
	NormalizedDotProductBetweenAccelerationAndVelocity = CurrentAcceleration2D.GetSafeNormal().Dot(
		CharacterVelocity2D.GetSafeNormal());
}

void ULyraAnimInst::GetLocationData(float DeltaTime)
{
	DisplacementPerFrame = FVector::Dist(WorldLocation, LastFrameWorldLocation);
}

void ULyraAnimInst::GetRotationData(float DeltaTime)
{
	// 此时代理的数据已经过时，是上一帧的数据。
	DeltaActorYaw = WorldRotation.Yaw - LastFrameActorYaw;
	float ClampedYawSpeed = UKismetMathLibrary::SafeDivide(DeltaActorYaw, DeltaTime * 6.f);
	ClampedYawSpeed = FMath::ClampAngle(ClampedYawSpeed, -90.f, 90.f);
	// 更新当前帧数据
	LeanAngle = ClampedYawSpeed;
	//根据上一帧方向调整LeanAngle;
	if (VelocityLocomotionDirection == ELocomotionDirection::Backward)
	{
		LeanAngle *= -1.f;
	}
	// 保存当前帧数据，供下一帧使用
	LastFrameActorYaw = WorldRotation.Yaw;
}

void ULyraAnimInst::UpdateOrientationData()
{
	/*
	 * Returns degree of the angle between Velocity and Rotation forward vector The range of return will be from [-180, 180]. Useful for feeding directional blend space.
	 * Params: 
	 *		Velocity — The velocity to use as direction relative to BaseRotation
	 *		Base Rotation — The base rotation, e.g. of a pawn
	 */
	VelocityLocomotionAngle = UKismetAnimationLibrary::CalculateDirection(CharacterVelocity2D, WorldRotation);
	VelocityLocomotionAngleWithOffset = UKismetMathLibrary::NormalizeAxis(VelocityLocomotionAngle - RootYawOffset);
	LastFrameVelocityLocomotionDirection = VelocityLocomotionDirection;
	VelocityLocomotionDirection = CalculateLocomotionDirection(VelocityLocomotionAngle, VelocityLocomotionDirection);

	// 计算加速度和角色朝向的夹角，用于判断角色做pivot运动时选择哪个方向的动画
	AccelLocomotionAngle = UKismetAnimationLibrary::CalculateDirection(CurrentAcceleration2D, WorldRotation);
	AccelLocomotionDirection = CalculateLocomotionDirection(AccelLocomotionAngle, AccelLocomotionDirection);
}

void ULyraAnimInst::GetCharacterStates(float DeltaTime)
{
	LastFrameGait = CurrentGait;
	CurrentGait = InComingGait;
	IsGaitChanged = (CurrentGait != LastFrameGait);

	LastFrameIsCrouching = IsCrouching;
	IsCrouching = InComingGait == EGaitType::Crouching;
	CrouchStateChanged = (IsCrouching != LastFrameIsCrouching);
	// 计算跳跃状态
	IsInAir = CurrentFrameMovementMode == EMovementMode::MOVE_Falling;
	const float ZVelocity = CharacterVelocity.Z;
	IsJumping = IsInAir && ZVelocity > 0.f;
	IsFalling = IsInAir && ZVelocity < 0.f;
	// 只有在IsJumping情况下才计算达最高点(Apex)的时间。
	TimeToJumpApex = IsJumping ? FMath::Abs(UKismetMathLibrary::SafeDivide(ZVelocity, GravityZ)) : 0.f;
	// 计算下落时间。
	if (IsFalling)
	{
		TimeFalling += DeltaTime;
	}
	else
	{
		if (IsJumping) // 刚起跳，重置下落时间。否则保持不变，我们可以用它来判断短跳还是长跳
		{
			TimeFalling = 0.f;
		}
	}
}


void ULyraAnimInst::UpdateRootYawOffset(float DeltaTime)
{
	switch (RootYawOffsetMode)
	{
	case ERootYawOffsetMode::Accumulate:
		{
			const float CompensationAngle = RootYawOffset - DeltaActorYaw;
			RootYawOffset = SetRootYawOffset(CompensationAngle);
		}
		break;
	case ERootYawOffsetMode::BlendOut:
		{
			const float InterpToZero = UKismetMathLibrary::FloatSpringInterp(
				RootYawOffset, 0.f, FloatSpringState, 80.f, 1.f, DeltaTime);
			RootYawOffset = SetRootYawOffset(InterpToZero);
		}
		break;
	case ERootYawOffsetMode::Hold:
		// Do nothing, hold the current RootYawOffset value
		break;
	}

	RootYawOffsetMode = ERootYawOffsetMode::BlendOut;
}


namespace
{
	ELocomotionDirection CalculateLocomotionDirection(float InAngle,
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


	float SetRootYawOffset(const float InAngle)
	{
		return UKismetMathLibrary::NormalizeAxis(InAngle);
	}
}
