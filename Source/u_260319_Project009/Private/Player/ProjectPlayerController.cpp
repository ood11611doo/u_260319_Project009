// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ProjectPlayerController.h"
#include "Player/ProjectPlayerState.h"
#include "u_260319_Project009.h"
#include "UI/ChatWidget.h"
#include "ProjectGameMode.h"
#include  "ProjectGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AProjectPlayerController::AProjectPlayerController()
{
	bReplicates = true;
}

void AProjectPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController()) return;
	
	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);
	
	if (ChatWidget)
	{
		ChatWidgetInstance = CreateWidget<UChatWidget>(this, ChatWidget);
		if (ChatWidgetInstance)
		{
			ChatWidgetInstance->AddToViewport();
		}
	}
	
	if (IsValid(NotifyWidget) == true)
	{
		NotifyWidgetInstance = CreateWidget<UUserWidget>(this, NotifyWidget);
		if (IsValid(NotifyWidgetInstance) == true)
		{
			NotifyWidgetInstance->AddToViewport();
		}
	}
}

void AProjectPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotifyTxt);
	DOREPLIFETIME(ThisClass, NotifyTxtMore);
	DOREPLIFETIME(ThisClass, NotifyTime);
}

void AProjectPlayerController::SetChatMessage(const FString& ChatString)
{
	if (!IsLocalController()) return;

	AProjectPlayerState* PS = GetPlayerState<AProjectPlayerState>();
	AProjectGameState* PGS = GetWorld()->GetGameState<AProjectGameState>();
	if (!PS || !PGS) return;

	FString TryDisplay = PS->GetPlayerTry(); 
	FString FormattedMsg = FString::Printf(TEXT("%s%s: %s"), *PS->PLName, *TryDisplay, *ChatString);

	if (PS->CurrentStatus == EPlayerStatus::YourTurn)
	{
		if (PGS->IsCorrect(ChatString) == ECheckType::Normal)
		{
			ServerRPCPrintChatMessage(FormattedMsg);
		}
		else
		{
			FunctionLib::MyPrintString(this, TEXT("Invalid Guess Format! Sent as chat."), 5.f, FColor::Red);
			ServerRPCPrintChatMessage(FormattedMsg);
		}
	}
	else if (PS->CurrentStatus == EPlayerStatus::Waiting)
	{
		if (PGS->IsCorrect(ChatString) == ECheckType::Normal)
		{
			FunctionLib::MyPrintString(this, TEXT("Not your turn! Sent as chat."), 5.f, FColor::Red);
		}
		ServerRPCPrintChatMessage(FormattedMsg);
	}
	else
	{
		ServerRPCPrintChatMessage(FormattedMsg);
	}
}

void AProjectPlayerController::PrintChatMessage(const FString& ChatString) const
{
	FunctionLib::MyPrintString(this, ChatString, 10.f);
}

void AProjectPlayerController::ClientRPCPrintChatMessage_Implementation(const FString& ChatString, FColor ChatColor)
{
	FunctionLib::MyPrintString(this, ChatString, 10.f, ChatColor);
}

void AProjectPlayerController::ServerRPCPrintChatMessage_Implementation(const FString& ChatString)
{
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		AProjectGameMode* PGM = Cast<AProjectGameMode>(GM);
		if (IsValid(PGM) == true)
		{
			PGM->ChatMessageStr(this, ChatString);
		}
	}
}


