// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "UObject/ConstructorHelpers.h"
#include "ServerRow.h"

UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/MenuSystem/WBP_ServerRow"));
	if(!ensure(ServerRowBPClass.Class)) { return; }
	ServerRowClass = ServerRowBPClass.Class;
}

bool UMainMenu::Initialize()
{
	bool Succes = Super::Initialize();
	if(!Succes) { return false; }

	if(!HostButton) { return false; }
	HostButton->OnClicked.AddDynamic(this, &UMainMenu::OpenHostMenu);

	if(!JoinButton) { return false; }
	JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	if(!QuitButton) { return false; }
	QuitButton->OnClicked.AddDynamic(this, &UMainMenu::QuitPressed);

	if(!CancelJoinMenuButton) { return false; }
	CancelJoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	if(!ConfirmJoinMenuButton) { return false; }
	ConfirmJoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);

	if(!CancelHostMenuButton) { return false; }
	CancelHostMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	if(!ConfirmHostMenuButton) { return false; }
	ConfirmHostMenuButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

	return true;
}

void UMainMenu::OpenHostMenu()
{
	if(MenuSwitcher != nullptr)
	{
		if(HostMenu != nullptr)
		{
			MenuSwitcher->SetActiveWidget(HostMenu);
		}
	}
}

void UMainMenu::HostServer()
{
	if(MenuInterface != nullptr)
	{
		FString ServerName = ServerHostName->Text.ToString();
		MenuInterface->Host(ServerName);
	}
}

void UMainMenu::SetServerList(TArray<FServerData> ServerDatas)
{

	UWorld* World = this->GetWorld();
	if(!World) { return; }

	ServerList->ClearChildren();
	uint32 i = 0;

	for(const FServerData& ServerData : ServerDatas)
	{
		UServerRow* Row = CreateWidget<UServerRow>(World, ServerRowClass);
		if(!Row) { return; }

		Row->ServerName->SetText(FText::FromString(ServerData.Name));
		Row->HostUser->SetText(FText::FromString(ServerData.HostUsername));
		FString FractionText = FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers);
		Row->ConnectionFraction->SetText(FText::FromString(FractionText));
		Row->Setup(this, i);
		++i;
		ServerList->AddChild(Row);
	}
}

void UMainMenu::SelectIndex(uint32 Index)
{
	SelectedIndex = Index;
	UpdateChildren();
}


void UMainMenu::UpdateChildren()
{
	for(int32 i = 0; i < ServerList->GetChildrenCount(); ++i)
	{
		auto Row = Cast<UServerRow>(ServerList->GetChildAt(i));
		if(Row != nullptr)
		{
			Row->Selected = (SelectedIndex.IsSet() && (SelectedIndex.GetValue() == i));
		}
	}
}

void UMainMenu::JoinServer()
{
	if(MenuInterface != nullptr)
	{
		if(SelectedIndex.IsSet() && MenuInterface != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Selected index %d."), SelectedIndex.GetValue());
			MenuInterface->Join(SelectedIndex.GetValue());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Selected index not set."));
		}
		
	}
}

void UMainMenu::QuitPressed()
{
	UWorld* World = GetWorld();
	if(!ensure(World)) { return; }
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if(!ensure(PlayerController)) { return; }

	PlayerController->ConsoleCommand("quit");
}


void UMainMenu::OpenJoinMenu()
{
	if(MenuSwitcher != nullptr)
	{
		if(JoinMenu != nullptr)
		{
			MenuSwitcher->SetActiveWidget(JoinMenu);
			if(MenuInterface != nullptr)
			{
				MenuInterface->RefreshServerList();
			}
		}
	}
}

void UMainMenu::OpenMainMenu()
{
	if(MenuSwitcher != nullptr)
	{
		if(MainMenu != nullptr)
		{
			MenuSwitcher->SetActiveWidget(MainMenu);
		}
	}
}


