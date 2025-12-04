// 版权没有，拷贝自由。


#include "BaseAnimLayer.h"

#include "Animation/Lyra/LyraAnimInst.h"

ULyraAnimInst* UBaseAnimLayer::GetABPBase() const
{
	auto MainAnimInst = Blueprint_GetMainAnimInstance();
	return Cast<ULyraAnimInst>(MainAnimInst);
}

UAnimSequenceBase* UBaseAnimLayer::SelectAnimationBasedOnGaitAndLocomotionDirection(EGaitType InGait,
	ELocomotionDirection InLocomotionDirection, const TMap<EGaitType, FDirectionalAnimation>& InAnimationMap) const
{
	const FDirectionalAnimation* DirectionalAnim = InAnimationMap.Find(InGait);
	if (!DirectionalAnim) return nullptr;
	switch (InLocomotionDirection)
	{
	case ELocomotionDirection::Forward:
		return DirectionalAnim->Forward;
	case ELocomotionDirection::Backward:
		return DirectionalAnim->Backward;
	case ELocomotionDirection::Left:
		return DirectionalAnim->Left;
	case ELocomotionDirection::Right:
		return DirectionalAnim->Right;
	default:
		return nullptr;
	}
}
