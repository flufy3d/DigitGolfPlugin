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

void FDigitGolfModule::ParseNode( const TSharedPtr<FJsonObject>& node, AActor*  parent)
{

    auto World = GEditor->GetEditorWorldContext().World();
    ULevel* Level = World->GetCurrentLevel();


    FString name = node->GetStringField( "name" );
    UE_LOG( LogDigitGolf, Log, TEXT( "name: %s" ), *name );

    
    TSharedPtr<FJsonObject> trans = node->GetObjectField( "trans" );
    TArray<TSharedPtr<FJsonValue>> loc = trans->GetArrayField( "loc" );
    float loc_x = loc[0]->AsNumber();
    float loc_y = loc[1]->AsNumber();
    float loc_z = loc[2]->AsNumber();
    TArray<TSharedPtr<FJsonValue>> scale = trans->GetArrayField( "scale" );
    float scale_x = scale[0]->AsNumber();
    float scale_y = scale[1]->AsNumber();
    float scale_z = scale[2]->AsNumber();
    TArray<TSharedPtr<FJsonValue>> rot = trans->GetArrayField( "rot" );
    float rot_x = rot[0]->AsNumber();
    float rot_y = rot[1]->AsNumber();
    float rot_z = rot[2]->AsNumber();


    int search_index = -1;
    name.FindLastChar( '_', search_index );
    FString asset_name = name.Left( search_index + 1 );
	asset_name = ObjSearchPath + '/' + asset_name + '.' + asset_name;

    FString asset_path = asset_name;
    UObject* Asset = GetAssetFromPath( asset_path );
    AActor* Actor = NULL;
    FText ErrorMessage;

    if ( Asset == NULL )
    {
        search_index = -1;
        name.FindLastChar( '_', search_index );
        asset_name = name.Left( search_index + 1 );
		asset_name = CommonSearchPath + '/' + asset_name + '.' + asset_name;

        asset_path = asset_name;
        Asset = GetAssetFromPath( asset_path );
    }

    if ( Asset != NULL )
    {
        const FAssetData AssetData( Asset );
       

        if ( StaticMeshActorFactory->CanCreateActorFrom( AssetData, ErrorMessage ) )
        {
            Actor = StaticMeshActorFactory->CreateActor( Asset, Level, FTransform::Identity,
                                                    RF_Transactional );

            Actor->SetActorLabel( name );

            Actor->SetActorLocation( FVector( loc_x, loc_y, loc_z ) );
            Actor->SetActorRotation( FRotator( rot_x, rot_y, rot_z ) );
            Actor->SetActorScale3D( FVector( scale_x, scale_y, scale_z ) );
            Actor->GetRootComponent()->SetMobility( EComponentMobility::Movable );

        }
        else
        {
            UE_LOG( LogDigitGolf, Log, TEXT( "error_msg :%s" ), *ErrorMessage.ToString() );
        }

    }
    else
    {
        Asset = GetAssetFromPath( "/Script/Engine.Actor" );


        const FAssetData AssetData( Asset );


        if ( EmptyActorFactory->CanCreateActorFrom( AssetData, ErrorMessage ) )
        {
            Actor = EmptyActorFactory->CreateActor( Asset, Level, FTransform::Identity,
                                                    RF_Transactional );

            Actor->SetActorLabel( name );

            Actor->SetActorLocation( FVector( loc_x, loc_y, loc_z ) );
            Actor->SetActorRotation( FRotator( rot_x, rot_y, rot_z ) );
            Actor->SetActorScale3D( FVector( scale_x, scale_y, scale_z ) );

        }
        else
        {
            UE_LOG( LogDigitGolf, Log, TEXT( "error_msg :%s" ), *ErrorMessage.ToString() );
        }

    }


    GEditor->ParentActors( parent, Actor, NAME_None );


    TArray<TSharedPtr<FJsonValue>> children = node->GetArrayField( "children" );
    int num = children.Num();
    if ( num > 0 )
    {
        for ( auto Itr( children.CreateIterator() ); Itr; Itr++ )
        {
            ParseNode( ( *Itr )->AsObject(), Actor );
        }
    }
    
}

void FDigitGolfModule::ImportScene( const FString& path, const FString& objSearchPath ,const FString& commonSearchPath )
{
	UE_LOG(LogDigitGolf, Log, TEXT("ImportScene"));
	UE_LOG(LogDigitGolf, Log, TEXT("scene file location :%s"), *path);

    ObjSearchPath = objSearchPath;
    CommonSearchPath = commonSearchPath;


	UE_LOG(LogDigitGolf, Log, TEXT("ImportScene begin"));
    

    TSharedPtr<FJsonObject> ProjectFile = LoadProjectFile( path );
    if ( ProjectFile.IsValid() )
    {

        auto World = GEditor->GetEditorWorldContext().World();
        ULevel* Level = World->GetCurrentLevel();

        UObject* Asset = GetAssetFromPath( "/Script/Engine.Actor" );
        const FAssetData AssetData( Asset );

        FText ErrorMessage;
        AActor* Actor = NULL;

        if ( EmptyActorFactory->CanCreateActorFrom( AssetData, ErrorMessage ) )
        {
            Actor = EmptyActorFactory->CreateActor( Asset, Level, FTransform::Identity,
                                                    RF_Transactional );

            Actor->SetActorLabel( "Anchor" );

            Actor->SetActorLocation( FVector( 0, 0, 0 ) );
            Actor->SetActorRotation( FRotator( 0, 0, 0 ) );
            Actor->SetActorScale3D( FVector( 1, 1, 1 ) );

        }
        else
        {
            UE_LOG( LogDigitGolf, Log, TEXT( "error_msg :%s" ), *ErrorMessage.ToString() );
        }



        FString version = ProjectFile->GetStringField( "version" );
        TArray<TSharedPtr<FJsonValue>> nodes = ProjectFile->GetArrayField( "scene" );
        
        for ( auto Itr( nodes.CreateIterator() ); Itr; Itr++ )
        {
            ParseNode( ( *Itr )->AsObject(), Actor );
        }
    }

    UE_LOG( LogDigitGolf, Log, TEXT( "ImportScene end" ) );


    FNotificationInfo Info( LOCTEXT( "DigitGolfImportDoneMessage", "ImportScene Done!" ) );
    Info.bFireAndForget = true;
    Info.bUseThrobber = false;
    Info.FadeOutDuration = 3.0f;
    Info.ExpireDuration = 3.0f;

    FSlateNotificationManager::Get().AddNotification( Info );

    


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