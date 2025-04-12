// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/AnimInstanceBaseLayer.h"

#include "LyraAnimInstance.h"
#include "SequencePlayerLibrary.h"

ULyraAnimInstance* UAnimInstanceBaseLayer::GetMainAnimInstance() const
{
	return Cast<ULyraAnimInstance>(GetSkelMeshComponent()->GetAnimInstance());
}

void UAnimInstanceBaseLayer::IdleOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequencePlayerReference sp;
	bool Result;
	USequencePlayerLibrary::ConvertToSequencePlayerPure(Node, sp, Result);
	if (Result)
	{
		USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, sp, IdleAnim);
	}
}
