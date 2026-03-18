#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatWidget.generated.h"

class UEditableTextBox;

UCLASS()
class U_260319_PROJECT009_API UChatWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> ETBChatInput;
	
protected:
	UFUNCTION()
	void OnChatInputCommitted(const FText& Text, ETextCommit::Type Method);
};