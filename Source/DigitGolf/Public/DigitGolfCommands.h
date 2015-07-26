// Some copyright should be here...

#pragma once

#include "SlateBasics.h"
#include "DigitGolfStyle.h"

class FDigitGolfCommands : public TCommands<FDigitGolfCommands>
{
public:

	FDigitGolfCommands()
		: TCommands<FDigitGolfCommands>(TEXT("DigitGolf"), NSLOCTEXT("Contexts", "DigitGolf", "DigitGolf Plugin"), NAME_None, FDigitGolfStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};