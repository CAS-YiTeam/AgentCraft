#pragma once

#include "CoreMinimal.h"
#include "Containers/UnrealString.h"
#include "MatrixMsg.generated.h"

USTRUCT(BlueprintType)
struct FMatrixMsgStruct
{

	GENERATED_BODY()

	// 必须使用小写字母 + 下划线！
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString src;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString dst;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString command;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString arg;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString kwargs;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool need_reply;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString reserved_field_01;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString reserved_field_02;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString reserved_field_03;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString reserved_field_04;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString reserved_field_05;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString reserved_field_06;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString reserved_field_07;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString reserved_field_08;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool valid = false;
};
