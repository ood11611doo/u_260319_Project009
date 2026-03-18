#include "UI/ChatWidget.h"
#include "Components/EditableTextBox.h"
#include "Player/ProjectPlayerController.h"

void UChatWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (ETBChatInput) ETBChatInput->OnTextCommitted.AddDynamic(this, &UChatWidget::OnChatInputCommitted);
}

void UChatWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (ETBChatInput) ETBChatInput->OnTextCommitted.RemoveDynamic(this, &UChatWidget::OnChatInputCommitted);
}

void UChatWidget::OnChatInputCommitted(const FText& Text, ETextCommit::Type Method)
{
	if (Method == ETextCommit::OnEnter && !Text.IsEmpty())
	{
		if (AProjectPlayerController* PC = Cast<AProjectPlayerController>(GetOwningPlayer()))
		{
			PC->SetChatMessage(Text.ToString());
			ETBChatInput->SetText(FText::GetEmpty());
			ETBChatInput->SetUserFocus(PC);
		}
	}
}