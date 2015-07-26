// Some copyright should be here...

#pragma once

#include "ModuleManager.h"


DECLARE_LOG_CATEGORY_EXTERN(LogDigitGolf, Log, All);

class FToolBarBuilder;
class FMenuBuilder;

class FDigitGolfModule : 
	public IModuleInterface

{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FDigitGolfModule& Get()
	{
		return FModuleManager::LoadModuleChecked< FDigitGolfModule >("DigitGolf");
	}

	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();

	void ImportScene(const FString& path);
private:

	void AddToolbarExtension(FToolBarBuilder& Builder);
	void AddMenuExtension(FMenuBuilder& Builder);

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
};