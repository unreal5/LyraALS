// Lyra高级运动系统


#include "AnimLayer/AnimInst_BaseLayer.h"

#include "SequencePlayerLibrary.h"
#include "Animation/LyraAnimInstance.h"
#include "Character/LyraCharacter.h"


ALyraCharacter* UAnimInst_BaseLayer::GetLyraCharacter()
{
	if (auto LyraCharacter = Cast<ALyraCharacter>(GetOwningActor()))
	{
		return LyraCharacter;
	}
	return nullptr;
}

ULyraAnimInstance* UAnimInst_BaseLayer::GetABPBase()
{
	if (auto OwningComp = GetOwningComponent())
	{
		return Cast<ULyraAnimInstance>(OwningComp->GetAnimInstance());
	}
	return nullptr;
}

void UAnimInst_BaseLayer::IdleOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequencePlayerReference SequencePlayer;
	bool Result;
	USequencePlayerLibrary::ConvertToSequencePlayerPure(Node, SequencePlayer, Result);
	if (Result)
	{
		USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, SequencePlayer, IdleAnim, 0.2f);
	}
}

void UAnimInst_BaseLayer::CycleOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequencePlayerReference SequencePlayer;
	bool Result;
	USequencePlayerLibrary::ConvertToSequencePlayerPure(Node, SequencePlayer, Result);
	if (!Result) return;
	if (auto LyraAnimInst = GetABPBase())
	{
		auto Gait = LyraAnimInst->CurrentGait;
		auto Direction = LyraAnimInst->LocomotionDirection;
		UAnimSequenceBase* Anim = SelectAnimBy(WalkCycleAnims, JoggingCycleAnims, Gait, Direction);
		USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, SequencePlayer, Anim, 0.2f);
	}
}

UAnimSequenceBase* UAnimInst_BaseLayer::SelectAnimBy(const TArray<TObjectPtr<UAnimSequenceBase>>& InWalkAnims,
	const TArray<TObjectPtr<UAnimSequenceBase>>& InJoggingAnims, EGait Gait, ELocomotionDirection Direction)
{
	auto AnimSet = Gait == EGait::Walking ? InWalkAnims : InJoggingAnims;
	if (AnimSet.IsValidIndex(static_cast<int32>(Direction)))
	{
		return AnimSet[static_cast<int32>(Direction)];
	}
	checkf(0, TEXT("动画集合中没有对应的动画"));
	return nullptr;
}