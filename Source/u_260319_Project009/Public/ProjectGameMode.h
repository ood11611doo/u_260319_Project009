// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectGameMode.generated.h"

class AProjectPlayerController;

UCLASS()
class U_260319_PROJECT009_API AProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void OnPostLogin(AController* NewPlayer) override;	
	
	FString GenerateAnswer();
	FString CorResult(const FString& RealAnswer, const FString& InputAnswer);
	
	void ChatMessageStr(AProjectPlayerController* PLCont, const FString& InputStr);
	void TryCountInc(AProjectPlayerController* PLCont);
	bool CheckTryCount(AProjectPlayerController* PLCont);
	
	void ResetGame();
	void ResultGame(AProjectPlayerController* PLCont, int StrCount);
	
	void ChangeNotify(const FString& InputStr);
	void ChangeNotifyMore(const FString& InputStr);
	void ChangeNotifyTime(const FString& InputStr);
	
	
protected:
	FString AnswerStr;
	TArray<TObjectPtr<AProjectPlayerController>> AllPlayers;
	UPROPERTY()
	TSet<AProjectPlayerController*> ReadyPlayers;

	int32 AnswerGet;
	int32 MaxTryGet;
	int32 TurnTimeGet;
	bool bIsWaitingForRestart = false;
	
	FTimerHandle TurnTimerHandle;
	int32 RemainingTurnTime;

	void UpdateAllPlayerStatuses();
	
	void AdvanceTurn();
	void UpdateTurnUI();
	
	void UpdateRestartUI();
	
	void StartTurnTimer();
	void OnTurnTimeExpired();
	void UpdateTimerUI();
};
