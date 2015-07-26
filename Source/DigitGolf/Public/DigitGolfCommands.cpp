// Some copyright should be here...

#include "DigitGolfPrivatePCH.h"
#include "DigitGolfCommands.h"

#define LOCTEXT_NAMESPACE "FDigitGolfModule"

void FDigitGolfCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "DigitGolf", "Bring up DigitGolf window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
