// Lyra高级运动系统


#include "Animation/LyraAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "Character/LyraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void ULyraAnimInstance::NativeInitializeAnimation()
{
	LyraCharacter = Cast<ALyraCharacter>(GetOwningActor());
	if (LyraCharacter)
	{
		LyraCharacterMovement = LyraCharacter->GetCharacterMovement();
	}
}

void ULyraAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	DrawDebugMessage(DeltaSeconds);
}

UCharacterMovementComponent* ULyraAnimInstance::GetCharacterMovement()
{
	if (nullptr != LyraCharacterMovement)
	{
		return LyraCharacterMovement;
	}
	// 尝试重新获取
	if (nullptr == LyraCharacter)
	{
		if (GetLyraCharacter())
		{
			LyraCharacterMovement = LyraCharacter->GetCharacterMovement();
		}
	}

	return LyraCharacterMovement;
}

ALyraCharacter* ULyraAnimInstance::GetLyraCharacter()
{
	if (nullptr == LyraCharacter)
	{
		LyraCharacter = Cast<ALyraCharacter>(GetOwningActor());
	}
	return LyraCharacter;
}

void ULyraAnimInstance::ReceiveEquippedGun_Implementation(EGuns NewEquippedGun)
{
	EquippedGun = NewEquippedGun;
}

void ULyraAnimInstance::ReceiveCurrentGait_Implementation(EGait NewGait)
{
	IncomingGait = NewGait;
}

void ULyraAnimInstance::GetCharacterStates()
{
	LastFrameGait = CurrentGait;
	CurrentGait = IncomingGait;
	bIsGaitChanged = CurrentGait != LastFrameGait;

	LastFrameIsCrouching = bIsCrouching;
	bIsCrouching = CurrentGait == EGait::Crouching;
	CrouchStateChanged = bIsCrouching != LastFrameIsCrouching;
}

void ULyraAnimInstance::GetVelocityData()
{
	if (const auto CharacterMovement = GetCharacterMovement())
	{
		CharacterVelocity = CharacterMovement->Velocity;
		CharacterVelocity2D = FVector(CharacterVelocity.X, CharacterVelocity.Y, 0.f);
		CharacterSpeed2D = CharacterVelocity2D.Size();
	}
}

void ULyraAnimInstance::GetAccelerationData(float DeltaTime)
{
	auto LyraMovement = GetCharacterMovement();
	if (!LyraMovement) return;
	Acceleration = LyraMovement->GetCurrentAcceleration();
	Acceleration2D = FVector(Acceleration.X, Acceleration.Y, 0.f);
	IsAccelerating = !Acceleration2D.IsNearlyZero();
}

void ULyraAnimInstance::GetLocationData()
{
	if (auto LyraChar = GetLyraCharacter())
	{
		FVector LastFrameWorldLocation = WorldLocation;
		WorldLocation = LyraChar->GetActorLocation();
		DeltaLocation = (WorldLocation - LastFrameWorldLocation).Size2D();
	}
}

void ULyraAnimInstance::UpdateOrientationData()
{
	LastFrameLocomotionDirection = LocomotionDirection;
	// 速度方向与角色前方的夹角
	VelocityLocomotionAngle = UKismetAnimationLibrary::CalculateDirection(CharacterVelocity2D, WorldRotation);
	// 在起始阶段，RootYawOffset插值到0，要经历一个过程，对此进行补偿
	VelocityLocomotionAngleWithOffset = UKismetMathLibrary::NormalizeAxis(VelocityLocomotionAngle - RootYawOffset);

	LocomotionDirection = CalculateLocomotionDirection(VelocityLocomotionAngle, LocomotionDirection);
	// 加速度方向与角色前方的夹角
	AccelerationLocomotionAngle = UKismetAnimationLibrary::CalculateDirection(Acceleration2D, WorldRotation);
	AccelerationLocomotionDirection = CalculateLocomotionDirection(AccelerationLocomotionAngle,
	                                                               AccelerationLocomotionDirection);
}

void ULyraAnimInstance::GetRotationData(float DeltaTime)
{
	auto LyraChar = GetLyraCharacter();
	if (!LyraChar) return;

	//const auto LastFrameRotation = WorldRotation;
	WorldRotation = LyraChar->GetActorRotation();

	const auto LastFrameActorYaw = ActorYaw;
	ActorYaw = WorldRotation.Yaw;

	// TODO: 怀疑冗余转身是由于角度越界引发，使用球形插值解决
	DeltaActorYaw = ActorYaw - LastFrameActorYaw;

	LeanAngle = FMath::Clamp(DeltaActorYaw / DeltaTime * 0.2f, -90.f, 90.f);
	// 如果向后移动，角度取反
	if (LocomotionDirection == ELocomotionDirection::Backward)
	{
		LeanAngle *= -1.f;
	}
}

void ULyraAnimInstance::SetRootYawOffset(float Angle)
{
	//Clamps an angle to the range of (-180, 180].
	RootYawOffset = FRotator::NormalizeAxis(Angle);
}

void ULyraAnimInstance::UpdateRootYawOffset(float DeltaTime)
{
	if (RootYawOffsetMode == ERootYawOffsetMode::Accumulate)
	{
		RootYawOffset -= DeltaActorYaw;
		SetRootYawOffset(RootYawOffset);
	}

	if (RootYawOffsetMode == ERootYawOffsetMode::BlendOut)
	{
		SetRootYawOffset(UKismetMathLibrary::FloatSpringInterp(RootYawOffset, 0.f, RootYawOffsetSpringState, 80.f, 1.f,
		                                                       DeltaTime, 1.f, 0.5f));
	}
	RootYawOffsetMode = ERootYawOffsetMode::BlendOut;
}

ELocomotionDirection ULyraAnimInstance::CalculateLocomotionDirection(float CurrentLocomotionAngle,
                                                                     ELocomotionDirection CurrentDirection,
                                                                     float DeadZone,
                                                                     float ForwardThreshold,
                                                                     float BackwardThreshold)
{
	//LastFrameLocomotionDirection = LocomotionDirection;

	const auto AbsLocomotionAngle = FMath::Abs(CurrentLocomotionAngle);
	// 修正死区
	if (CurrentDirection == ELocomotionDirection::Forward && AbsLocomotionAngle <= (ForwardThreshold + DeadZone))
	{
		ForwardThreshold += DeadZone;
	}
	else if (AbsLocomotionAngle >= (BackwardThreshold + DeadZone))
	{
		BackwardThreshold -= DeadZone;
	}

	if (AbsLocomotionAngle <= ForwardThreshold)
	{
		return ELocomotionDirection::Forward;
	}
	else if (AbsLocomotionAngle >= BackwardThreshold)
	{
		return ELocomotionDirection::Backward;
	}
	else if (CurrentLocomotionAngle > 0.f)
	{
		return ELocomotionDirection::Right;
	}
	else
	{
		return ELocomotionDirection::Left;
	}
}

void ULyraAnimInstance::PivotOutputNode_OnBecomeRelevant(const FAnimUpdateContext& Context,
                                                         const FAnimNodeReference& Node)
{
	NormalizedPivotAcceleration2D = Acceleration2D.GetSafeNormal2D();
	TurnYawCurveValue = GetCurveValue("root_rotation_Z");
}

void ULyraAnimInstance::IdleOutputNode_OnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	RootYawOffsetMode = ERootYawOffsetMode::Accumulate;

	ProcessTurnYawCurve();
}

void ULyraAnimInstance::ProcessTurnYawCurve()
{
	LastFrameTurnYawCurveValue = TurnYawCurveValue;

	// 检测位于哪个阶段：转身阶段或者恢复阶段
	const float IsTurningCurveValue = GetCurveValue("IsTurning");
	// 根据我们做的“IsTurning”曲线，转身时的值为1，否则为0，中间可能有一些过渡值
	if (FMath::IsNearlyEqual(IsTurningCurveValue, 1.f, 0.01f))
	{
		TurnYawCurveValue = GetCurveValue("root_rotation_Z");

		// 我们的动画特点是所有转身完毕后，补偿角为0
		if (!FMath::IsNearlyZero(LastFrameTurnYawCurveValue))
		{
			const float Delta = TurnYawCurveValue - LastFrameTurnYawCurveValue;
			SetRootYawOffset(RootYawOffset - Delta);
		}
	}
	else
	{
		LastFrameTurnYawCurveValue = TurnYawCurveValue = 0.f;
	}
}
