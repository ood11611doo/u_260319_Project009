// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ProjectGameState.generated.h"


enum ECheckType : uint8
{
	Normal = 0,
	Invalid = 1
};

UCLASS()
class U_260319_PROJECT009_API AProjectGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	ECheckType IsCorrect(const FString& InputStr);
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION(NetMulticast, Reliable)
	void MRPCLoginBroadcast(const FString& InNameString = FString(TEXT("Empty Name")));
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 CurrentTurnIndex = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AnswerLength = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxTryCount = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TurnTime = 30;
	
	FString GetCurrentPlayerName();
};
