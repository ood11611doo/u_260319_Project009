#include "ProjectGameMode.h"
#include "ProjectGameState.h"
#include "Player/ProjectPlayerController.h"
#include "Player/ProjectPlayerState.h"
#include "TimerManager.h"

void AProjectGameMode::BeginPlay()
{
	Super::BeginPlay();
	AnswerStr = GenerateAnswer();
}

void AProjectGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
    
	if (AProjectPlayerController* PLCont = Cast<AProjectPlayerController>(NewPlayer))
	{
		AllPlayers.Add(PLCont);

		if (AProjectPlayerState* PPS = PLCont->GetPlayerState<AProjectPlayerState>())
		{
			PPS->PLName = FString::Printf(TEXT("Player %d"), AllPlayers.Num());
			if (AProjectGameState* PGS = GetGameState<AProjectGameState>())
			{
				PPS->MaxCount = PGS->MaxTryCount;
			}
		}
		UpdateAllPlayerStatuses();
	}
}

FString AProjectGameMode::GenerateAnswer()
{
	AProjectGameState* PGS = GetGameState<AProjectGameState>();
	int32 Length = PGS ? PGS->AnswerLength : 3;

	TArray<int32> Nums;
	for (int32 i = 1; i <= 9; ++i) Nums.Add(i);

	FMath::RandInit(FDateTime::Now().GetTicks());
	
	FString Returns;
	for (int32 i = 0; i < Length; ++i)
	{
		int32 Index = FMath::RandRange(0, Nums.Num() - 1);
		Returns.Append(FString::FromInt(Nums[Index]));
		Nums.RemoveAt(Index);
	}

	UE_LOG(LogTemp, Warning, TEXT("Answer: %s"), *Returns);
	return Returns;
}

FString AProjectGameMode::CorResult(const FString& RealAnswer, const FString& InputAnswer)
{
	int32 CorCount = 0;
	int32 NumCount = 0;
	int32 Length = RealAnswer.Len();

	for (int32 i = 0; i < Length; ++i)
	{
		if (RealAnswer[i] == InputAnswer[i])
		{
			CorCount++;
		}
		else if (RealAnswer.Contains(FString::Printf(TEXT("%c"), InputAnswer[i])))
		{
			NumCount++;				
		}
	}

	return (CorCount == 0 && NumCount == 0) ? TEXT("OUT") : FString::Printf(TEXT("%dS%dB"), CorCount, NumCount);
}

void AProjectGameMode::ChatMessageStr(AProjectPlayerController* PLCont, const FString& InputStr)
{
	AProjectGameState* PGS = GetGameState<AProjectGameState>();
	if (!PGS || !PLCont) return;
	
	// Parse message content
	int32 ColonIndex;
	FString ActualContent = InputStr.FindChar(':', ColonIndex) ? InputStr.RightChop(ColonIndex + 2).TrimStartAndEnd() : InputStr.TrimStartAndEnd();
	
	if (bIsWaitingForRestart)
	{
		if (ActualContent.Equals(TEXT("y"), ESearchCase::IgnoreCase))
		{
			ReadyPlayers.Add(PLCont);
			UpdateRestartUI();

			if (ReadyPlayers.Num() >= AllPlayers.Num())
			{
				ResetGame();
			}
		}
		return; 
	}
	
	AProjectPlayerState* CurrentPS = PLCont->GetPlayerState<AProjectPlayerState>();
	if (CurrentPS && CurrentPS->CurrentStatus == EPlayerStatus::YourTurn && PGS->IsCorrect(ActualContent) == ECheckType::Normal)
	{
		GetWorldTimerManager().ClearTimer(TurnTimerHandle);
		
		FString ResultStr = CorResult(AnswerStr, ActualContent);
		TryCountInc(PLCont);
        
		ChangeNotifyMore(FString::Printf(TEXT("%s: %s [%s]"), *CurrentPS->PLName, *ActualContent, *ResultStr));

		FString FinalStr = InputStr + TEXT(" -> ") + ResultStr;
		for (auto& PC : AllPlayers) PC->ClientRPCPrintChatMessage(FinalStr, FColor::Cyan);

		if (FCString::Atoi(*ResultStr.Left(1)) == PGS->AnswerLength) 
		{
			ResultGame(PLCont, PGS->AnswerLength);
		}
		else 
		{
			AdvanceTurn();
		}
	}
	else
	{
		for (auto& PC : AllPlayers) PC->ClientRPCPrintChatMessage(InputStr, FColor::White);
	}
}

void AProjectGameMode::TryCountInc(AProjectPlayerController* PLCont)
{
	if (AProjectPlayerState* PLState = PLCont ? PLCont->GetPlayerState<AProjectPlayerState>() : nullptr)
	{
		PLState->TryCount++;
	}
}

void AProjectGameMode::ResetGame()
{
	AnswerStr = GenerateAnswer();
	bIsWaitingForRestart = false;
	ReadyPlayers.Empty();

	for (auto& PC : AllPlayers)
	{
		if (AProjectPlayerState* PPS = PC->GetPlayerState<AProjectPlayerState>()) PPS->TryCount = 0;
	}

	ChangeNotify(TEXT("Game Restarted!"));
	ChangeNotifyMore(TEXT(""));
	UpdateAllPlayerStatuses();
	StartTurnTimer();
}

void AProjectGameMode::ResultGame(AProjectPlayerController* PLCont, int StrCount)
{
	AProjectGameState* PGS = GetGameState<AProjectGameState>();
	if (!PGS) return;
	
	FString GameOverMsg = (PLCont && StrCount == (PGS ? PGS->AnswerLength : 3)) 
		? FString::Printf(TEXT("%s is Winner!"), *PLCont->GetPlayerState<AProjectPlayerState>()->PLName) 
		: TEXT("No one won the game (All players DEAD).");
	PGS->CurrentTurnIndex = 0;

	bIsWaitingForRestart = true;
	ReadyPlayers.Empty();
	GetWorldTimerManager().ClearTimer(TurnTimerHandle);

	ChangeNotify(GameOverMsg);
	ChangeNotifyTime(TEXT(""));
	UpdateRestartUI();
	UpdateAllPlayerStatuses();
}

void AProjectGameMode::UpdateAllPlayerStatuses()
{
	AProjectGameState* PGS = GetGameState<AProjectGameState>();
	if (!PGS) return;

	for (int32 i = 0; i < AllPlayers.Num(); ++i)
	{
		if (AProjectPlayerState* PS = AllPlayers[i]->GetPlayerState<AProjectPlayerState>())
		{
			if (bIsWaitingForRestart) PS->CurrentStatus = EPlayerStatus::NotReady;
			else if (i == PGS->CurrentTurnIndex)
			{
				PS->CurrentStatus = EPlayerStatus::YourTurn;
				ChangeNotify(FString::Printf(TEXT("Current Turn: %s"), *PS->PLName));
			}
			else PS->CurrentStatus = EPlayerStatus::Waiting;
		}
	}
}

void AProjectGameMode::AdvanceTurn()
{
	AProjectGameState* PGS = GetGameState<AProjectGameState>();
	if (!PGS || AllPlayers.Num() == 0) return;

	int32 StartingIndex = PGS->CurrentTurnIndex;
	bool bFoundValidPlayer = false;

	for (int32 i = 1; i <= AllPlayers.Num(); ++i)
	{
		int32 NextIndex = (StartingIndex + i) % AllPlayers.Num();
		AProjectPlayerState* PS = AllPlayers[NextIndex]->GetPlayerState<AProjectPlayerState>();
		if (PS && PS->TryCount < PGS->MaxTryCount)
		{
			PGS->CurrentTurnIndex = NextIndex;
			bFoundValidPlayer = true;
			break;
		}
	}

	if (!bFoundValidPlayer) ResultGame(nullptr, -1); 
	else
	{
		UpdateAllPlayerStatuses();
		StartTurnTimer();
	}
}

void AProjectGameMode::UpdateRestartUI()
{
	ChangeNotifyMore(FString::Printf(TEXT("Type 'y' to restart! (%d/%d)"), ReadyPlayers.Num(), AllPlayers.Num()));
}

void AProjectGameMode::StartTurnTimer()
{
	AProjectGameState* PGS = GetGameState<AProjectGameState>();
	RemainingTurnTime = PGS ? PGS->TurnTime : 30; 
	UpdateTimerUI();
	GetWorldTimerManager().SetTimer(TurnTimerHandle, this, &AProjectGameMode::UpdateTimerUI, 1.0f, true);
}

void AProjectGameMode::UpdateTimerUI()
{
	if (RemainingTurnTime <= 0)
	{
		OnTurnTimeExpired();
		return;
	}

	FString TimeStr = FString::Printf(TEXT("Time Left: %ds"), RemainingTurnTime);
	ChangeNotifyTime(TimeStr);
	RemainingTurnTime--;
}

void AProjectGameMode::OnTurnTimeExpired()
{
	GetWorldTimerManager().ClearTimer(TurnTimerHandle);
	AProjectGameState* PGS = GetGameState<AProjectGameState>();
	if (PGS && AllPlayers.IsValidIndex(PGS->CurrentTurnIndex))
	{
		TryCountInc(AllPlayers[PGS->CurrentTurnIndex]);
		AllPlayers[PGS->CurrentTurnIndex]->ClientRPCPrintChatMessage(TEXT("Turn timed out! Try count increased."), FColor::Red);
	}
	AdvanceTurn();
}

void AProjectGameMode::ChangeNotify(const FString& InputStr) { for (auto& PC : AllPlayers) PC->NotifyTxt = FText::FromString(InputStr); }
void AProjectGameMode::ChangeNotifyMore(const FString& InputStr) { for (auto& PC : AllPlayers) PC->NotifyTxtMore = FText::FromString(InputStr); }
void AProjectGameMode::ChangeNotifyTime(const FString& InputStr) { for (auto& PC : AllPlayers) PC->NotifyTime = FText::FromString(InputStr); }