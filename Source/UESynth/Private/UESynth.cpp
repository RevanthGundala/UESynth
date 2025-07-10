// Copyright Epic Games, Inc. All Rights Reserved.

#include "UESynth.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateStyle.h"
#include "LevelEditor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/Commands/UICommandList.h"
#include "Styling/AppStyle.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "FUESynthModule"

static const FName UESynthTabName("UESynth");

void FUESynthModule::StartupModule()
{
	RegisterMenus();
}

void FUESynthModule::ShutdownModule()
{
	UToolMenus::Get()->UnregisterOwner(this);
}

void FUESynthModule::PluginButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("UESynth button clicked!"));
}

void FUESynthModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntry(
				"UESynthButton",
				LOCTEXT("UESynthButton_Label", "UESynth"),
				LOCTEXT("UESynthButton_Tooltip", "Activate the UESynth tab"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateRaw(this, &FUESynthModule::PluginButtonClicked))
			);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(
					"UESynth",
					FUIAction(FExecuteAction::CreateRaw(this, &FUESynthModule::PluginButtonClicked)),
					LOCTEXT("PluginButton_Label", "UESynth"),
					LOCTEXT("PluginButton_Tooltip", "A button for the UESynth plugin"),
					FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Recompile")
				));
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUESynthModule, UESynth)
