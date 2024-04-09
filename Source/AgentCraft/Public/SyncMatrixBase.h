// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MatrixCommunication.h"
#include "BasicAgentCpp.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "SyncMatrixBase.generated.h"

UCLASS()
class AGENTCRAFT_API ASyncMatrixBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASyncMatrixBase();
	AMatrixCommunication* MatrixCom = nullptr;
	TMap<FString, ABasicAgentCpp> AgentMap;
    TArray<FAgentSummaryStruct> AgentSummaryList;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void UpdateAgent(ABasicAgentCpp* agent, FAgentSummaryStruct agent_summary);
    ABasicAgentCpp* CreateAgent(FAgentSummaryStruct agent_summary);
	void ProcessMatComMessage(FMatrixMsgStruct MatrixMsg);
    // Called every frame
	virtual void Tick(float DeltaTime) override;

private:
    uint32 tick_cnt = 0;
};
