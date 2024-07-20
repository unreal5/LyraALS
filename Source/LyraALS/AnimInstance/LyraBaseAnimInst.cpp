// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/LyraBaseAnimInst.h"

#include "KismetAnimationLibrary.h"
#include "LyraALS.h"
#include "Animation/AnimNodeReference.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void ULyraBaseAnimInst::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	GetVelocityData();
	GetAccelerationData();
	GetLocationData();
	GetRotationData(DeltaSeconds);

	UpdateOrientation(DeltaSeconds);
	GetCharacterStates();
}

UCharacterMovementComponent* ULyraBaseAnimInst::GetCharacterMovementComponent() const
{
	ACharacter* Character = Cast<ACharacter>(GetOwningActor());
	return Character ? Character->GetCharacterMovement() : nullptr;
}

void ULyraBaseAnimInst::Pivot_BecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	PivotAcceleration2D = Acceleration2D;
	//UE_LOG(LogLyraALS,Log,  TEXT("Pivot动画结点相关"));
}

void ULyraBaseAnimInst::GetVelocityData()
{
	auto CharacterMovementComponent = GetCharacterMovementComponent();
	if (!CharacterMovementComponent) return;

	CharacterVelocity = CharacterMovementComponent->Velocity;
	CharacterVelocity2D = FVector(CharacterVelocity.X, CharacterVelocity.Y, 0.f);
	CharacterSpeed2D = CharacterVelocity2D.Size2D();
}

void ULyraBaseAnimInst::GetLocationData()
{
	AActor* OwningActor = GetOwningActor();
	if (!OwningActor) return;

	const FVector LastFrameWorldLocation = WorldLocation;
	WorldLocation = OwningActor->GetActorLocation();
	DeltaLocation = (WorldLocation - LastFrameWorldLocation).Size2D();
}

void ULyraBaseAnimInst::GetRotationData(float DeltaSeconds)
{
	AActor* OwningActor = GetOwningActor();
	if (!OwningActor) return;

	float LastFrameActorYaw = WorldRotation.Yaw;
	WorldRotation = OwningActor->GetActorRotation();
	float DeltaActorYaw = UKismetMathLibrary::SafeDivide(WorldRotation.Yaw - LastFrameActorYaw, DeltaSeconds);
	// 如果向后运动，则角度取反。
	if (VelocityLocomotionDirection == ELocomotionDirection::Backward)
	{
		DeltaActorYaw = -DeltaActorYaw;
	}
	LeanAngle = UKismetMathLibrary::ClampAngle(DeltaActorYaw, -90.f, 90.f);
}

void ULyraBaseAnimInst::GetAccelerationData()
{
	const auto CharacterMovementComponent = GetCharacterMovementComponent();
	if (!CharacterMovementComponent) return;

	Acceleration = CharacterMovementComponent->GetCurrentAcceleration();
	Acceleration2D = FVector(Acceleration.X, Acceleration.Y, 0.f);
	bIsAccelerating = Acceleration2D.SizeSquared2D() > 0.0001f;
}

void ULyraBaseAnimInst::UpdateOrientation(float DeltaTime)
{
	VelocityLocomotionAngle = UKismetAnimationLibrary::CalculateDirection(CharacterVelocity2D, WorldRotation);
	// 计算加速度方向
	AccelerationLocomotionAngle = UKismetAnimationLibrary::CalculateDirection(Acceleration2D, WorldRotation);
	AccelerationLocomotionDirection = CalculateLocomotionDirection(AccelerationLocomotionAngle,
	                                                               AccelerationLocomotionDirection);

	LastFrameLocomotionDirection = VelocityLocomotionDirection;
	VelocityLocomotionDirection = CalculateLocomotionDirection(VelocityLocomotionAngle, VelocityLocomotionDirection);
}

ELocomotionDirection ULyraBaseAnimInst::CalculateLocomotionDirection(float CurrentLocomotionAngle,
                                                                     ELocomotionDirection CurrentLocomotionDirection,
                                                                     float ForwardMin, float ForwardMax,
                                                                     float BackwardMin,
                                                                     float BackwardMax, float DeadZone)
{
	if (CurrentLocomotionDirection == ELocomotionDirection::Forward)
	{
		ForwardMin -= DeadZone;
		ForwardMin += DeadZone;
	}
	if (CurrentLocomotionDirection == ELocomotionDirection::Backward)
	{
		BackwardMin += DeadZone;
		BackwardMax -= DeadZone;
	}

	if (CurrentLocomotionAngle < BackwardMin || CurrentLocomotionAngle > BackwardMax)
	{
		return ELocomotionDirection::Backward;
	}

	if (CurrentLocomotionAngle > ForwardMin && CurrentLocomotionAngle < ForwardMax)
	{
		return ELocomotionDirection::Forward;
	}

	return CurrentLocomotionAngle > 0.f ? ELocomotionDirection::Right : ELocomotionDirection::Left;
}

void ULyraBaseAnimInst::GetCharacterStates()
{
	// 以这种奇怪的方式实现的原因在于，Gait是接口调用，被动传递的，它没有当前帧和上一帧的概念，
	// 而别的变量是主动在NativeThreadSafeUpdateAnimation中获取的。
	LastFrameGait = CurrentGait;
	CurrentGait = InComingGait;
	bIsGaitChanged = LastFrameGait != CurrentGait;
}
