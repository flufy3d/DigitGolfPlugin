// Some copyright should be here...

#include "DigitGolfPrivatePCH.h"

#include "GameFramework/Actor.h"
#include "SlateBasics.h"
#include "SlateExtras.h"

#include "DigitGolfStyle.h"
#include "DigitGolfCommands.h"

#include "LevelEditor.h"

#include "DigitGolfConfigWindow.h"

DEFINE_LOG_CATEGORY(LogDigitGolf)

static const FName DigitGolfTabName("DigitGolf");

#define LOCTEXT_NAMESPACE "FDigitGolfModule"

void FDigitGolfModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FDigitGolfStyle::Initialize();
	FDigitGolfStyle::ReloadTextures();

	FDigitGolfCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FDigitGolfCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FDigitGolfModule::PluginButtonClicked),
		FCanExecuteAction());
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FDigitGolfModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FDigitGolfModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(DigitGolfTabName, FOnSpawnTab::CreateRaw(this, &FDigitGolfModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FDigitGolfTabTitle", "DigitGolf"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
	
}

void FDigitGolfModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
	FDigitGolfStyle::Shutdown();

	FDigitGolfCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(DigitGolfTabName);
}

TSharedRef<SDockTab> FDigitGolfModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			//SNullWidget::NullWidget
			SNew(SDigitGolfConfigWindow)

		];

}

void FDigitGolfModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->InvokeTab(DigitGolfTabName);
}
void FDigitGolfModule::ImportScene(const FString& path)
{
	UE_LOG(LogDigitGolf, Log, TEXT("ImportScene"));
	UE_LOG(LogDigitGolf, Log, TEXT("___ %s ____ "), *path);


	static AActor* myactor;
	if (path.Equals("Add"))
	{
		UE_LOG(LogDigitGolf, Log, TEXT("ImportScene"));
		auto World = GEditor->GetEditorWorldContext().World();
		ULevel* Level = World->GetCurrentLevel();

		myactor = World->SpawnActor<AActor>(FVector::ForwardVector, FRotator::ZeroRotator);
	
		//myactor->Rename(TEXT("kenshin1987"));
		myactor->SetActorLabel(TEXT("kenshin1987"));
	
		//this code must in construct fucntion
		//USceneComponent* MyUSceneComponent = myactor->CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
		//myactor->SetRootComponent(MyUSceneComponent);


	}
	else if (path.Equals("Del"))
	{
		UE_LOG(LogDigitGolf, Log, TEXT("DelScene"));
		auto World = GEditor->GetEditorWorldContext().World();
		ULevel* Level = World->GetCurrentLevel();
		World->DestroyActor(myactor);
	}
	
}
void FDigitGolfModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FDigitGolfCommands::Get().OpenPluginWindow);
}

void FDigitGolfModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FDigitGolfCommands::Get().OpenPluginWindow);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDigitGolfModule, DigitGolf)