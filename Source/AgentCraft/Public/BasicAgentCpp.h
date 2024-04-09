// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MatrixMsg.h"
#include "GameFramework/Character.h"
#include "BasicAgentCpp.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoveActorDelegate, float, DeltaTime);

// 在C++中，避免循环头文件依赖（也称为头文件包含死循环）是至关重要的。
// 循环依赖在编译时可能会引发错误，或者至少会引起混乱和未定义的行为。
// 在UE4中，这个问题同样存在。
// 要避免循环依赖，我们通常使用前向声明。
// 前向声明是在你需要引用一个类但又不需要知道其所有详情（例如类成员和方法）的情况下使用的
// 通常在头文件中需要声明指向另外某个类的指针或引用时
class ASyncMatrixBase;

UCLASS()
class AGENTCRAFT_API ABasicAgentCpp : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABasicAgentCpp();
    ASyncMatrixBase* SyncMatrix = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
    virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // 然后，在你的.cpp文件中，你需要添加此函数的实现。
    // 注意这里函数名需要在后面加上_Implementation，这是UE4的约定
    // Create Event
    UFUNCTION(BlueprintNativeEvent, Category = "AgentCraft")
        void OnAgentUpdateStatus(const FString& BpAgentStatus);
    UPROPERTY(BlueprintReadWrite, Category = "AgentCraft")
        FString AgentStatus = "";


    // Create Event
    UFUNCTION(BlueprintNativeEvent, Category = "AgentCraft")
        void OnAgentUpdateAnimation(const FString& BpAgentAnimation);
    UPROPERTY(BlueprintReadWrite, Category = "AgentCraft")
        FString AgentAnimation = "";


    // Create Event
    UFUNCTION(BlueprintNativeEvent, Category = "AgentCraft")
        void OnAgentUpdateActivity(const FString& BpAgentActivity);
    UPROPERTY(BlueprintReadWrite, Category = "AgentCraft")
        FString AgentActivity = "";

    // How long this agent become active
    float AgentAliveTime = 0;

    void SendRequestBackToMatrix(FAgentSummaryStruct agent_summary);

    // A small demo
    UFUNCTION(Category = "AgentCraft")
    void CubeDanceDemo(float DeltaTime);
    TSubclassOf<AActor> BPToSpawn;
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

