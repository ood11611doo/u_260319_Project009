#include "ProjectGameState.h"
#include "Net/UnrealNetwork.h"

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