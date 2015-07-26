#include "DigitGolfPrivatePCH.h"
#include "DigitGolfConfigWindow.h"


#define LOCTEXT_NAMESPACE "FDigitGolfModule"

FString SDigitGolfConfigWindow::FilePath = FString(TEXT("Path/scene.json"));

void SDigitGolfConfigWindow::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew( SOverlay )
		+SOverlay::Slot()
		[
			SNew( SVerticalBox )
			+SVerticalBox::Slot()
			.AutoHeight()
			.Padding(2.0f)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.0f)
				[
					SNew(SButton)
					.Text(LOCTEXT("ImportBtn", "Import Scene"))
					.OnClicked(this, &SDigitGolfConfigWindow::OnImportBtn)
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.0f)
				[
					SNew(SEditableTextBox)
					.SelectAllTextWhenFocused(true)
					.Text(this, &SDigitGolfConfigWindow::GetFilePath)
					.ToolTipText( LOCTEXT("PathTooltip", "set the Path of scene.json") )
					.OnTextCommitted(this, &SDigitGolfConfigWindow::OnPortTextChanged)
					.OnTextChanged(this, &SDigitGolfConfigWindow::OnPortTextChanged, ETextCommit::Default)
				]
			]
		]
	];

}

FReply SDigitGolfConfigWindow::OnImportBtn()
{	
	FDigitGolfModule& module = FDigitGolfModule::Get();
	module.ImportScene(FilePath);
	return FReply::Handled();
}

void SDigitGolfConfigWindow::OnPortTextChanged(const FText& InText, ETextCommit::Type InCommitType)
{
	FilePath = InText.ToString();
}

FText SDigitGolfConfigWindow::GetFilePath() const
{
	return FText::FromString(FilePath);
}

#undef LOCTEXT_NAMESPACE