// Some copyright should be here...

#include "DigitGolfPrivatePCH.h"

#include "GameFramework/Actor.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Json.h"
#include "IDesktopPlatform.h"
#include "DesktopPlatformModule.h"

#include "DigitGolfStyle.h"
#include "DigitGolfCommands.h"

#include "LevelEditor.h"

#include "DigitGolfConfigWindow.h"

DEFINE_LOG_CATEGORY(LogDigitGolf)

static const FName DigitGolfTabName("DigitGolf");

#define LOCTEXT_NAMESPACE "FDigitGolfModule"

UObject* GetAssetFromPath(FString AssetPath)
{
	// If there is no dot, add a dot and repeat the object name.
	// /Game/Meshes/MyStaticMesh.MyStaticMesh would be the actual path
	// to the object, while the MyStaticMesh before the dot is the package
	// copied from ConstructorHelpers
	int32 PackageDelimPos = INDEX_NONE;
	AssetPath.FindChar(TCHAR('.'), PackageDelimPos);
	if (PackageDelimPos == INDEX_NONE)
	{
		int32 ObjectNameStart = INDEX_NONE;
		AssetPath.FindLastChar(TCHAR('/'), ObjectNameStart);
		if (ObjectNameStart != INDEX_NONE)
		{
			const FString ObjectName = AssetPath.Mid(ObjectNameStart + 1);
			AssetPath += TCHAR('.');
			AssetPath += ObjectName;
		}
	}


	// try to find the asset
	UE_LOG(LogDigitGolf, Log, TEXT("Trying to find Asset %s."), *AssetPath);
	//UObject* Asset = FindObject<UObject>( ANY_PACKAGE, *AssetPath, false );
	UObject* Asset = StaticLoadObject(UObject::StaticClass(), NULL, *AssetPath);
	if (Asset == NULL)
	{
		UE_LOG(LogDigitGolf, Log, TEXT("Failed to find Asset %s."), *AssetPath);
		return NULL;
	}
	return Asset;
}

void FDigitGolfModule::FindRelativeFactory()
{
	for (UActorFactory* ActorFactory : GEditor->ActorFactories)
	{
		const FString& fac_name = ActorFactory->GetFName().ToString();
		if (fac_name.Equals("ActorFactoryStaticMesh_0") )
		{
			StaticMeshActorFactory = ActorFactory;
			UE_LOG(LogDigitGolf, Log, TEXT("load %s"), *(ActorFactory->GetFName().ToString()));
		}
		else if (fac_name.Equals("ActorFactoryEmptyActor_0") )
		{
			EmptyActorFactory = ActorFactory;
			UE_LOG(LogDigitGolf, Log, TEXT("load %s"), *(ActorFactory->GetFName().ToString()));
		}
		
		
		
	}
}

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
	FindRelativeFactory();
}

TSharedPtr<FJsonObject> LoadProjectFile(const FString &FileName)
{
	FString FileContents;

	if (!FFileHelper::LoadFileToString(FileContents, *FileName))
	{
		return TSharedPtr<FJsonObject>(NULL);
	}

	TSharedPtr< FJsonObject > JsonObject;
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(FileContents);
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		return TSharedPtr<FJsonObject>(NULL);
	}

	return JsonObject;
}
void FDigitGolfModule::ImportScene(const FString& path)
{
	UE_LOG(LogDigitGolf, Log, TEXT("ImportScene"));
	UE_LOG(LogDigitGolf, Log, TEXT("___ %s ____ "), *path);

	auto World = GEditor->GetEditorWorldContext().World();
	ULevel* Level = World->GetCurrentLevel();

	static AActor* myactor;
	if (path.Equals("Add"))
	{
		UE_LOG(LogDigitGolf, Log, TEXT("ImportScene"));

		TArray<FString> file_path;

		void* ParentWindowPtr = FSlateApplication::Get().GetActiveTopLevelWindow()->GetNativeWindow()->GetOSWindowHandle();

		if (ParentWindowPtr)
		{
			IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

			if (DesktopPlatform)
			{
				FString FileTypes = TEXT("Images (*.json)|*.json|All Files (*.*)|*.*");

				DesktopPlatform->OpenFileDialog(
					ParentWindowPtr,
					"ChooseSceneFile",
					TEXT(""),
					TEXT(""),
					FileTypes,
					EFileDialogFlags::None,
					file_path
					);
			}

			TSharedPtr<FJsonObject> ProjectFile = LoadProjectFile(file_path[0]);
			if (ProjectFile.IsValid())
			{
				FString version = ProjectFile->GetStringField("version");
				//TSharedPtr<FJsonObject> scene = ProjectFile->GetObjectField("scene");
				TArray<TSharedPtr<FJsonValue>> nodes = ProjectFile->GetArrayField("scene");


	
				for (auto Itr(nodes.CreateIterator()); Itr; Itr++)
				{
					TSharedPtr<FJsonObject> node = (*Itr)->AsObject();
					FString name = node->GetStringField("name");
					UE_LOG(LogDigitGolf, Log, TEXT("name: %s"), *name);

				}
				

				

			}

		}


		//UObject* Asset = GetAssetFromPath("/Script/Engine.Actor");
		FString path = TEXT("/Game/NineDragonsA/Object/o_jiulongrock3_.o_jiulongrock3_");
		UObject* Asset = GetAssetFromPath(path);
		if (Asset == NULL)
		{
			UE_LOG(LogDigitGolf, Log, TEXT("can not found asset: %s"),*path);
			return;
		}
			
		const FAssetData AssetData(Asset);

		FText ErrorMessage;
		AActor* Actor = NULL;

		if (StaticMeshActorFactory->CanCreateActorFrom(AssetData, ErrorMessage))
		{
			Actor = StaticMeshActorFactory->CreateActor(Asset, Level, FTransform::Identity,
				RF_Transactional);

			Actor->SetActorLabel(TEXT("kenshin1987"));

			Actor->SetActorLocation(FVector(0, 100,261));
			Actor->SetActorRotation(FRotator(0, 0, 90));
			Actor->SetActorScale3D(FVector(2, 2, 2));
			myactor = Actor;

		}
		else
		{
			UE_LOG(LogDigitGolf, Log, TEXT("error_msg :%s"), *ErrorMessage.ToString());
		}

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