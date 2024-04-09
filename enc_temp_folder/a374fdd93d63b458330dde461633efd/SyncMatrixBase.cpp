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
    UE_LOG(LogTemp, Log, TEXT("SyncMatrix Begin Play"));
    // Find actor of class AMatrixCom (frist encounter)
    MatrixCom = Cast<AMatrixCommunication>(
        UGameplayStatics::GetActorOfClass(this, AMatrixCommunication::StaticClass())
    );

}

void RequestNextAgentUpdate(AMatrixCommunication* MatrixCom)
{
    // send a message to the MatrixCom actor to request the next agent update
    // UE_LOG(LogTemp, Log, TEXT("RequestNextAgentUpdate"));
    // GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "RequestNextAgentUpdate");
    if (MatrixCom)
    {
        FMatrixMsgStruct Msg = FMatrixMsgStruct();

        // 生成UID
        Msg.src = MatrixCom->MatrixComUID;
        Msg.dst = "matrix";
        Msg.command = "update_agents";
        Msg.need_reply = true;
        // UE_LOG(LogTemp, Log, TEXT("MatrixCom->ConvertToJsonAndSendWs(Msg)"));
        MatrixCom->ConvertToJsonAndSendWs(Msg);
        // UE_LOG(LogTemp, Log, TEXT("MatrixCom->ConvertToJsonAndSendWs(Msg) :: Done"));
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


void ASyncMatrixBase::ProcessMatComMessage(FMatrixMsgStruct MatrixMsg)
{

    // do something with the message
    FString cmd = MatrixMsg.command;
    FString arg = MatrixMsg.arg;
    if (cmd == "update_agents.re")
    {
        // parse json dat
        FAgentSummaryArrayStruct AgentSummaryArray;
        TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(arg);
        TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
        FJsonSerializer::Deserialize(JsonReader, JsonObject);
        FJsonObjectConverter::JsonObjectToUStruct<FAgentSummaryArrayStruct>(JsonObject.ToSharedRef(), &AgentSummaryArray, 0, 0);
        // check dat
        for (auto agent_summary : AgentSummaryArray.agent_summary_array)
        {
            // FVector agent_summary.agent_location to FString
            FString agent_location_str = FString::Printf(
                TEXT("%f, %f, %f"),
                agent_summary.agent_location.X,
                agent_summary.agent_location.Y,
                agent_summary.agent_location.Z
            );
            FString printbuf = FString::Printf(
                TEXT("agent_id: %s, agent_location: %s"),
                *agent_summary.agent_id,
                *agent_location_str
            );
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, printbuf);

            if (AgentMap.Contains(agent_summary.agent_id))
            {
                // update agent location
                ABasicAgentCpp* agent = AgentMap.Find(agent_summary.agent_id);
                UpdateAgent(agent, agent_summary);
            }
            else
            {
                ABasicAgentCpp* agent = CreateAgent(agent_summary);
                UpdateAgent(agent, agent_summary);
            }
        }
    }

}

// Called every frame
void ASyncMatrixBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    tick_cnt += 1;

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

    // every two ticks, request the next agent update
    if (tick_cnt % 40 == 0)
    {
        RequestNextAgentUpdate(MatrixCom);
    }

}

void ASyncMatrixBase::UpdateAgent(ABasicAgentCpp* agent, FAgentSummaryStruct agent_summary)
{
    if (agent->AgentActivity != agent_summary.agent_activity)
    {
        agent->AgentActivity = agent_summary.agent_activity;
        agent->OnAgentUpdateActivity(agent_summary.agent_activity);
    }

    if (agent->AgentAnimation != agent_summary.agent_animation)
    {
        agent->AgentAnimation = agent_summary.agent_animation;
        agent->OnAgentUpdateAnimation(agent_summary.agent_animation);
    }

    if (agent->AgentStatus != agent_summary.agent_status)
    {
        agent->AgentStatus = agent_summary.agent_status;
        agent->OnAgentUpdateStatus();
    }
}

ABasicAgentCpp* ASyncMatrixBase::CreateAgent(FAgentSummaryStruct agent_summary)
{
    FVector SpawnLocation = agent_summary.agent_location;
    FRotator SpawnRotation = FRotator::ZeroRotator;
    ABasicAgentCpp* agent = GetWorld()->SpawnActor<ABasicAgentCpp>(SpawnLocation, SpawnRotation);
    agent->SyncMatrix = this;
    return agent;
}
