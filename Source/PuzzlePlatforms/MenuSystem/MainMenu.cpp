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
	HostButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

	if(!JoinButton) { return false; }
	JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	if(!QuitButton) { return false; }
	QuitButton->OnClicked.AddDynamic(this, &UMainMenu::QuitPressed);

	if(!CancelJoinMenuButton) { return false; }
	CancelJoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	if(!ConfirmJoinMenuButton) { return false; }
	ConfirmJoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);

	return true;
}

void UMainMenu::HostServer()
{
	if(MenuInterface != nullptr)
	{
		MenuInterface->Host();
	}
}

void UMainMenu::SetServerList(TArray<FString> ServerNames)
{

	UWorld* World = this->GetWorld();
	if(!World) { return; }

	ServerList->ClearChildren();
	uint32 i = 0;

	for(const FString& ServerName : ServerNames)
	{
		UServerRow* Row = CreateWidget<UServerRow>(World, ServerRowClass);
		if(!Row) { return; }

		Row->ServerName->SetText(FText::FromString(ServerName));
		Row->Setup(this, i);
		++i;
		ServerList->AddChild(Row);
	}
}

void UMainMenu::SelectIndex(uint32 Index)
{
	SelectedIndex = Index;
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


