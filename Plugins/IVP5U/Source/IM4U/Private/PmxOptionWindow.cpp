// Copyright 2023 NaN_Name, Inc. All Rights Reserved.


#include "PmxOptionWindow.h"
#include "IM4UPrivatePCH.h"

#include "CoreMinimal.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"

#define LOCTEXT_NAMESPACE "PMXOption"

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 0
#define myFAppStyle FAppStyle::Get()
#else
#define myFAppStyle FAppStyle::Get()
#endif


void SPmxOptionWindow::Construct(const FArguments& InArgs)
{
	ImportUI = InArgs._ImportUI;
	WidgetWindow = InArgs._WidgetWindow;
	bIsObjFormat = InArgs._IsObjFormat;

	check(ImportUI);
	TSharedPtr<SBox> InspectorBox;
	this->ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(2)
			[
				SNew(SBorder)
				.Padding(FMargin(3))
				.BorderImage(myFAppStyle.GetBrush("ToolPanel.GroupBorder"))
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(STextBlock)
						.Font(myFAppStyle.GetFontStyle("CurveEd.LabelFont"))
						.Text(LOCTEXT("Import_CurrentFileTitle", "Current File: "))
					]
					+ SHorizontalBox::Slot()
						.Padding(5, 0, 0, 0)
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(STextBlock)
							.Font(myFAppStyle.GetFontStyle("CurveEd.InfoFont"))
							.Text(InArgs._FullPath)
						]
				]
			]
			+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(2)
				[
					SAssignNew(InspectorBox, SBox)
					.MaxDesiredHeight(650.0f)
					.WidthOverride(400.0f)
				]
			+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Right)
				.Padding(2)
				[
					SNew(SUniformGridPanel)
					.SlotPadding(2)
					/*+ SUniformGridPanel::Slot(0, 0)
					[
						IDocumentation::Get()->CreateAnchor(FString("Engine/Content/FBX/ImportOptions"))
					]
					+ SUniformGridPanel::Slot(0, 0)
						[
							SNew(SButton)
							.HAlign(HAlign_Center)
							.Text(LOCTEXT("MMDOptionWindow_ImportAll", "Import All"))
							.ToolTipText(LOCTEXT("MMDOptionWindow_ImportAll_ToolTip", "Import all files with these same settings"))
							.IsEnabled(this, &SPmxOptionWindow::CanImport)
							.OnClicked(this, &SPmxOptionWindow::OnImportAll)
						]*/
					+ SUniformGridPanel::Slot(1, 0)
						[
							SAssignNew(ImportButton, SButton)
							.HAlign(HAlign_Center)
							.Text(LOCTEXT("MMDOptionWindow_Import", "Import"))
							.IsEnabled(this, &SPmxOptionWindow::CanImport)
							.OnClicked(this, &SPmxOptionWindow::OnImport)
						]
					+ SUniformGridPanel::Slot(2, 0)
						[
							SNew(SButton)
							.HAlign(HAlign_Center)
							.Text(LOCTEXT("MMDOptionWindow_Cancel", "Cancel"))
							.ToolTipText(LOCTEXT("MMDOptionWindow_Cancel_ToolTip", "Cancels importing this MMD file"))
							.OnClicked(this, &SPmxOptionWindow::OnCancel)
						]
				]
		];

	FPropertyEditorModule& PropertyEditorModule 
		= FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	TSharedPtr<IDetailsView> DetailsView
		= PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	InspectorBox->SetContent(DetailsView->AsShared());
	DetailsView->SetObject(ImportUI);
}

bool SPmxOptionWindow::CanImport()  const
{
#if 0
	// do test to see if we are ready to import
	if (ImportUI->MeshTypeToImport == PMXIT_Animation)
	{
		if (ImportUI->Skeleton == NULL || !ImportUI->bImportAnimations)
		{
			return false;
		}
	}

	if (ImportUI->AnimSequenceImportData->AnimationLength == FBXALIT_SetRange)
	{
		if (ImportUI->AnimSequenceImportData->StartFrame > ImportUI->AnimSequenceImportData->EndFrame)
		{
			return false;
		}
	}
#endif
	return true;
}

#undef LOCTEXT_NAMESPACE
