#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectGameMode.generated.h"

class AProjectPlayerController;
class AProjectGameState;

UCLASS()
class U_260319_PROJECT009_API AProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void OnPostLogin(AController* NewPlayer) override;	
	
	// --- Functions ---
	
	FString GenerateAnswer();
	FString CorResult(const FString& RealAnswer, const FString& InputAnswer);

	void ChatMessageStr(AProjectPlayerController* PLCont, const FString& InputStr);
	void TryCountInc(AProjectPlayerController* PLCont);
	void ResetGame();
	void ResultGame(AProjectPlayerController* PLCont, int StrCount);
	void UpdateAllPlayerStatuses();
	void AdvanceTurn();
	void UpdateRestartUI();
	void ChangeNotify(const FString& InputStr);
	void ChangeNotifyMore(const FString& InputStr);
	void ChangeNotifyTime(const FString& InputStr);

protected:
	// --- Variables ---
	
	FString AnswerStr;
	bool bIsWaitingForRestart = false;
	
	FTimerHandle TurnTimerHandle;
	int32 RemainingTurnTime;

	UPROPERTY()
	TArray<TObjectPtr<AProjectPlayerController>> AllPlayers;
	UPROPERTY()
	TSet<TObjectPtr<AProjectPlayerController>> ReadyPlayers;

	// --- Functions ---

	void StartTurnTimer();
	void OnTurnTimeExpired();
	void UpdateTimerUI();
};