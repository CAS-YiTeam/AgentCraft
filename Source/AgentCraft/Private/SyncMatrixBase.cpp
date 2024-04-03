// Fill out your copyright notice in the Description page of Project Settings.


#include "SyncMatrixBase.h"

// Sets default values
ASyncMatrixBase::ASyncMatrixBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASyncMatrixBase::BeginPlay()
{
	Super::BeginPlay();
    // Find actor of class AMatrixCom (frist encounter)
    MatrixCom = Cast<AMatrixCommunication>(
        UGameplayStatics::GetActorOfClass(this, AMatrixCommunication::StaticClass())
    );

}







void RequestNextAgentUpdate(AMatrixCommunication* MatrixCom)
{
    // send a message to the MatrixCom actor to request the next agent update
    if (MatrixCom)
    {
        FMatrixMsgStruct Msg = FMatrixMsgStruct();

        // 生成UID
        Msg.src = MatrixCom->MatrixComUID;
        Msg.dst = "matrix";
        Msg.command = "update_agents";
        Msg.need_reply = true;
        MatrixCom->ConvertToJsonAndSendWs(Msg);
    }
}

enum class Option {
    connect_to_matrix,
    update_script,
    update_location,
    play_event,
    parent_play_event,
    create_agent,
    agent_activate,
    duplicate_agent,
    connect_agent,
    disconnect_agent,
};


void ProcessMatComMessage(FMatrixMsgStruct MatrixMsg)
{

    // do something with the message
    FString cmd = MatrixMsg.command;
    if (cmd == "update_script")
    {

    }

}








// Called every frame
void ASyncMatrixBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// check if the MatrixCom actor is valid, if not, return
	if (!MatrixCom)
	{
		return;
	}

	int max_msg_run = 1000;
	for (int i = 0; i < max_msg_run; i++)
	{
		if (MatrixCom->IsQueueEmpty())
		{
			break;
		}
		auto MatrixMsg = MatrixCom->PopNextMessageFromQueue();
		if (MatrixMsg.valid)
		{
			// do something with the message
			ProcessMatComMessage(MatrixMsg);
		}
	}
}
