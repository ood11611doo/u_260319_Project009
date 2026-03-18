// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectGameMode.h"
#include "ProjectGameState.h"
#include "Player/ProjectPlayerController.h"
#include "Player/ProjectPlayerState.h"

void AProjectGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	AProjectGameState* PGS = GetGameState<AProjectGameState>();
	if (IsValid(PGS))
	{
		AnswerGet = PGS->AnswerLength;
		MaxTryGet = PGS->MaxTryCount;
		TurnTimeGet = PGS->TurnTime;
	}
	
	AnswerStr = GenerateAnswer();
}

void AProjectGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
    
	AProjectPlayerController* PLCont = Cast<AProjectPlayerController>(NewPlayer);
	if (IsValid(PLCont))
	{
		AllPlayers.Add(PLCont);
		AProjectPlayerState* PPS = PLCont->GetPlayerState<AProjectPlayerState>();
		if (IsValid(PPS))
		{
			PPS->PLName = FString::Printf(TEXT("Player %d"), AllPlayers.Num());
			PPS->MaxCount = MaxTryGet;
		}

		UpdateAllPlayerStatuses();
	}
}

FString AProjectGameMode::GenerateAnswer()
{
	TArray<int32> Nums;
	for (int32 i = 1; i <= 9; ++i)
	{
		Nums.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Nums = Nums.FilterByPredicate([](int32 Num) { return Num > 0; });
	
	FString Returns;
	for (int32 i = 0; i < AnswerGet; ++i)
	{
		int32 Index = FMath::RandRange(0, Nums.Num() - 1);
		Returns.Append(FString::FromInt(Nums[Index]));
		Nums.RemoveAt(Index);
	}

	UE_LOG(LogTemp, Warning, TEXT("%s"), *Returns);
	return Returns;
}

FString AProjectGameMode::CorResult(const FString& RealAnswer, const FString& InputAnswer)
{
	int32 CorCount = 0;
	int32 NumCount = 0;

	for (int32 i = 0; i < AnswerGet; ++i)
	{
		if (RealAnswer[i] == InputAnswer[i])
		{
			CorCount++;
		}
		else 
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InputAnswer[i]);
			if (RealAnswer.Contains(PlayerGuessChar))
			{
				NumCount++;				
			}
		}
	}

	if (CorCount == 0 && NumCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), CorCount, NumCount);
}

void AProjectGameMode::ChatMessageStr(AProjectPlayerController* PLCont, const FString& InputStr)
{
	AProjectGameState* PGS = GetGameState<AProjectGameState>();
	if (!IsValid(PGS)) return;
	
	FString ActualContent;
	int32 ColonIndex;
	if (InputStr.FindChar(':', ColonIndex))
	{
		ActualContent = InputStr.RightChop(ColonIndex + 2).TrimStartAndEnd();
	}
	else
	{
		ActualContent = InputStr.TrimStartAndEnd();
	}
	
	if (bIsWaitingForRestart)
	{
		if (ActualContent.Equals(TEXT("y"), ESearchCase::IgnoreCase))
		{
			ReadyPlayers.Add(PLCont);
			UpdateRestartUI();

			if (ReadyPlayers.Num() >= AllPlayers.Num())
			{
				bIsWaitingForRestart = false;
				ReadyPlayers.Empty();
				ChangeNotify(TEXT("Game Restarted!"));
				ResetGame();
			}
			return; 
		}
	}
	
	AProjectPlayerState* CurrentPS = PLCont->GetPlayerState<AProjectPlayerState>();
	if (!CurrentPS) return;

	if (CurrentPS->CurrentStatus == EPlayerStatus::YourTurn && PGS->IsCorrect(ActualContent) == ECheckType::Normal)
	{
		GetWorldTimerManager().ClearTimer(TurnTimerHandle);
		
		FString ResultStr = CorResult(AnswerStr, ActualContent);
		TryCountInc(PLCont);
        
		FString SpecialInfo = FString::Printf(TEXT("%s: %s [%s]"), *CurrentPS->PLName, *ActualContent, *ResultStr);
		ChangeNotifyMore(SpecialInfo);

		FString FinalStr = InputStr + TEXT(" -> ") + ResultStr;

		for (AProjectPlayerController* PC : AllPlayers)
		{
			PC->ClientRPCPrintChatMessage(FinalStr, FColor::Cyan);
		}

		int32 StrikeCount = FCString::Atoi(*ResultStr.Left(1));
		if (StrikeCount == AnswerGet) 
		{
			ResultGame(PLCont, StrikeCount);
		}
		else 
		{
			AdvanceTurn();
		}
	}
	else
	{
		for (AProjectPlayerController* PC : AllPlayers)
		{
			PC->ClientRPCPrintChatMessage(InputStr, FColor::White);
		}
	}
}

void AProjectGameMode::TryCountInc(AProjectPlayerController* PLCont)
{
	AProjectPlayerState* PLState = PLCont->GetPlayerState<AProjectPlayerState>();
	if (IsValid(PLState))
	{
		PLState->TryCount++;
	}
}

bool AProjectGameMode::CheckTryCount(AProjectPlayerController* PLCont)
{
	bool bIsOver = false;
	
	AProjectPlayerState* PLState = PLCont->GetPlayerState<AProjectPlayerState>();
	if (IsValid(PLState))
	{
		if (PLState->TryCount >= MaxTryGet)
		{
			bIsOver = true;
		}
	}
	
	return bIsOver;
}

void AProjectGameMode::ResetGame()
{
	AnswerStr = GenerateAnswer();
	bIsWaitingForRestart = false;
	ReadyPlayers.Empty();

	for (auto& PLConts : AllPlayers)
	{
		if (AProjectPlayerState* PPS = PLConts->GetPlayerState<AProjectPlayerState>())
		{
			PPS->TryCount = 0;
		}
	}

	ChangeNotifyMore(TEXT(""));
	UpdateAllPlayerStatuses();
	StartTurnTimer();
}

void AProjectGameMode::ResultGame(AProjectPlayerController* PLCont, int StrCount)
{
	bool bGameOver = false;
	FString GameOverMsg;

	if (StrCount == AnswerGet && PLCont)
	{
		AProjectPlayerState* PS = PLCont->GetPlayerState<AProjectPlayerState>();
		GameOverMsg = PS ? PS->PLName + TEXT(" is Winner!") : TEXT("Game Over!");
		bGameOver = true;
	}
	else
	{
		GameOverMsg = TEXT("No one won the game (All players DEAD).");
		bGameOver = true;
	}

	if (bGameOver)
	{
		bIsWaitingForRestart = true;
		ReadyPlayers.Empty();
		ChangeNotify(GameOverMsg);
		ChangeNotifyTime(TEXT(""));
        
		UpdateRestartUI(); 
	}
}

void AProjectGameMode::ChangeNotify(const FString& InputStr)
{
	for (const auto& PLConts : AllPlayers)
	{
		PLConts->NotifyTxt = FText::FromString(InputStr);
	}
}

void AProjectGameMode::ChangeNotifyMore(const FString& InputStr)
{
	for (const auto& PLConts : AllPlayers)
	{
		PLConts->NotifyTxtMore = FText::FromString(InputStr);
	}
}

void AProjectGameMode::ChangeNotifyTime(const FString& InputStr)
{
	for (const auto& PLConts : AllPlayers)
	{
		PLConts->NotifyTime = FText::FromString(InputStr);
	}
}

void AProjectGameMode::UpdateAllPlayerStatuses()
{
	AProjectGameState* PGS = GetGameState<AProjectGameState>();
	if (!PGS) return;

	for (int32 i = 0; i < AllPlayers.Num(); ++i)
	{
		AProjectPlayerState* PS = AllPlayers[i]->GetPlayerState<AProjectPlayerState>();
		if (PS)
		{
			if (bIsWaitingForRestart)
			{
				PS->CurrentStatus = EPlayerStatus::NotReady;
			}
			else if (i == PGS->CurrentTurnIndex)
			{
				PS->CurrentStatus = EPlayerStatus::YourTurn;
				ChangeNotify(FString::Printf(TEXT("Current Turn: %s"), *PS->PLName));
			}
			else
			{
				PS->CurrentStatus = EPlayerStatus::Waiting;
			}
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
		if (!CheckTryCount(AllPlayers[NextIndex]))
		{
			PGS->CurrentTurnIndex = NextIndex;
			bFoundValidPlayer = true;
			break;
		}
	}

	if (!bFoundValidPlayer)
	{
		ResultGame(nullptr, -1); 
	}
	else
	{
		UpdateAllPlayerStatuses();
		StartTurnTimer();
	}
}

void AProjectGameMode::UpdateTurnUI()
{
	AProjectGameState* PGS = GetGameState<AProjectGameState>();
	if (PGS && AllPlayers.IsValidIndex(PGS->CurrentTurnIndex))
	{
		AProjectPlayerController* CurrentPC = AllPlayers[PGS->CurrentTurnIndex];
		if (IsValid(CurrentPC))
		{
			AProjectPlayerState* PS = CurrentPC->GetPlayerState<AProjectPlayerState>();
			FString TurnMsg = FString::Printf(TEXT("Current Turn: %s"), PS ? *PS->PLName : TEXT("Unknown"));
			ChangeNotify(TurnMsg);
		}
	}
}

void AProjectGameMode::UpdateRestartUI()
{
	FString Status = FString::Printf(TEXT("Type 'y' to restart! (%d/%d)"), ReadyPlayers.Num(), AllPlayers.Num());
	ChangeNotifyMore(Status);
}

void AProjectGameMode::StartTurnTimer()
{
	RemainingTurnTime = TurnTimeGet; 
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
	for (auto& PC : AllPlayers)
	{
		if (IsValid(PC)) PC->NotifyTime = FText::FromString(TimeStr);
	}

	RemainingTurnTime--;
}

void AProjectGameMode::OnTurnTimeExpired()
{
	GetWorldTimerManager().ClearTimer(TurnTimerHandle);

	AProjectGameState* PGS = GetGameState<AProjectGameState>();
	if (PGS && AllPlayers.IsValidIndex(PGS->CurrentTurnIndex))
	{
		AProjectPlayerController* CurrentPC = AllPlayers[PGS->CurrentTurnIndex];
		if (IsValid(CurrentPC))
		{
			TryCountInc(CurrentPC);
			CurrentPC->ClientRPCPrintChatMessage(TEXT("Turn timed out! Try count increased."), FColor::Red);
		}
	}

	AdvanceTurn();
}