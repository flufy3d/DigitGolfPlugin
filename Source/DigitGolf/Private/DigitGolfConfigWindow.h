#pragma once

class SDigitGolfConfigWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDigitGolfConfigWindow){}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	FReply OnImportBtn();
	void OnPortTextChanged(const FText& InText, ETextCommit::Type InCommitType);
	FText GetFilePath() const;

private:
	static FString FilePath;
	
};
