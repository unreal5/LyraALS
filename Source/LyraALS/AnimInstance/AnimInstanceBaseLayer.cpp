// Fill out your copyright notice in the Description page of Project Settings.


#include "SequencePlayerLibrary.h"

#include "AnimInstance/AnimInstanceBaseLayer.h"

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
