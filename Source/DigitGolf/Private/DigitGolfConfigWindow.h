#pragma once

class SDigitGolfConfigWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDigitGolfConfigWindow){}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	FReply OnImportBtn();
    FReply OnChooseFileBtn();
	void OnScenePathTextChanged(const FText& InText, ETextCommit::Type InCommitType);
	FText GetFilePath() const;
    void OnObjSearchPathTextChanged( const FText& InText, ETextCommit::Type InCommitType );
    FText GetObjSearchPath() const;
    void OnCommonSearchPathTextChanged( const FText& InText, ETextCommit::Type InCommitType );
    FText GetCommonSearchPath() const;

private:
	static FString FilePath;
    static FString ObjSearchPath;
    static FString CommonSearchPath;
	
};
