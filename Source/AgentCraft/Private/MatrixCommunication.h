// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IWebSocket.h"
#include "Misc/Guid.h"
#include "Json.h"
#include "JsonObjectConverter.h"
#include "MatrixCommunication.generated.h"

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
};


UCLASS()
class AMatrixCommunication : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMatrixCommunication();
	TSharedPtr<IWebSocket> WebSocket;
	FString WebSocketCurrentUrl = "";
	FString WebSocketConnectionStat = "Waiting";
	FString MatrixComUID;
	FString GetWebSocketAddr();
	void InitWebSocket();
	void TryReconnect();
	void ShutdownWebSocket();
	FMatrixMsgStruct ParsedFMatrixMsgStruct(FString TcpLatestRecvString);
	void ConvertToJsonAndSendWs(FMatrixMsgStruct MatrixMsg);
	void WsSendJson(TSharedPtr<FJsonObject> ReplyJson);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void BeginDestroy() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FString ParseCommandLineArguments();

};
