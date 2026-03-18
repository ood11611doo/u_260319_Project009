// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectGameState.h"
#include "Kismet//GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/ProjectPlayerController.h"

void AProjectGameState::MRPCLoginBroadcast_Implementation(const FString& InNameString)
{
	if (HasAuthority() == false)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC) == true)
		{
			AProjectPlayerController* CXPC = Cast<AProjectPlayerController>(PC);
			if (IsValid(CXPC) == true)
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				CXPC->PrintChatMessage(NotificationString);
			}
		}
	}
}

ECheckType AProjectGameState::IsCorrect(const FString& InputStr)
{
	if (InputStr.Len() != AnswerLength) return ECheckType::Invalid;

	TSet<TCHAR> UniqueDigs;
	for (TCHAR C : InputStr)
	{
		if (!FChar::IsDigit(C) || C == '0') return ECheckType::Invalid;
		UniqueDigs.Add(C);
	}

	if (UniqueDigs.Num() != AnswerLength) return ECheckType::Invalid;

	return ECheckType::Normal;
}

void AProjectGameState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AProjectGameState, CurrentTurnIndex);
}
