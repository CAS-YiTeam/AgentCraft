// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IWebSocket.h"
#include "Misc/Guid.h"
#include "Json.h"
#include "MatrixMsg.h"
#include "JsonObjectConverter.h"
#include "MatrixCommunication.generated.h"



UCLASS()
class AMatrixCommunication : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMatrixCommunication();
	TSharedPtr<IWebSocket> WebSocket;

	UPROPERTY(BlueprintReadOnly)
	FString WebSocketCurrentUrl = "";

	UPROPERTY(BlueprintReadOnly)
	FString WebSocketConnectionStat = "Waiting";

    UPROPERTY(BlueprintReadWrite)
    FString ClientID = "";

    UPROPERTY(BlueprintReadOnly)
    FString MatrixComUID;

	UFUNCTION(BlueprintCallable)
	FMatrixMsgStruct PopNextMessageFromQueue();

	UFUNCTION(BlueprintCallable)
	bool IsQueueEmpty();

	FString GetWebSocketAddr();
	void InitWebSocket();
	void TryReconnect();

    TCircularQueue<FMatrixMsgStruct> MsgQueue = TCircularQueue<FMatrixMsgStruct>(10000);
	FMatrixMsgStruct ParsedFMatrixMsgStruct(FString TcpLatestRecvString);

    UFUNCTION(BlueprintCallable)
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
