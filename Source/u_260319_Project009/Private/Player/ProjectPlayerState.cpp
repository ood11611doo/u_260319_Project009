// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ProjectPlayerState.h"
#include "Net/UnrealNetwork.h"

AProjectPlayerState::AProjectPlayerState() : PLName(TEXT("BasePlayer")), TryCount(0), MaxCount(3)
{
	bReplicates = true;
}

void AProjectPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PLName);
	DOREPLIFETIME(ThisClass, TryCount);
	DOREPLIFETIME(ThisClass, MaxCount);
}

FString AProjectPlayerState::GetPlayerTry()
{
	if (TryCount >= MaxCount)
	{
		return TEXT("(DEAD)");
	}
    
	return FString::Printf(TEXT("(%d/%d)"), TryCount, MaxCount);
}

