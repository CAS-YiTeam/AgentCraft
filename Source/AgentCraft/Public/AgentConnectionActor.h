// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasicAgentCpp.h"
#include "GameFramework/Actor.h"
#include "AgentConnectionActor.generated.h"

UCLASS()
class AGENTCRAFT_API AAgentConnectionActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAgentConnectionActor();

    UPROPERTY(BlueprintReadWrite)
        ABasicAgentCpp* ConnectFromAgent;

    UPROPERTY(BlueprintReadWrite)
        ABasicAgentCpp* ConnectToAgent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    // How long this agent become active
    float AgentAliveTime = 0;

    // A small demo
    void CubeDanceDemo(float DeltaTime);
    TSubclassOf<AActor> BPToSpawn;
};
