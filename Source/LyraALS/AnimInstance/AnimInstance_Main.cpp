// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/AnimInstance_Main.h"

#include "KismetAnimationLibrary.h"
#include "Animation/AnimInstanceProxy.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

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

	// UpdateDistanceToGround 依赖 WorldLocation
	UpdateDistanceToGround();
}

void UAnimInstance_Main::GetRotationData(float DeltaTime)
{
	auto OwningActor = Cast<APawn>(GetOwningActor());
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
	AimPitch = UKismetMathLibrary::NormalizeAxis(OwningActor->GetBaseAimRotation().Pitch);
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

void UAnimInstance_Main::OnUpdateIdleState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	RootYawOffsetMode = ERootYawOffsetMode::Accumulate;
	ProcessTurnYawCurve();
}

void UAnimInstance_Main::ProcessTurnYawCurve()
{
	LastFrameTurnYawCurveValue = TurnYawCurveValue;
	float IsTurningValue = GetCurveValue("IsTurning");
	if (IsTurningValue < 1.f) // recovery phase
	{
		TurnYawCurveValue = LastFrameTurnYawCurveValue = 0.f;
	}
	else
	{
		TurnYawCurveValue = UKismetMathLibrary::SafeDivide(GetCurveValue("root_rotation_Z"), IsTurningValue);
		if (!FMath::IsNearlyZero(LastFrameTurnYawCurveValue))
		{
			const float Delta = TurnYawCurveValue - LastFrameTurnYawCurveValue;
			SetRootYawOffset(RootYawOffset - Delta);
		}
	}
}


void UAnimInstance_Main::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	auto NativeThreadId = FPlatformTLS::GetCurrentThreadId();
	// 如果判定游戏正在运行？
	if (GWorld && GWorld->HasBegunPlay() && !UGameplayStatics::IsGamePaused(GWorld))
	{
		auto&& Proxy = GetProxyOnAnyThread<FAnimInstanceProxy>();
		//if (GGameThreadId == NativeThreadId)
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("主线程调用Native thread safe update animation"));
		//}
		//UE_LOG(LogTemp, Warning, TEXT("游戏线程ID: %d, 当前线程ID: %d, Proxy Id = %s"), GGameThreadId, NativeThreadId, *Proxy.GetAnimInstanceName());
	}
}

void UAnimInstance_Main::UpdateDistanceToGround()
{
	UWorld* World = GetWorld();
	if (!World) return;
	auto Char = Cast<ACharacter>(GetOwningActor());
	if (!Char) return;
	const float CapsuleHalfHeight = Char->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	// 脚部
	FVector Start = WorldLocation - FVector{0.f, 0.f, CapsuleHalfHeight};
	// 向下
	FVector End = FVector{Start.X, Start.Y, Start.Z - 1000.f};
	float Radius = 5.f;
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	// 此时位于线程中，不允许使用主线程绘制，因此EDrawDebugTrace::None
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwningActor());
	/*
	UKismetSystemLibrary::SphereTraceSingle(this, Start, End,
	                                        Radius, UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
	                                        ActorsToIgnore,
	                                        EDrawDebugTrace::None, HitResult, true);*/

	World->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, ECC_Visibility,
	                            FCollisionShape::MakeSphere(Radius), Params);
	DistanceToGround = HitResult.bBlockingHit ? HitResult.Distance : 0.f;
}
