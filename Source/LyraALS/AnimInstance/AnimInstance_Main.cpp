// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/AnimInstance_Main.h"

#include "KismetAnimationLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/KismetMathLibrary.h"

void UAnimInstance_Main::UpdateRootYawOffset(float DeltaTime)
{
	switch (RootYawOffsetMode)
	{
	case ERootYawOffsetMode::Accumulate:
		SetRootYawOffset(RootYawOffset - DeltaActorYaw);
		break;
	case ERootYawOffsetMode::BlendOut:
		{
			float SpringInterp = UKismetMathLibrary::FloatSpringInterp(RootYawOffset, 0.f, RootYawOffsetSpringState,
			                                                           80.f,
			                                                           1.f, DeltaTime, 1.f, 0.5f);
			SetRootYawOffset(SpringInterp);
		}
		break;
	default:
		break;
	}
	RootYawOffsetMode = ERootYawOffsetMode::BlendOut;
}

void UAnimInstance_Main::SetRootYawOffset(float Angle)
{
	RootYawOffset = UKismetMathLibrary::NormalizeAxis(Angle);
}

UCharacterMovementComponent* UAnimInstance_Main::GetCharacterMovementComponent() const
{
	if (auto Char = Cast<ACharacter>(GetOwningActor()))
	{
		return Char->GetCharacterMovement();
	}
	return nullptr;
}

void UAnimInstance_Main::GetVelocityData()
{
	auto CharMovementComp = GetCharacterMovementComponent();
	if (!CharMovementComp) return;

	CharacterVelocity = CharMovementComp->Velocity;
	CharacterVelocity2D = FVector(CharacterVelocity.X, CharacterVelocity.Y, 0.f);
	CharacterSpeed = CharacterVelocity.Size();
}

void UAnimInstance_Main::GetAccelerationData()
{
	auto CharMovementComp = GetCharacterMovementComponent();
	if (!CharMovementComp) return;

	Acceleration = CharMovementComp->GetCurrentAcceleration();
	Acceleration2D = FVector(Acceleration.X, Acceleration.Y, 0.f);
	IsAcceleration = !Acceleration2D.IsNearlyZero();
}

void UAnimInstance_Main::GetLocationData()
{
	auto OwningActor = GetOwningActor();
	if (!OwningActor) return;

	LastFrameWorldLocation = WorldLocation;;
	WorldLocation = OwningActor->GetActorLocation();
	DeltaLocation = (WorldLocation - LastFrameWorldLocation).Size2D();
}

void UAnimInstance_Main::GetRotationData(float DeltaTime)
{
	auto OwningActor = GetOwningActor();
	if (!OwningActor) return;

	WorldRotation = OwningActor->GetActorRotation();

	LastFrameActorYaw = ActorYaw;
	ActorYaw = WorldRotation.Yaw;
	DeltaActorYaw = ActorYaw - LastFrameActorYaw;
	LeanAngle = FMath::Clamp(UKismetMathLibrary::SafeDivide(DeltaActorYaw, DeltaTime) / 6.f, -90.f, 90.f);
	if (VelocityLocomotionDirection == ELocomotionDirection::Backward)
	{
		LeanAngle = -LeanAngle;
	}
}

void UAnimInstance_Main::UpdateOrientationData()
{
	LastFrameVelocityLocomotionDirection = VelocityLocomotionDirection;
	// 计算加速度与角色的夹角
	AccelerationLocomotionAngle = UKismetAnimationLibrary::CalculateDirection(Acceleration2D, WorldRotation);

	VelocityLocomotionangle = UKismetAnimationLibrary::CalculateDirection(CharacterVelocity2D, WorldRotation);
	// 由于插值的存在，在角色运动时，RootYawOffset并不立即为0，这里给补偿回去（先 - RootYawOffset, 然后在动画层中 + RootYawOffset。
	// 其实不计算也可以，直接在ABP_Layer中使用VelocityLocomotionangle + RootYawOffset。
	VelocityLocomotionAngleWithOffset = UKismetMathLibrary::NormalizeAxis(VelocityLocomotionangle - RootYawOffset);

	VelocityLocomotionDirection = CalculateLocomotionDirection(VelocityLocomotionDirection, VelocityLocomotionangle);

	AccelerationLocomotionDirection = CalculateLocomotionDirection(AccelerationLocomotionDirection,
	                                                               AccelerationLocomotionAngle);
}

ELocomotionDirection UAnimInstance_Main::CalculateLocomotionDirection(
	const ELocomotionDirection& CurrentLocomotionDirection, float CurrentLocomotionAngle, float BackwardThreshold,
	float ForwardThreshold, const float DeadZone)
{
	const float AbsAngle = FMath::Abs(CurrentLocomotionAngle);

	// 对于前后向，扩大阀值
	if (CurrentLocomotionDirection == ELocomotionDirection::Backward)
	{
		BackwardThreshold -= DeadZone;
	}
	else if (CurrentLocomotionDirection == ELocomotionDirection::Forward)
	{
		ForwardThreshold += DeadZone;
	}
	else if (CurrentLocomotionDirection == ELocomotionDirection::Right)
	{
		BackwardThreshold += DeadZone;
	}
	else
	{
		ForwardThreshold -= DeadZone;
	}

	if (AbsAngle >= BackwardThreshold)
	{
		return ELocomotionDirection::Backward;
	}
	if (AbsAngle <= ForwardThreshold)
	{
		return ELocomotionDirection::Forward;
	}
	return CurrentLocomotionAngle > 0.f ? ELocomotionDirection::Right : ELocomotionDirection::Left;
}

void UAnimInstance_Main::PivotOnBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	PivotAcceleration2D = Acceleration2D;
}

void UAnimInstance_Main::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}
