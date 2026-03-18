#include "Player/ProjectPlayerController.h"
#include "Player/ProjectPlayerState.h"
#include "ProjectGameState.h"
#include "ProjectGameMode.h"
#include "UI/ChatWidget.h"
#include "u_260319_Project009.h"
#include "Net/UnrealNetwork.h"

AProjectPlayerController::AProjectPlayerController()
{
	bReplicates = true;
}

void AProjectPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController()) return;
	
	SetInputMode(FInputModeUIOnly());
	
	if (ChatWidgetClass)
	{
		ChatWidgetInstance = CreateWidget<UChatWidget>(this, ChatWidgetClass);
		if (ChatWidgetInstance) ChatWidgetInstance->AddToViewport();
	}
	
	if (NotifyWidgetClass)
	{
		NotifyWidgetInstance = CreateWidget<UUserWidget>(this, NotifyWidgetClass);
		if (NotifyWidgetInstance) NotifyWidgetInstance->AddToViewport();
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
	AProjectGameState* PGS = GetWorld() ? GetWorld()->GetGameState<AProjectGameState>() : nullptr;
	
	if (!PS || !PGS) return;

	FString FormattedMsg = FString::Printf(TEXT("%s%s: %s"), *PS->PLName, *PS->GetPlayerTry(), *ChatString);

	if (PS->CurrentStatus == EPlayerStatus::YourTurn)
	{
		if (PGS->IsCorrect(ChatString) != ECheckType::Normal)
		{
			FunctionLib::MyPrintString(this, TEXT("Invalid Guess Format! Sent as chat."), 5.f, FColor::Red);
		}
	}
	else if (PS->CurrentStatus == EPlayerStatus::Waiting && PGS->IsCorrect(ChatString) == ECheckType::Normal)
	{
		FunctionLib::MyPrintString(this, TEXT("Not your turn! Sent as chat."), 5.f, FColor::Red);
	}

	ServerRPCPrintChatMessage(FormattedMsg);
}

void AProjectPlayerController::ClientRPCPrintChatMessage_Implementation(const FString& ChatString, FColor ChatColor)
{
	FunctionLib::MyPrintString(this, ChatString, 10.f, ChatColor);
}

void AProjectPlayerController::ServerRPCPrintChatMessage_Implementation(const FString& ChatString)
{
	if (AProjectGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<AProjectGameMode>() : nullptr)
	{
		GM->ChatMessageStr(this, ChatString);
	}
}