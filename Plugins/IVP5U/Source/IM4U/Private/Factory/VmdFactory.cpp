// Copyright 2023 NaN_Name. All Rights Reserved.

#include "Factory/VmdFactory.h"
#include "../IM4UPrivatePCH.h"

#include "VmdImporter.h"

#include "CoreMinimal.h"
#include "ImportUtils/SkelImport.h"
#include "AnimationUtils.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"

#include "VmdImportUI.h"
#include "RigEditor/IKRigController.h"


#include "Factory/VmdImportOption.h"


#define LOCTEXT_NAMESPACE "VMDImportFactory"


DEFINE_LOG_CATEGORY(LogMMD4UE4_VMDFactory)

#define ADD_NAME_MAP( x , y ) NameMap.Add((y),(x))

void initMmdNameMap()
{
	if (NameMap.Num() == 0)
	{
		ADD_NAME_MAP(L"左足", L"左足D");
		ADD_NAME_MAP(L"左ひざ", L"左ひざD");
		ADD_NAME_MAP(L"左足首", L"左足首D");
		ADD_NAME_MAP(L"左つま先", L"左足先EX");
		ADD_NAME_MAP(L"右足", L"右足D");
		ADD_NAME_MAP(L"右ひざ", L"右ひざD");
		ADD_NAME_MAP(L"右足首", L"右足首D");
		ADD_NAME_MAP(L"右つま先", L"右足先EX");
		//ADD_NAME_MAP(L"センター", L"腰");
		ADD_NAME_MAP(L"腰キャンセル右", L"右腰キャンセル");
		ADD_NAME_MAP(L"腰キャンセル左", L"左腰キャンセル");
	}
#if 0
	if (NameMap.Num() == 0)
	{
		ADD_NAME_MAP(L"操作中心", L"op_center");
		ADD_NAME_MAP(L"全ての親", L"all_parent");
		ADD_NAME_MAP(L"センター", L"center");
		ADD_NAME_MAP(L"センター2", L"center2");
		ADD_NAME_MAP(L"グルーブ", L"groove");
		ADD_NAME_MAP(L"グルーブ2", L"groove2");
		ADD_NAME_MAP(L"腰", L"waist");
		ADD_NAME_MAP(L"下半身", L"lowerBody");
		ADD_NAME_MAP(L"上半身", L"upperBody");
		ADD_NAME_MAP(L"上半身", L"upperBody2");
		ADD_NAME_MAP(L"首", L"neck");
		ADD_NAME_MAP(L"頭", L"head");
		ADD_NAME_MAP(L"左目", L"eyeL");
		ADD_NAME_MAP(L"右目", L"eyeR");
		
		ADD_NAME_MAP(L"左肩", L"shoulderL");
		ADD_NAME_MAP(L"左腕", L"armL");
		ADD_NAME_MAP(L"左ひじ", L"elbowL");
		ADD_NAME_MAP(L"左手首", L"wristL");
		ADD_NAME_MAP(L"左親指０", L"thumb0L");
		ADD_NAME_MAP(L"左親指１", L"thumb1L");
		ADD_NAME_MAP(L"左親指２", L"thumb2L");
		ADD_NAME_MAP(L"左人指０", L"fore0L");
		ADD_NAME_MAP(L"左人指１", L"fore1L");
		ADD_NAME_MAP(L"左人指２", L"fore2L");
		ADD_NAME_MAP(L"左中指０", L"middle0L");
		ADD_NAME_MAP(L"左中指１", L"middle1L");
		ADD_NAME_MAP(L"左中指２", L"middle2L");
		ADD_NAME_MAP(L"左薬指０", L"third0L");
		ADD_NAME_MAP(L"左薬指１", L"third1L");
		ADD_NAME_MAP(L"左薬指２", L"third2L");
		ADD_NAME_MAP(L"左小指０", L"little0L");
		ADD_NAME_MAP(L"左小指１", L"little1L");
		ADD_NAME_MAP(L"左小指２", L"little2L");
		ADD_NAME_MAP(L"左足", L"legL");
		ADD_NAME_MAP(L"左ひざ", L"kneeL");
		ADD_NAME_MAP(L"左足首", L"ankleL");
		
		ADD_NAME_MAP(L"右肩", L"shoulderR");
		ADD_NAME_MAP(L"右腕", L"armR");
		ADD_NAME_MAP(L"右ひじ", L"elbowR");
		ADD_NAME_MAP(L"右手首", L"wristR");
		ADD_NAME_MAP(L"右親指０", L"thumb0R");
		ADD_NAME_MAP(L"右親指１", L"thumb1R");
		ADD_NAME_MAP(L"右親指２", L"thumb2R");
		ADD_NAME_MAP(L"右人指０", L"fore0R");
		ADD_NAME_MAP(L"右人指１", L"fore1R");
		ADD_NAME_MAP(L"右人指２", L"fore2R");
		ADD_NAME_MAP(L"右中指０", L"middle0R");
		ADD_NAME_MAP(L"右中指１", L"middle1R");
		ADD_NAME_MAP(L"右中指２", L"middle2R");
		ADD_NAME_MAP(L"右薬指０", L"third0R");
		ADD_NAME_MAP(L"右薬指１", L"third1R");
		ADD_NAME_MAP(L"右薬指２", L"third2R");
		ADD_NAME_MAP(L"右小指０", L"little0R");
		ADD_NAME_MAP(L"右小指１", L"little1R");
		ADD_NAME_MAP(L"右小指２", L"little2R");
		ADD_NAME_MAP(L"右足", L"legR");
		ADD_NAME_MAP(L"右ひざ", L"kneeR");
		ADD_NAME_MAP(L"右足首", L"ankleR");

		ADD_NAME_MAP(L"両目", L"eyes");
		ADD_NAME_MAP(L"左足ＩＫ", L"ikLegL");
		ADD_NAME_MAP(L"右足ＩＫ", L"ikLegR");
		ADD_NAME_MAP(L"左つま先ＩＫ", L"ikToeL");
		ADD_NAME_MAP(L"右つま先ＩＫ", L"ikToeR");

	}
#endif
}

/////////////////////////////////////////////////////////
//prototype ::from dxlib 
//创建以X轴为中心的旋转矩阵
void CreateRotationXMatrix(FMatrix* Out, float Angle);
// 求仅旋转分量矩阵的积（3）×3以外的部分也不代入值）
void MV1LoadModelToVMD_CreateMultiplyMatrixRotOnly(FMatrix* Out, FMatrix* In1, FMatrix* In2);
// 判定角度限制的共同函数（subIndexJdg的判定比较不明…）
void CheckLimitAngle(
	const FVector& RotMin,
	const FVector& RotMax,
	FVector* outAngle, //target angle ( in and out param)
	bool subIndexJdg //(ik link index < ik loop temp):: linkBoneIndex < ikt
);
///////////////////////////////////////////////////////

UVmdFactory::UVmdFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = NULL;
	//SupportedClass = UPmxFactory::StaticClass();
	Formats.Empty();

	Formats.Add(TEXT("vmd;vmd animations"));

	bCreateNew = false;
	bText = false;
	bEditorImport = true;

	initMmdNameMap();
}

void UVmdFactory::PostInitProperties()
{
	Super::PostInitProperties();

	ImportUI = NewObject<UVmdImportUI>(this, NAME_None, RF_NoFlags);
}

bool UVmdFactory::DoesSupportClass(UClass* Class)
{
	return (Class == UVmdFactory::StaticClass());
}

UClass* UVmdFactory::ResolveSupportedClass()
{
	return UVmdFactory::StaticClass();
}

UObject* UVmdFactory::FactoryCreateBinary
(
	UClass* Class,
	UObject* InParent,
	FName Name,
	EObjectFlags Flags,
	UObject* Context,
	const TCHAR* Type,
	const uint8*& Buffer,
	const uint8* BufferEnd,
	FFeedbackContext* Warn,
	bool& bOutOperationCanceled
)
{
	MMD4UE4::VmdMotionInfo vmdMotionInfo;

	if (vmdMotionInfo.VMDLoaderBinary(Buffer, BufferEnd) == false)
	{
		UE_LOG(LogMMD4UE4_VMDFactory, Error,
		       TEXT("VMD Import Cancel:: vmd data load faile."));
		return NULL;
	}

	/////////////////////////////////////////
	UAnimSequence* LastCreatedAnim = NULL;
	USkeleton* Skeleton = NULL;
	USkeletalMesh* SkeletalMesh = NULL;
	UIKRigDefinition* IKRig = NULL;
	VMDImportOptions* ImportOptions = NULL;

	if (false)
	{
		//读取模型后的警告文显示：注释栏
		FText TitleStr = FText::Format(
			LOCTEXT("ImportReadMe_Generic_Dbg", "{0} 制限事項"), FText::FromString("IM4U Plugin"));
		const FText MessageDbg
			= FText(LOCTEXT("ImportReadMe_Generic_Dbg_Comment",
			                "当前有效的参数包括：\n\
				：：Skeleton Asset（必需：与动画相关联）\n\
				：：SkeletalMesh Asset（可选：Animation关联到MorphTarget.NULL时，MorphTargetSkip。）\n\
				：：动画资源（执行仅将Morph添加到现有资源（非空值）的过程。在空值下创建包含Bone和Morph的新资源）\n\
				：：DataTable（MMD2UE4Name）Asset（任意：在NULL以外读取时，用MMD = UE4替换Bone和MorphName，执行导入。需要事先以CSV形式导入或新建。）\n\
				：：MmdExtendAsse（可选：在NULL以外从VMD生成AnimSeq资产时，从Extend参照IK信息进行计算时使用。必须事先导入模型或手动生成资产。）\n\
				\n\
				\n\
				注意：新Asset生成因IK等未对应而不推荐。仅支持追加Morph。"
				)
			);
		FMessageDialog::Open(EAppMsgType::Ok, MessageDbg, &TitleStr);
	}
	/***************************************
	* 导入VMD时显示警告
	****************************************/
	if (false)
	{
		//读取模型后的警告文显示：注释栏
		FText TitleStr = FText(LOCTEXT("ImportVMD_TargetModelInfo", "警告[ImportVMD_TargetModelInfo]"));
		const FText MessageDbg
			= FText::Format(LOCTEXT("ImportVMD_TargetModelInfo_Comment",
			                        "注意：运动数据取入信息：\\\
				\n\
				此VMD是为“{ 0 }”创建的文件。\n\
				\n\
				对于模型运动，仅捕获具有相同骨骼名称的数据。\n\
				如果包含名称与模型侧骨骼名称不同的相同骨骼，则\n\
				预先创建转换表（MMD2UE4NameTableRow），\n\
				可通过在InportOption画面中指定进行导入。"
			)
			                , FText::FromString(vmdMotionInfo.ModelName)
			);
		FMessageDialog::Open(EAppMsgType::Ok, MessageDbg, &TitleStr);
	}
	/////////////////////////////////////
	// factory animation asset from vmd data.
	////////////////////////////////////
	if (vmdMotionInfo.keyCameraList.Num() == 0)
	{
		//如果不是摄影机动画
		FVmdImporter* VmdImporter = FVmdImporter::GetInstance();

		EVMDImportType ForcedImportType = VMDIT_Animation;
		bool bOperationCanceled = false;
		bool bIsPmxFormat = true;
		// show Import Option Slate
		bool bImportAll = false;
		ImportUI->bIsObjImport = false; //anim mode
		ImportUI->OriginalImportType = EVMDImportType::VMDIT_Animation;
		ImportOptions
			= GetVMDImportOptions(
				VmdImporter,
				ImportUI,
				true, //bShowImportDialog, 
				InParent->GetPathName(),
				bOperationCanceled,
				bImportAll,
				ImportUI->bIsObjImport, //bIsPmxFormat,
				bIsPmxFormat,
				ForcedImportType
			);
		/* 第一次判定 */
		if (ImportOptions)
		{
			Skeleton = ImportUI->Skeleton;
			SkeletalMesh = ImportUI->SkeletonMesh;
			/* 最低限度的参数设置检查 */
			if ((!Skeleton) || (!SkeletalMesh) || (Skeleton != SkeletalMesh->GetSkeleton()))
			{
				UE_LOG(LogMMD4UE4_VMDFactory, Warning,
				       TEXT("[ImportAnimations]::Parameter check for Import option!"));

				{
					//	读取模型后的警告文显示：注释栏
					FText TitleStr = FText(LOCTEXT("ImportVMD_OptionWarn_CheckPh1", "警告[ImportVMD_TargetModelInfo]"));
					const FText MessageDbg
						= FText::Format(LOCTEXT("ImportVMD_OptionWarn_CheckPh1_Comment",
						                        "注意：“导入”选项的参数检查：：\n\
						\n\
						[强制要求](必須)\n\
						-骨架资源：选择目标骨架。\n\
						如果为NULL，则表示导入错误。\n\
						[可选](任意)\n\
						-骨骼网格资源：选择目标骨骼网格。\n\
						-但是，SkellMesh包括骨架。(但是，网格必须选择相同的骨架)\n\
						如果为NULL，则跳过导入变形曲线。(未捕获变形)\n\
						\n\
						重试导入选项！"
						)
						                , FText::FromString(vmdMotionInfo.ModelName)
						);
					FMessageDialog::Open(EAppMsgType::Ok, MessageDbg, &TitleStr);
				}
				/* 再来一次*/
				ImportOptions
					= GetVMDImportOptions(
						VmdImporter,
						ImportUI,
						true, //bShowImportDialog, 
						InParent->GetPathName(),
						bOperationCanceled,
						bImportAll,
						ImportUI->bIsObjImport, //bIsPmxFormat,
						bIsPmxFormat,
						ForcedImportType
					);
			}
		}
		if (ImportOptions)
		{
			Skeleton = ImportUI->Skeleton;
			SkeletalMesh = ImportUI->SkeletonMesh;
			IKRig = ImportUI->IKRig;
			bool preParamChk = true; 
			/*包含关系检查*/
			if (SkeletalMesh)
			{
				if (Skeleton != SkeletalMesh->GetSkeleton())
				{
					//TBD::ERR case
					{
						UE_LOG(LogMMD4UE4_VMDFactory, Error,
						       TEXT("ImportAnimations : Skeleton not equrl skeletalmesh->skelton ...")
						);
					}
					preParamChk = false;
				}
			}
			if (preParamChk)
			{
				////////////////////////////////////
				if (!ImportOptions->AnimSequenceAsset)
				{
					//create AnimSequence Asset from VMD
					LastCreatedAnim = ImportAnimations(
						Skeleton,
						SkeletalMesh,
						InParent,
						Name.ToString(),
						IKRig,
						ImportUI->MMD2UE4NameTableRow,
						ImportUI->MmdExtendAsset,
						&vmdMotionInfo
					);
				}
				else
				{
					//TBB::Option中未选择AinimSeq时，结束
					// add morph curve only to exist ainimation
					LastCreatedAnim = AddtionalMorphCurveImportToAnimations(
						SkeletalMesh,
						ImportOptions->AnimSequenceAsset, //UAnimSequence* exsistAnimSequ,
						ImportUI->MMD2UE4NameTableRow,
						&vmdMotionInfo
					);
				}
			}
			else
			{
				//TBD::ERR case
				{
					UE_LOG(LogMMD4UE4_VMDFactory, Error,
					       TEXT("ImportAnimations : preParamChk false. import ERROR !!!! ...")
					);
				}
			}
		}
		else
		{
			UE_LOG(LogMMD4UE4_VMDFactory, Warning,
			       TEXT("VMD Import Cancel"));
		}
	}
	else
	{
		//导入相机动画
		//今后安装预定？
		UE_LOG(LogMMD4UE4_VMDFactory, Warning,
		       TEXT("VMD Import Cancel::Camera root... not impl"));

		LastCreatedAnim = NULL;
	}
	return LastCreatedAnim;
};

UAnimSequence* UVmdFactory::ImportAnimations(
	USkeleton* Skeleton,
	USkeletalMesh* SkeletalMesh,
	UObject* Outer,
	const FString& Name,
	//UFbxAnimSequenceImportData* TemplateImportData, 
	//TArray<FbxNode*>& NodeArray
	UIKRigDefinition* IKRig,
	UDataTable* ReNameTable,
	UMMDExtendAsset* mmdExtend,
	MMD4UE4::VmdMotionInfo* vmdMotionInfo
)
{
	UAnimSequence* LastCreatedAnim = NULL;


	// we need skeleton to create animsequence
	if (Skeleton == NULL)
	{
		//TBD::ERR case
		{
			UE_LOG(LogMMD4UE4_VMDFactory, Error,
			       TEXT("ImportAnimations : args Skeleton is null ...")
			);
		}
		return NULL;
	}

	{
		FString SequenceName = Name;


		SequenceName += "_";
		//SequenceName += ANSI_TO_TCHAR(CurAnimStack->GetName());
		SequenceName += Skeleton->GetName();

		UE_LOG(LogMMD4UE4_VMDFactory, Error, TEXT("SequenceName!!%s "), *SequenceName);
		// See if this sequence already exists.
		SequenceName = ObjectTools::SanitizeObjectName(SequenceName);

		UE_LOG(LogMMD4UE4_VMDFactory, Error, TEXT("SequenceName!!%s "), *SequenceName);
		FString animpath = SkeletalMesh->GetPathName();
		int32 indexs = -1;
		if (animpath.FindLastChar('/', indexs)) {
			animpath = animpath.Left(indexs);
		}
		else {
			return NULL;
		}
		FString ParentPath;
		if (Outer) {
			ParentPath = FString::Printf(TEXT("%s/%s"), *FPackageName::GetLongPackagePath(*Outer->GetName()), *SequenceName);//保留拖入功能
		}
		else {
			ParentPath = FString::Printf(TEXT("%s/%s"), *animpath, *SequenceName);
		}
		UE_LOG(LogMMD4UE4_VMDFactory, Error, TEXT("I!!%s   %s"), *animpath, *SequenceName);
		UObject* ParentPackage = CreatePackage(*ParentPath);
		UObject* Object = LoadObject<UObject>(ParentPackage, *SequenceName, NULL, LOAD_None, NULL);
		UAnimSequence* DestSeq = Cast<UAnimSequence>(Object);
		// if object with same name exists, warn user
		if (Object && !DestSeq)
		{
			//AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Error, LOCTEXT("Error_AssetExist", "Asset with same name exists. Can't overwrite another asset")), FFbxErrors::Generic_SameNameAssetExists);
			//continue; // Move on to next sequence..
			return LastCreatedAnim;
		}

		// If not, create new one now.
		if (!DestSeq)
		{
			DestSeq = NewObject<UAnimSequence>(ParentPackage, *SequenceName, RF_Public | RF_Standalone);

			// Notify the asset registry
			FAssetRegistryModule::AssetCreated(DestSeq);
		}
		else
		{
			DestSeq->ResetAnimation();
		}

		DestSeq->SetSkeleton(Skeleton);

		LastCreatedAnim = DestSeq;
	}
	///////////////////////////////////
	// Create RawCurve -> Track Curve Key
	//////////////////////

	if (LastCreatedAnim)
	{
		bool importSuccessFlag = true;
		/* vmd animation regist*/
		if (!ImportVMDToAnimSequence(LastCreatedAnim, Skeleton, ReNameTable, IKRig, mmdExtend, vmdMotionInfo))
		{
			//TBD::ERR case
			check(false);
			importSuccessFlag = false;
		}
		/* morph animation regist*/
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION < 2
		if (!ImportMorphCurveToAnimSequence(LastCreatedAnim, Skeleton, SkeletalMesh, ReNameTable, vmdMotionInfo))
		{
			//TBD::ERR case
			{
				UE_LOG(LogMMD4UE4_VMDFactory, Error,TEXT("ImportMorphCurveToAnimSequence is false root..."));
			}
			//check(false);
			importSuccessFlag = false;
		}
#endif
		/*Import正常時PreviewMesh更新*/
		if ((importSuccessFlag) && (SkeletalMesh))
		{
			LastCreatedAnim->SetPreviewMesh(SkeletalMesh);
			UE_LOG(LogMMD4UE4_VMDFactory, Log,
			       TEXT("[ImportAnimations] Set PreviewMesh Pointer.")
			);
		}
	}

	/////////////////////////////////////////
	// end process?
	////////////////////////////////////////
	if (LastCreatedAnim)
	{
		/***********************/
		// refresh TrackToskeletonMapIndex
		//LastCreatedAnim->RefreshTrackMapFromAnimTrackNames();
		if (false)
		{
			//LastCreatedAnim->BakeTrackCurvesToRawAnimation();
		}
		else
		{
			// otherwise just compress
			//LastCreatedAnim->PostProcessSequence();
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION < 2
			auto& adc = LastCreatedAnim->GetController();
			adc.OpenBracket(LOCTEXT("ImportAsSkeletalMesh", "Importing VMD Animation"));

			adc.UpdateCurveNamesFromSkeleton(Skeleton, ERawCurveTrackTypes::RCT_Float);
			adc.NotifyPopulated();

			adc.CloseBracket();
#endif
			// mark package as dirty
			MarkPackageDirty();
			SkeletalMesh->MarkPackageDirty();


			LastCreatedAnim->PostEditChange();
			LastCreatedAnim->SetPreviewMesh(SkeletalMesh);
			LastCreatedAnim->MarkPackageDirty();

			Skeleton->SetPreviewMesh(SkeletalMesh);
			Skeleton->PostEditChange();
		}
	}

	return LastCreatedAnim;
}

/******************************************************************************
* Start
* copy from: http://d.hatena.ne.jp/edvakf/touch/20111016/1318716097
* x1~y2 : 0 <= xy <= 1 :bezier points
* x : 0<= x <= 1 : frame rate
******************************************************************************/
float UVmdFactory::interpolateBezier(float x1, float y1, float x2, float y2, float x)
{
	float t = 0.5, s = 0.5;
	for (int i = 0; i < 15; i++)
	{
		float ft = (3 * s * s * t * x1) + (3 * s * t * t * x2) + (t * t * t) - x;
		if (ft == 0) break; // Math.abs(ft) < 0.00001 でもいいかも
		if (FGenericPlatformMath::Abs(ft) < 0.0001) break;
		if (ft > 0)
			t -= 1.0 / (float)(4 << i);
		else // ft < 0
			t += 1.0 / (float)(4 << i);
		s = 1 - t;
	}
	return (3 * s * s * t * y1) + (3 * s * t * t * y2) + (t * t * t);
}

/** End **********************************************************************/

/*******************
*将VMD表情数据添加到现有AnimSequ资源的过程
*与MMD4Mecanimu的综合利用测试功能
**********************/
UAnimSequence* UVmdFactory::AddtionalMorphCurveImportToAnimations(
	USkeletalMesh* SkeletalMesh,
	UAnimSequence* exsistAnimSequ,
	UDataTable* ReNameTable,
	MMD4UE4::VmdMotionInfo* vmdMotionInfo
)
{
	USkeleton* Skeleton = NULL;
	// we need skeleton to create animsequence
	if (exsistAnimSequ == NULL)
	{
		return NULL;
	}
	{
		//TDB::if exsite assets need fucn?
		//exsistAnimSequ->RecycleAnimSequence();

		Skeleton = exsistAnimSequ->GetSkeleton();
	}
	///////////////////////////////////
	// Create RawCurve -> Track Curve Key
	//////////////////////
	if (exsistAnimSequ)
	{
		//exsistAnimSequ->NumFrames = vmdMotionInfo->maxFrame;
		//exsistAnimSequ->SequenceLength = FGenericPlatformMath::Max<float>(1.0f / 30.0f*(float)exsistAnimSequ->NumFrames, MINIMUM_ANIMATION_LENGTH);
		/////////////////////////////////
		if (!ImportMorphCurveToAnimSequence(
				exsistAnimSequ,
				Skeleton,
				SkeletalMesh,
				ReNameTable,
				vmdMotionInfo)
		)
		{
			//TBD::ERR case
			check(false);
		}
	}

	/////////////////////////////////////////
	// end process?
	////////////////////////////////////////
	if (exsistAnimSequ)
	{
		bool existAsset = true;
		/***********************/
		// refresh TrackToskeletonMapIndex
		//exsistAnimSequ->RefreshTrackMapFromAnimTrackNames();
		if (existAsset)
		{
			//exsistAnimSequ->BakeTrackCurvesToRawAnimation();
		}
		else
		{
			// otherwise just compress
			//exsistAnimSequ->PostProcessSequence();
			
			auto& adc = exsistAnimSequ->GetController();
			adc.OpenBracket(LOCTEXT("ImportAsSkeletalMesh", "Importing VMD Animation"));

			adc.UpdateCurveNamesFromSkeleton(Skeleton, ERawCurveTrackTypes::RCT_Float);
			adc.NotifyPopulated();

			adc.CloseBracket();
			
			// mark package as dirty
			MarkPackageDirty();
			SkeletalMesh->MarkPackageDirty();


			exsistAnimSequ->PostEditChange();
			exsistAnimSequ->SetPreviewMesh(SkeletalMesh);
			exsistAnimSequ->MarkPackageDirty();

			Skeleton->SetPreviewMesh(SkeletalMesh);
			Skeleton->PostEditChange();
		}
	}

	return exsistAnimSequ;
}
/*******************
*导入Morph目标AnimCurve
*将Morphtarget FloatCurve从VMD文件数据导入AnimSeq
**********************/
bool UVmdFactory::ImportMorphCurveToAnimSequence(
	UAnimSequence* DestSeq,
	USkeleton* Skeleton,
	USkeletalMesh* SkeletalMesh,
	UDataTable* ReNameTable,
	MMD4UE4::VmdMotionInfo* vmdMotionInfo
)
{
	if (!DestSeq || !Skeleton || !vmdMotionInfo)
	{
		//TBD:: ERR in Param...
		return false;
	}
	//USkeletalMesh * mesh = Skeleton->GetAssetPreviewMesh(DestSeq);// GetPreviewMesh();
	USkeletalMesh* mesh = SkeletalMesh;
	if (!mesh)
	{
		//如果进入该路径的条件在Skeleton Asset生成后一次也没有打开资源
		//NULL的样子。比起使用这个函数，还是考虑别的手段比较好…。需要调查的范围。
		//TDB:ERR。previewMesh is Null
		{
			UE_LOG(LogMMD4UE4_VMDFactory, Error,
				TEXT("ImportMorphCurveToAnimSequence GetAssetPreviewMesh Not Found...")
			);
		}
		return false;
	}
	/* morph animation regist*/

	auto& adc = DestSeq->GetController();
	//adc.OpenBracket(LOCTEXT("AddNewRawTrack_Bracket", "Adding new Morph Animation Track"));
	for (int i = 0; i < vmdMotionInfo->keyFaceList.Num(); ++i)
	{
		MMD4UE4::VmdFaceTrackList* vmdFaceTrackPtr = &vmdMotionInfo->keyFaceList[i];

		//original
		FName Name = *vmdFaceTrackPtr->TrackName;

		if (ReNameTable)
		{
			//如果指定了转换表的资源，则从表中获取转换名称
			FMMD2UE4NameTableRow* dataRow;
			FString ContextString;
			dataRow = ReNameTable->FindRow<FMMD2UE4NameTableRow>(Name, ContextString);
			if (dataRow)
			{
				Name = FName(*dataRow->MmdOriginalName);
			}
		}
		//self
		if (mesh != NULL)
		{
			UMorphTarget* morphTargetPtr = mesh->FindMorphTarget(Name);
			if (!morphTargetPtr)
			{
				//TDB::ERR. not found Morph Target(Name) in mesh
				{
					UE_LOG(LogMMD4UE4_VMDFactory, Warning,
						TEXT("ImportMorphCurveToAnimSequence Target Morph Not Found...Search[%s]VMD-Org[%s]"),
						*Name.ToString(), *vmdFaceTrackPtr->TrackName);
				}
			}
			else {

				if (vmdFaceTrackPtr->keyList.Num() > 1) {

					Skeleton->AddCurveMetaData(Name);
					FAnimationCurveIdentifier curve2;
					curve2.CurveType = ERawCurveTrackTypes::RCT_Float;
					TArray<FRichCurveKey> keyarrys;

					if (adc.AddCurve(curve2)) {

						MMD4UE4::VMD_FACE_KEY* faceKeyPtr = NULL;
						for (int s = 0; s < vmdFaceTrackPtr->keyList.Num(); ++s)
						{
							check(vmdFaceTrackPtr->sortIndexList[s] < vmdFaceTrackPtr->keyList.Num());
							faceKeyPtr = &vmdFaceTrackPtr->keyList[vmdFaceTrackPtr->sortIndexList[s]];
							check(faceKeyPtr);
							float SequenceLength;
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 1
							SequenceLength = DestSeq->GetPlayLength();
#else
							SequenceLength = DestSeq->SequenceLength;
#endif
							float timeCurve = faceKeyPtr->Frame / 30.0f;
							if (timeCurve > SequenceLength)
							{
								//this key frame(time) more than Target SeqLength ... 
								break;
							}
							keyarrys.Add(FRichCurveKey(timeCurve, faceKeyPtr->Factor));

						}
						adc.SetCurveKeys(curve2, keyarrys);

						UE_LOG(LogMMD4UE4_VMDFactory, Log, TEXT("!!success[%s]VMD-Org[%d]"), *Name.ToString(), vmdFaceTrackPtr->keyList.Num());
					}
					else {
						UE_LOG(LogMMD4UE4_VMDFactory, Warning, TEXT("!!failed[%s]VMD-Org[%d]"), *Name.ToString(), vmdFaceTrackPtr->keyList.Num());
					}

				}

			}
		}
		
		
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 1
		DestSeq->Modify();	
#else
		DestSeq->MarkRawDataAsModified();
#endif
		

	}
	return true;
}



/*******************
* Import VMD Animation
* 从VMD文件的数据将运动数据导入AnimSeq
**********************/
bool UVmdFactory::ImportVMDToAnimSequence(
	UAnimSequence* DestSeq,
	USkeleton* Skeleton,
	UDataTable* ReNameTable,
	UIKRigDefinition* IKRig,
	UMMDExtendAsset* mmdExtend,
	MMD4UE4::VmdMotionInfo* vmdMotionInfo
)
{
	// nullptr check in-param
	if (!DestSeq || !Skeleton || !vmdMotionInfo)
	{
		UE_LOG(LogMMD4UE4_VMDFactory, Error,
		       TEXT("ImportVMDToAnimSequence : Ref InParam is Null. DestSeq[%x],Skelton[%x],vmdMotionInfo[%x]"),
		       DestSeq, Skeleton, vmdMotionInfo);
		//TBD:: ERR in Param...
		return false;
	}
	if (!ReNameTable)
	{
		UE_LOG(LogMMD4UE4_VMDFactory, Warning,
		       TEXT("ImportVMDToAnimSequence : Target ReNameTable is null."));
	}
	if (!mmdExtend)
	{
		UE_LOG(LogMMD4UE4_VMDFactory, Warning,
		       TEXT("ImportVMDToAnimSequence : Target MMDExtendAsset is null."));
	}

	float ResampleRate = 30.f;

	auto& adc = DestSeq->GetController();
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 1
	adc.InitializeModel();
	//adc.OpenBracket(LOCTEXT("AddNewRawTrack_Bracket", "Adding new Bone Animation Track"));
#endif
	

	const FFrameRate ResampleFrameRate(ResampleRate, 1);
	adc.SetFrameRate(ResampleFrameRate);
	
	
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 1

	const FFrameNumber NumberOfFrames = FGenericPlatformMath::Max<int32>((int32)vmdMotionInfo->maxFrame, 1);
	adc.SetNumberOfFrames(NumberOfFrames.Value,false);
#else
	adc.NotifyPopulated();
	adc.SetPlayLength(FGenericPlatformMath::Max<float>(1.0f / 30.0f * (float)vmdMotionInfo->maxFrame, MINIMUM_ANIMATION_LENGTH));
#endif
	const int32 NumBones = Skeleton->GetReferenceSkeleton().GetNum();

	const TArray<FTransform>& RefBonePose = Skeleton->GetReferenceSkeleton().GetRefBonePose();

	TArray<FRawAnimSequenceTrack> TempRawTrackList;

	check(RefBonePose.Num() == NumBones);
	// 注册与Skeleton的Bone关系@必要事项
	for (int32 BoneIndex = 0; BoneIndex < NumBones; ++BoneIndex)
	{
		TempRawTrackList.Add(FRawAnimSequenceTrack());
		check(BoneIndex == TempRawTrackList.Num() - 1);
		FRawAnimSequenceTrack& RawTrack = TempRawTrackList[BoneIndex];

		auto refTranslation = RefBonePose[BoneIndex].GetTranslation();

		FName targetName = Skeleton->GetReferenceSkeleton().GetBoneName(BoneIndex);;
		FName* pn = NameMap.Find(targetName);
		if (pn)
			targetName = *pn;

		if (ReNameTable)
		{
			//如果指定了转换表的资源，则从表中获取转换名称
			FMMD2UE4NameTableRow* dataRow;
			FString ContextString;
			dataRow = ReNameTable->FindRow<FMMD2UE4NameTableRow>(targetName, ContextString);
			if (dataRow)
			{
				targetName = FName(*dataRow->MmdOriginalName);
			}
		}

		//UE_LOG(LogTemp, Warning, TEXT("%s"),*targetName.ToString());
		int vmdKeyListIndex = vmdMotionInfo->FindKeyTrackName(targetName.ToString(),
		                                                      MMD4UE4::VmdMotionInfo::EVMD_KEYBONE);
		if (vmdKeyListIndex == -1)
		{
			{
				UE_LOG(LogMMD4UE4_VMDFactory, Warning,
				       TEXT("ImportVMDToAnimSequence Target Bone Not Found...[%s]"),
				       *targetName.ToString());
			}
			//nop
			//设定与帧相同的值
			for (int32 i = 0; i < DestSeq->GetNumberOfSampledKeys(); i++)
			{
				FTransform nrmTrnc;
				nrmTrnc.SetIdentity();
				RawTrack.PosKeys.Add(FVector3f(nrmTrnc.GetTranslation() + refTranslation));
				RawTrack.RotKeys.Add(FQuat4f(nrmTrnc.GetRotation()));
				RawTrack.ScaleKeys.Add(FVector3f(nrmTrnc.GetScale3D()));
			}
		}
		else
		{
			check(vmdKeyListIndex > -1);
			int sortIndex = 0;
			int preKeyIndex = -1;
			auto& kybone = vmdMotionInfo->keyBoneList[vmdKeyListIndex];
			//if (kybone.keyList.Num() < 2)					continue;
			int nextKeyIndex = kybone.sortIndexList[sortIndex];
			int nextKeyFrame = kybone.keyList[nextKeyIndex].Frame;
			int baseKeyFrame = 0;

			{
				UE_LOG(LogMMD4UE4_VMDFactory, Log,
				       TEXT("ImportVMDToAnimSequence Target Bone Found...Name[%s]-KeyNum[%d]"),
				       *targetName.ToString(),
				       kybone.sortIndexList.Num());
			}
			bool dbg = false;
			if (targetName == L"右ひじ")
				dbg = true;
			//事先针对各轨迹，在没有父Bone的情况下，在Local坐标下计算预定全部注册的帧（如果有更好的处理……讨论）
			//如果进入90度以上的轴旋转，则由于四元数的原因或处理有错误而进入多余的旋转。
			//通过上述方式，仅通过Z旋转（旋转运动），下半身和上半身的轴成为物理上不可能的旋转的组合。臭虫。

			if (targetName == L"右足ＩＫ")
			{
				UE_LOG(LogMMD4UE4_VMDFactory, Log, TEXT("右足ＩＫ"));
			}
			if (targetName == L"左足ＩＫ")
			{
				UE_LOG(LogMMD4UE4_VMDFactory, Log, TEXT("左足ＩＫ"));
			}


			for (int32 i = 0; i < DestSeq->GetNumberOfSampledKeys(); i++)
			{
				if (i == 0)
				{
					if (i == nextKeyFrame)
					{
						FTransform tempTranceform(
							FQuat(
								kybone.keyList[nextKeyIndex].Quaternion[0],
								kybone.keyList[nextKeyIndex].Quaternion[2] * (-1),
								kybone.keyList[nextKeyIndex].Quaternion[1],
								kybone.keyList[nextKeyIndex].Quaternion[3]
							),
							FVector(
								kybone.keyList[nextKeyIndex].Position[0],
								kybone.keyList[nextKeyIndex].Position[2] * (-1),
								kybone.keyList[nextKeyIndex].Position[1]
							) * 10.0f,
							FVector(1, 1, 1)
						);
						//将从引用姿势移动了Key的姿势的值作为初始值
						RawTrack.PosKeys.Add(FVector3f(tempTranceform.GetTranslation() + refTranslation));
						RawTrack.RotKeys.Add(FQuat4f(tempTranceform.GetRotation()));
						RawTrack.ScaleKeys.Add(FVector3f(tempTranceform.GetScale3D()));

						preKeyIndex = nextKeyIndex;
						uint32 lastKF = nextKeyFrame;
						while (sortIndex + 1 < kybone.sortIndexList.Num() && kybone.keyList[nextKeyIndex].Frame <=
							lastKF)
						{
							sortIndex++;
							nextKeyIndex = kybone.sortIndexList[sortIndex];
						}
						lastKF = nextKeyFrame = kybone.keyList[nextKeyIndex].Frame;

						while (sortIndex + 1 < kybone.sortIndexList.Num() && kybone.keyList[kybone.sortIndexList[
							sortIndex + 1]].Frame == lastKF)
						{
							sortIndex++;
							nextKeyIndex = kybone.sortIndexList[sortIndex];
						}
						nextKeyFrame = kybone.keyList[nextKeyIndex].Frame;
					}
					else
					{
						preKeyIndex = nextKeyIndex;
						//例外处理。未为初始帧（0）设置关键帧
						FTransform nrmTrnc;
						nrmTrnc.SetIdentity();
						RawTrack.PosKeys.Add(FVector3f(nrmTrnc.GetTranslation() + refTranslation));
						RawTrack.RotKeys.Add(FQuat4f(nrmTrnc.GetRotation()));
						RawTrack.ScaleKeys.Add(FVector3f(nrmTrnc.GetScale3D()));
					}
				}
				else //if (nextKeyFrame == i)
				{
					float blendRate = 1;
					FTransform NextTranc;
					FTransform PreTranc;
					FTransform NowTranc;

					NextTranc.SetIdentity();
					PreTranc.SetIdentity();
					NowTranc.SetIdentity();

					if (nextKeyIndex > 0)
					{
						MMD4UE4::VMD_KEY& PreKey = kybone.keyList[preKeyIndex];
						MMD4UE4::VMD_KEY& NextKey = kybone.keyList[nextKeyIndex];
						if (NextKey.Frame <= (uint32)i)
						{
							blendRate = 1.0f;
						}
						else
						{
							//TBD:：帧间为1的话不以0.5计算吗？
							blendRate = 1.0f - (float)(NextKey.Frame - (uint32)i) / (float)(NextKey.Frame - PreKey.
								Frame);
						}
						//pose
						NextTranc.SetLocation(
							FVector(
								NextKey.Position[0],
								NextKey.Position[2] * (-1),
								NextKey.Position[1]
							));
						PreTranc.SetLocation(
							FVector(
								PreKey.Position[0],
								PreKey.Position[2] * (-1),
								PreKey.Position[1]
							));

						NowTranc.SetLocation(
							FVector(
								interpolateBezier(
									NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_X][
										D_VMD_KEY_BEZIER_AR_2_KND_X] / 127.0f,
									NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][
										D_VMD_KEY_BEZIER_AR_2_KND_X] / 127.0f,
									NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_X][
										D_VMD_KEY_BEZIER_AR_2_KND_X] / 127.0f,
									NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][
										D_VMD_KEY_BEZIER_AR_2_KND_X] / 127.0f,
									blendRate
								) * (NextTranc.GetTranslation().X - PreTranc.GetTranslation().X) + PreTranc.
								GetTranslation().X
								,
								interpolateBezier(
									NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_X][
										D_VMD_KEY_BEZIER_AR_2_KND_Z] / 127.0f,
									NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][
										D_VMD_KEY_BEZIER_AR_2_KND_Z] / 127.0f,
									NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_X][
										D_VMD_KEY_BEZIER_AR_2_KND_Z] / 127.0f,
									NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][
										D_VMD_KEY_BEZIER_AR_2_KND_Z] / 127.0f,
									blendRate
								) * (NextTranc.GetTranslation().Y - PreTranc.GetTranslation().Y) + PreTranc.
								GetTranslation().Y
								,
								interpolateBezier(
									NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_X][
										D_VMD_KEY_BEZIER_AR_2_KND_Y] / 127.0f,
									NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][
										D_VMD_KEY_BEZIER_AR_2_KND_Y] / 127.0f,
									NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_X][
										D_VMD_KEY_BEZIER_AR_2_KND_Y] / 127.0f,
									NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][
										D_VMD_KEY_BEZIER_AR_2_KND_Y] / 127.0f,
									blendRate
								) * (NextTranc.GetTranslation().Z - PreTranc.GetTranslation().Z) + PreTranc.
								GetTranslation().Z
							)
						);
						//rot
						NextTranc.SetRotation(
							FQuat(
								NextKey.Quaternion[0],
								NextKey.Quaternion[2] * (-1),
								NextKey.Quaternion[1],
								NextKey.Quaternion[3]
							));
						PreTranc.SetRotation(
							FQuat(
								PreKey.Quaternion[0],
								PreKey.Quaternion[2] * (-1),
								PreKey.Quaternion[1],
								PreKey.Quaternion[3]
							));

						float bezirT = interpolateBezier(
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_X][
								D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][
								D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_X][
								D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][
								D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							blendRate
						);
						NowTranc.SetRotation(
							FQuat::Slerp(PreTranc.GetRotation(), NextTranc.GetRotation(), bezirT)
						);
						/*UE_LOG(LogMMD4UE4_VMDFactory, Warning,
							TEXT("interpolateBezier Rot:[%s],F[%d/%d],BLD[%.2f],biz[%.2f]BEZ[%s]"),
							*targetName.ToString(), i, NextKey.Frame, blendRate, bezirT,*NowTranc.GetRotation().ToString()
							);*/
					}
					else
					{
						NowTranc.SetLocation(
							FVector(
								kybone.keyList[nextKeyIndex].Position[0],
								kybone.keyList[nextKeyIndex].Position[2] * (-1),
								kybone.keyList[nextKeyIndex].Position[1]
							));
						NowTranc.SetRotation(
							FQuat(
								kybone.keyList[nextKeyIndex].Quaternion[0],
								kybone.keyList[nextKeyIndex].Quaternion[2] * (-1),
								kybone.keyList[nextKeyIndex].Quaternion[1],
								kybone.keyList[nextKeyIndex].Quaternion[3]
							));
						//TBD:需要重新研究该路线存在的花纹、处理
						//check(false);
					}

					FTransform tempTranceform(
						NowTranc.GetRotation(),
						NowTranc.GetTranslation() * 10.0f,
						FVector(1, 1, 1)
					);
					//将从引用姿势移动了Key的姿势的值作为初始值
					RawTrack.PosKeys.Add(FVector3f(tempTranceform.GetTranslation() + refTranslation));
					RawTrack.RotKeys.Add(FQuat4f(tempTranceform.GetRotation()));
					RawTrack.ScaleKeys.Add(FVector3f(tempTranceform.GetScale3D()));

					if (nextKeyFrame == i)
					{

						preKeyIndex = nextKeyIndex;
						uint32 lastKF = nextKeyFrame;
						while (sortIndex + 1 < kybone.sortIndexList.Num() && kybone.keyList[nextKeyIndex].Frame <=
							lastKF)
						{
							sortIndex++;
							nextKeyIndex = kybone.sortIndexList[sortIndex];
						}
						lastKF = nextKeyFrame = kybone.keyList[nextKeyIndex].Frame;

						while (sortIndex + 1 < kybone.sortIndexList.Num() && kybone.keyList[kybone.sortIndexList[
							sortIndex + 1]].Frame == lastKF)
						{
							sortIndex++;
							nextKeyIndex = kybone.sortIndexList[sortIndex];
						}
						nextKeyFrame = kybone.keyList[nextKeyIndex].Frame;
					}
				}
			}
		}
	}
	adc.OpenBracket(LOCTEXT("AddNewRawTrack_Bracket", "Adding new Bone Animation Track"));
	/* AddTrack */
	for (int32 BoneIndex = 0; BoneIndex < NumBones; ++BoneIndex)
	{
		FName BoneName = Skeleton->GetReferenceSkeleton().GetBoneName(BoneIndex);

		FRawAnimSequenceTrack& RawTrack = TempRawTrackList[BoneIndex];



		//DestSeq->AddNewRawTrack(BoneName, &RawTrack);

		int32 NewTrackIndex = INDEX_NONE;
		if (RawTrack.PosKeys.Num() > 1)
		{
			bool is_sucess = false;//写法改变

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 1
			is_sucess = adc.AddBoneCurve(BoneName);
#else
			if (adc.AddBoneTrack(BoneName) != INDEX_NONE) is_sucess = true;
#endif

			if (is_sucess)
			{
				if (BoneName == L"腰") {
					for (int32 ix = 0; ix < RawTrack.PosKeys.Num(); ix++) {
						//RawTrack.PosKeys[ix] = FVector3f(0.0f,0.0f,0.0f);
						RawTrack.PosKeys[ix].X = 0.0f;
						RawTrack.PosKeys[ix].Y = 0.0f;
						RawTrack.RotKeys[ix] = FQuat4f(0.0f, 0.0f, 0.0f,1.0f);
					}
					//UE_LOG(LogTemp, Warning, TEXT("%f,%f,%f"), RawTrack.PosKeys[1].X, RawTrack.PosKeys[1].Y, RawTrack.PosKeys[1].Z);//看看有多少个
				}


				adc.SetBoneTrackKeys(BoneName, RawTrack.PosKeys, RawTrack.RotKeys, RawTrack.ScaleKeys);
				
			}
		}
	}
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 1
	for (int i = 0; i < vmdMotionInfo->keyFaceList.Num(); ++i)
	{
		MMD4UE4::VmdFaceTrackList* vmdFaceTrackPtr = &vmdMotionInfo->keyFaceList[i];

		//original
		FName Name = *vmdFaceTrackPtr->TrackName;

		if (ReNameTable)
		{
			//如果指定了转换表的资源，则从表中获取转换名称
			FMMD2UE4NameTableRow* dataRow;
			FString ContextString;
			dataRow = ReNameTable->FindRow<FMMD2UE4NameTableRow>(Name, ContextString);
			if (dataRow)
			{
				Name = FName(*dataRow->MmdOriginalName);
			}
		}
		//self
		USkeletalMesh* mesh = Skeleton->GetPreviewMesh();
		if (mesh != NULL)
		{

			UMorphTarget* morphTargetPtr = mesh->FindMorphTarget(Name);
			if (!morphTargetPtr)
			{
				//TDB::ERR. not found Morph Target(Name) in mesh
				{
					UE_LOG(LogMMD4UE4_VMDFactory, Warning,
						TEXT("ImportMorphCurveToAnimSequence Target Morph Not Found...Search[%s]VMD-Org[%s]"),
						*Name.ToString(), *vmdFaceTrackPtr->TrackName);
				}
			}
			else {

				if (vmdFaceTrackPtr->keyList.Num() > 1) {

					Skeleton->AddCurveMetaData(Name);
					FAnimationCurveIdentifier curve2;
					curve2.CurveType = ERawCurveTrackTypes::RCT_Float;
					TArray<FRichCurveKey> keyarrys;

					if (adc.AddCurve(curve2)) {

						MMD4UE4::VMD_FACE_KEY* faceKeyPtr = NULL;
						for (int s = 0; s < vmdFaceTrackPtr->keyList.Num(); ++s)
						{
							check(vmdFaceTrackPtr->sortIndexList[s] < vmdFaceTrackPtr->keyList.Num());
							faceKeyPtr = &vmdFaceTrackPtr->keyList[vmdFaceTrackPtr->sortIndexList[s]];
							check(faceKeyPtr);
							float SequenceLength;
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 1
							SequenceLength = DestSeq->GetPlayLength();
#else
							SequenceLength = DestSeq->SequenceLength;
#endif
							float timeCurve = faceKeyPtr->Frame / 30.0f;
							if (timeCurve > SequenceLength)
							{
								//this key frame(time) more than Target SeqLength ... 
								break;
							}
							keyarrys.Add(FRichCurveKey(timeCurve, faceKeyPtr->Factor));

						}
						adc.SetCurveKeys(curve2, keyarrys);

						UE_LOG(LogMMD4UE4_VMDFactory, Log, TEXT("!!success[%s]VMD-Org[%d]"), *Name.ToString(), vmdFaceTrackPtr->keyList.Num());
					}
					else {
						UE_LOG(LogMMD4UE4_VMDFactory, Warning, TEXT("!!failed[%s]VMD-Org[%d]"), *Name.ToString(), vmdFaceTrackPtr->keyList.Num());
					}

				}

			}
		}


#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 1
		DestSeq->Modify();
#else
		DestSeq->MarkRawDataAsModified();
#endif


	}
	adc.UpdateCurveNamesFromSkeleton(Skeleton, ERawCurveTrackTypes::RCT_Float);
	adc.NotifyPopulated();
#else
	adc.NotifyPopulated();
#endif
	
	adc.CloseBracket();
	//GWarn->EndSlowTask();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
// 创建以X轴为中心的旋转矩阵
void CreateRotationXMatrix(FMatrix* Out, float Angle)
{
	float Sin, Cos;

	//_SINCOS(Angle, &Sin, &Cos);
	//	Sin = sinf( Angle ) ;
	//	Cos = cosf( Angle ) ;
	Sin = FMath::Sin(Angle);
	Cos = FMath::Cos(Angle);

	//_MEMSET(Out, 0, sizeof(MATRIX));
	FMemory::Memzero(Out, sizeof(FMatrix));
	Out->M[0][0] = 1.0f;
	Out->M[1][1] = Cos;
	Out->M[1][2] = Sin;
	Out->M[2][1] = -Sin;
	Out->M[2][2] = Cos;
	Out->M[3][3] = 1.0f;

	//return 0;
}

// 求仅旋转分量矩阵的积（3）×3以外的部分也不代入值）
void MV1LoadModelToVMD_CreateMultiplyMatrixRotOnly(FMatrix* Out, FMatrix* In1, FMatrix* In2)
{
	Out->M[0][0] = In1->M[0][0] * In2->M[0][0] + In1->M[0][1] * In2->M[1][0] + In1->M[0][2] * In2->M[2][0];
	Out->M[0][1] = In1->M[0][0] * In2->M[0][1] + In1->M[0][1] * In2->M[1][1] + In1->M[0][2] * In2->M[2][1];
	Out->M[0][2] = In1->M[0][0] * In2->M[0][2] + In1->M[0][1] * In2->M[1][2] + In1->M[0][2] * In2->M[2][2];

	Out->M[1][0] = In1->M[1][0] * In2->M[0][0] + In1->M[1][1] * In2->M[1][0] + In1->M[1][2] * In2->M[2][0];
	Out->M[1][1] = In1->M[1][0] * In2->M[0][1] + In1->M[1][1] * In2->M[1][1] + In1->M[1][2] * In2->M[2][1];
	Out->M[1][2] = In1->M[1][0] * In2->M[0][2] + In1->M[1][1] * In2->M[1][2] + In1->M[1][2] * In2->M[2][2];

	Out->M[2][0] = In1->M[2][0] * In2->M[0][0] + In1->M[2][1] * In2->M[1][0] + In1->M[2][2] * In2->M[2][0];
	Out->M[2][1] = In1->M[2][0] * In2->M[0][1] + In1->M[2][1] * In2->M[1][1] + In1->M[2][2] * In2->M[2][1];
	Out->M[2][2] = In1->M[2][0] * In2->M[0][2] + In1->M[2][1] * In2->M[1][2] + In1->M[2][2] * In2->M[2][2];
}

/////////////////////////////////////
// 判定角度限制的共同函数（subIndexJdg的判定比较不明…）
void CheckLimitAngle(
	const FVector& RotMin,
	const FVector& RotMax,
	FVector* outAngle, //target angle ( in and out param)
	bool subIndexJdg //(ik link index < ik loop temp):: linkBoneIndex < ikt
)
{
	//#define DEBUG_CheckLimitAngle
#ifdef DEBUG_CheckLimitAngle
	FVector debugVec = *outAngle;
#endif

	*outAngle = ClampVector(
		*outAngle,
		RotMin,
		RotMax
	);

	//debug
#ifdef DEBUG_CheckLimitAngle
	UE_LOG(LogMMD4UE4_VMDFactory, Log,
		TEXT("CheckLimitAngle::out[%s]<-In[%s]:MI[%s]MX[%s]"),
		*outAngle->ToString(),
		*debugVec.ToString(),
		*RotMin.ToString(),
		*RotMax.ToString()
		);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////

/*****************
* 从MMD侧的名称检索并取得TableRow的UE侧名称
* Return :T is Found
* @param :ue4Name is Found Row Name
****************/
bool UVmdFactory::FindTableRowMMD2UEName(
	UDataTable* ReNameTable,
	FName mmdName,
	FName* ue4Name
)
{
	if (ReNameTable == NULL || ue4Name == NULL)
	{
		return false;
	}

	TArray<FName> getTableNames = ReNameTable->GetRowNames();

	FMMD2UE4NameTableRow* dataRow;
	FString ContextString;
	for (int i = 0; i < getTableNames.Num(); ++i)
	{
		ContextString = "";
		dataRow = ReNameTable->FindRow<FMMD2UE4NameTableRow>(getTableNames[i], ContextString);
		if (dataRow)
		{
			if (mmdName == FName(*dataRow->MmdOriginalName))
			{
				*ue4Name = getTableNames[i];
				return true;
			}
		}
	}
	return false;
}

/*****************
* 从Bone名称中搜索并获取与RefSkelton匹配的BoneIndex
* Return :index, -1 is not found
* @param :TargetName is Target Bone Name
****************/
int32 UVmdFactory::FindRefBoneInfoIndexFromBoneName(
	const FReferenceSkeleton& RefSkelton,
	const FName& TargetName
)
{
	for (int i = 0; i < RefSkelton.GetRefBoneInfo().Num(); ++i)
	{
		if (RefSkelton.GetRefBoneInfo()[i].Name == TargetName)
		{
			return i;
		}
	}
	return -1;
}


/*****************
* 递归计算当前关键帧中指定Bone的Glb坐标
* Return :trncform
* @param :TargetName is Target Bone Name
****************/
FTransform UVmdFactory::CalcGlbTransformFromBoneIndex(
	UAnimSequence* DestSeq,
	USkeleton* Skeleton,
	int32 BoneIndex,
	int32 keyIndex
)
{
	if (DestSeq == NULL || Skeleton == NULL || BoneIndex < 0 || keyIndex < 0)
	{
		//error root
		return FTransform::Identity;
	}

	auto& dat = DestSeq->GetDataModel()->GetBoneAnimationTracks()[BoneIndex].InternalTrackData;

	FTransform resultTrans(
		FQuat(dat.RotKeys[keyIndex]), // qt.X, qt.Y, qt.Z, qt.W),
		FVector(dat.PosKeys[keyIndex]),
		FVector(dat.ScaleKeys[keyIndex])
	);

	int ParentBoneIndex = Skeleton->GetReferenceSkeleton().GetParentIndex(BoneIndex);
	if (ParentBoneIndex >= 0)
	{
		//found parent bone
		resultTrans *= CalcGlbTransformFromBoneIndex(
			DestSeq,
			Skeleton,
			ParentBoneIndex,
			keyIndex
		);
	}
	return resultTrans;
}
bool UVmdFactory::ImportVmdFromFile(FString file, USkeletalMesh* SkeletalMesh) {
	initMmdNameMap();
	MMD4UE4::VmdMotionInfo vmdMotionInfo;

	UVmdFactory* MyFactory = NewObject<UVmdFactory>();
	TArray<uint8> File_Result;

	FString filepath = file;
	int32 indexs = -1;
	if (filepath.FindLastChar('\\', indexs)) {
		filepath = filepath.Right(filepath.Len() - indexs - 1);
		if (filepath.FindLastChar('.', indexs)) {
			filepath = filepath.Left(indexs);
			if (FPaths::FileExists(file)) {

				if (FFileHelper::LoadFileToArray(File_Result, *file)) {
					const uint8* DataPtr = File_Result.GetData();
					if (vmdMotionInfo.VMDLoaderBinary(DataPtr,NULL) == false)
					{
						UE_LOG(LogMMD4UE4_VMDFactory, Error, TEXT("!!!VMD Import error::vmd data load faile."));
						return false;
					}
					UAnimSequence* LastCreatedAnim = NULL;
					USkeleton* Skeleton = NULL;
					//UIKRigDefinition* IKRig = NULL;
					VMDImportOptions* ImportOptions = NULL;
					UDataTable* MMD2UE4NameTable = NULL;
					//UMMDExtendAsset* MMDasset = NULL;
					if (SkeletalMesh) {
						Skeleton = SkeletalMesh->GetSkeleton();

						LastCreatedAnim = MyFactory->ImportAnimations(
							Skeleton,
							SkeletalMesh,
							NULL,
							filepath,
							NULL,
							MMD2UE4NameTable,
							NULL,
							&vmdMotionInfo
						);
						return true;
					}
					else {
						UE_LOG(LogMMD4UE4_VMDFactory, Error, TEXT("!!!VMD Import error::SkeletalMesh is null."));
					}
				}
				else {
					UE_LOG(LogMMD4UE4_VMDFactory, Error, TEXT("!!!VMD Import error:: LoadFileToArray."));
				}
			}
			else {
				UE_LOG(LogMMD4UE4_VMDFactory, Error, TEXT("!!!VMD Import error:: FIle is not exist."));
			}

		}
		else {
			UE_LOG(LogMMD4UE4_VMDFactory, Error, TEXT("!!!VMD Import error::filepath type error."));

		}
	}
	else {
		UE_LOG(LogMMD4UE4_VMDFactory, Error, TEXT("!!!VMD Import error::filepath error.%d,%s"), indexs, *filepath);
	}

	return false;
}
#undef LOCTEXT_NAMESPACE
