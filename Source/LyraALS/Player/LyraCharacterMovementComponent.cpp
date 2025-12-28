// 版权没有，拷贝自由。


#include "LyraCharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"


// Sets default values for this component's properties
ULyraCharacterMovementComponent::ULyraCharacterMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

const FLyraCharacterGroundInfo& ULyraCharacterMovementComponent::GetGroundInfo()
{
	if (!CharacterOwner || (GFrameCounter == CachedGroundInfo.LastUpdateFrame))
	{
		return CachedGroundInfo;
	}


	const UCapsuleComponent* CapsuleComp = CharacterOwner->GetCapsuleComponent();
	check(CapsuleComp);

	const float CapsuleHalfHeight = CapsuleComp->GetScaledCapsuleHalfHeight();
	constexpr ECollisionChannel CollisionChannel = ECC_Visibility;
	FVector TraceStart(GetActorLocation());
	TraceStart.Z -= CapsuleHalfHeight;

	const FVector TraceEnd = TraceStart - FVector(0.f, 0.f, GroundTraceDistance);

	FCollisionQueryParams QueryParams(NAME_None, false, CharacterOwner);
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(QueryParams, ResponseParam);

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CollisionChannel, QueryParams, ResponseParam);

	CachedGroundInfo.GroundDistance = HitResult.bBlockingHit
		                                  ? FMath::Max((HitResult.Distance), 0.0f)
		                                  : 0.f;


	CachedGroundInfo.LastUpdateFrame = GFrameCounter;

	return CachedGroundInfo;
}
