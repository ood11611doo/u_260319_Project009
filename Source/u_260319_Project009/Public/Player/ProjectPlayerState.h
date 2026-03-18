#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ProjectPlayerState.generated.h"

UENUM(BlueprintType)
enum class EPlayerStatus : uint8
{
	NotReady,
	Waiting,
	YourTurn
};

UCLASS()
class U_260319_PROJECT009_API AProjectPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AProjectPlayerState();

	// --- Variables ---

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player State")
	EPlayerStatus CurrentStatus = EPlayerStatus::NotReady;
	
	UPROPERTY(Replicated)
	FString PLName;
	
	UPROPERTY(Replicated)
	int32 TryCount;

	UPROPERTY(Replicated)
	int32 MaxCount;

	// --- Functions ---

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	FString GetPlayerTry();
};