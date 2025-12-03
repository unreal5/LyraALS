// 版权没有，拷贝自由。


#include "LyraAnimInst.h"

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
}

void FLyraAnimInstProxy::Update(float DeltaSeconds)
{
	FAnimInstanceProxy::Update(DeltaSeconds);
}


// 动画实例
FAnimInstanceProxy* ULyraAnimInst::CreateAnimInstanceProxy()
{
	return &LyraAnimInstProxy;
}

void ULyraAnimInst::DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy)
{
	// Nothing to clean up since we are using a struct
}

// 动画评估后调用，运行于游戏线程
void ULyraAnimInst::NativePostEvaluateAnimation()
{
	Super::NativePostEvaluateAnimation();
}
