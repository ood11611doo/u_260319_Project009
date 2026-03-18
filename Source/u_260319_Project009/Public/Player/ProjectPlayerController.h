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

	// --- Variables ---

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Notifications")
	FText NotifyTxt;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Notifications")
	FText NotifyTxtMore;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Notifications")
	FText NotifyTime;

	// --- Functions ---

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	void SetChatMessage(const FString& ChatString);
	
	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessage(const FString& ChatString);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessage(const FString& ChatString, FColor ChatColor = FColor::White);
    
protected:
	// --- UI ---

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UChatWidget> ChatWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> NotifyWidgetClass;

	UPROPERTY()
	TObjectPtr<UChatWidget> ChatWidgetInstance;

	UPROPERTY()
	TObjectPtr<UUserWidget> NotifyWidgetInstance;
};