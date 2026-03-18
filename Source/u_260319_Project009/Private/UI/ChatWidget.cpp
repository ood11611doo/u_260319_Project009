// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ChatWidget.h"
#include "Components/EditableTextBox.h"
#include "Player/ProjectPlayerController.h"

void UChatWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (!ETBChatInput->OnTextCommitted.IsAlreadyBound(this, &UChatWidget::OnChatInputCommitted))
	{
		ETBChatInput->OnTextCommitted.AddDynamic(this, &UChatWidget::OnChatInputCommitted);
	}
}

void UChatWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	if (ETBChatInput->OnTextCommitted.IsAlreadyBound(this, &UChatWidget::OnChatInputCommitted))
	{
		ETBChatInput->OnTextCommitted.RemoveDynamic(this, &UChatWidget::OnChatInputCommitted);
	}
}

void UChatWidget::OnChatInputCommitted(const FText& Text, ETextCommit::Type Method)
{
	if (Method == ETextCommit::OnEnter)
	{
		APlayerController* PC = GetOwningPlayer();
		if (IsValid(PC))
		{
			AProjectPlayerController* Controller = static_cast<AProjectPlayerController*>(PC);
			if (IsValid(Controller))
			{
				Controller->SetChatMessage(Text.ToString());
				ETBChatInput->SetText(FText());
				ETBChatInput->SetUserFocus(Controller);
			}
		}
	}
}