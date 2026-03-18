// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ProjectPlayerController.generated.h"

class UChatWidget;
class UUserWidget;

UCLASS()
class U_260319_PROJECT009_API AProjectPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AProjectPlayerController();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	void SetChatMessage(const FString& ChatString);
	void PrintChatMessage(const FString& ChatString) const;
	
	UFUNCTION(Client, Reliable)
    void ClientRPCPrintChatMessage(const FString& ChatString, FColor ChatColor = FColor::White);
    
    UFUNCTION(Server, Reliable)
    void ServerRPCPrintChatMessage(const FString& ChatString);
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	FText NotifyTxt;
	UPROPERTY(Replicated, BlueprintReadOnly)
	FText NotifyTxtMore;
	UPROPERTY(Replicated, BlueprintReadOnly)
	FText NotifyTime;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UChatWidget> ChatWidget;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> NotifyWidget;

	UPROPERTY()
	TObjectPtr<UChatWidget> ChatWidgetInstance;
	UPROPERTY()
	TObjectPtr<UUserWidget> NotifyWidgetInstance;
	
	FString ChatStr;
};
