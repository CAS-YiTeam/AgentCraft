// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BasicAgentCpp.generated.h"

UCLASS()
class AGENTCRAFT_API ABasicAgentCpp : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABasicAgentCpp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};


// Actions to accept

// Easy todo
// - Walk to
// - Show Bubble Text
// - Update Input and Output
// - Update Connection
// - Turn to
// - Receive Build Connection Command & Build Connection

// Parent Agent Input
// Parent Agent Output


// Future todo
// - Talk
// - Run
// - Turn smooth

