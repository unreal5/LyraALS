// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/AnimInstance_Layer.h"

#include "AnimExecutionContextLibrary.h"
#include "AnimInstance_Main.h"
#include "SequencePlayerLibrary.h"

UAnimInstance_Main* UAnimInstance_Layer::GetABPBase() const
{
	return Cast<UAnimInstance_Main>(GetSkelMeshComponent()->GetAnimInstance());
}

void UAnimInstance_Layer::IdleOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequencePlayerReference sp;
	bool Result;
	USequencePlayerLibrary::ConvertToSequencePlayerPure(Node, sp, Result);
	if (Result)
	{
		USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, sp, IdleAnim);
	}
}

/* 如果只在BecomeRelevant中设置动画，则在Update时，如果切换状态，无法切换动画 */
/* 因此，需要在Update中，根据步态设置动画 */
/*
void UAnimInstance_Layer::CycleOnBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequencePlayerReference sp;
	bool Result;
	USequencePlayerLibrary::ConvertToSequencePlayerPure(Node, sp, Result);
	if (!Result) return;

	auto SelectedAnim = GetABPBase()->CurrentGait == EGait::Walking ? CycleWalkingAnim : CycleJoggingAnim;
	USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, sp, SelectedAnim);
	USequencePlayerLibrary::SetStartPosition(sp, 0.f);
}
*/
void UAnimInstance_Layer::CycleOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequencePlayerReference sp;
	bool Result;
	USequencePlayerLibrary::ConvertToSequencePlayerPure(Node, sp, Result);
	if (!Result) return;

	// 获取当前动画的时间
	float CurrentTime = USequencePlayerLibrary::GetAccumulatedTime(sp);
	// 加上当前帧的DeltaTime

	//UAnimExecutionContextLibrary::GetDeltaTime(Context);
	//CurrentTime += Context.GetContext()->GetDeltaTime(); 
	
	auto SelectedAnim = GetABPBase()->CurrentGait == EGait::Walking ? CycleWalkingAnim : CycleJoggingAnim;
	USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, sp, SelectedAnim);
	//USequencePlayerLibrary::SetAccumulatedTime(sp, CurrentTime);
	//USequencePlayerLibrary::SetStartPosition(sp, 0.f);
}
