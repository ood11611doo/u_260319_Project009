// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ProjectPawn.h"
#include "u_260319_Project009.h"

void AProjectPawn::BeginPlay()
{
	Super::BeginPlay();
	
	FString NetRoleString = FunctionLib::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("CXPawn::BeginPlay() %s [%s]"), *FunctionLib::GetNetModeString(this), *NetRoleString);
	FunctionLib::MyPrintString(this, CombinedString, 10.f);
}

void AProjectPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	FString NetRoleString = FunctionLib::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("CXPawn::PossessedBy() %s [%s]"), *FunctionLib::GetNetModeString(this), *NetRoleString);
	FunctionLib::MyPrintString(this, CombinedString, 10.f);
}