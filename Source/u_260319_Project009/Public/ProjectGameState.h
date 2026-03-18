#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ProjectGameState.generated.h"

UENUM(BlueprintType)
enum class ECheckType : uint8
{
	Normal,
	Invalid
};

UCLASS()
class U_260319_PROJECT009_API AProjectGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	// --- Variables ---

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game State")
	int32 CurrentTurnIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rules")
	int32 AnswerLength = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rules")
	int32 MaxTryCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rules")
	int32 TurnTime = 30;

	// --- Functions ---

	ECheckType IsCorrect(const FString& InputStr);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};