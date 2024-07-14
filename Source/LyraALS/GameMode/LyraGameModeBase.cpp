// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LyraGameModeBase.h"

ALyraGameModeBase::ALyraGameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ALS/Character/BP_LyraCharacter"));
	if (PlayerPawnBPClass.Succeeded())
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
