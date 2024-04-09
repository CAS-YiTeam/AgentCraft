// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicAgentCpp.h"
#include "SyncMatrixBase.h"

// Sets default values
ABasicAgentCpp::ABasicAgentCpp()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // "/Game/" is the content folder
    ConstructorHelpers::FClassFinder<AActor> BPFinder(
        TEXT("/Game/Dynamic/Agent/Agent_Level_1/CubeAgent/FloatingCube"));
    BPToSpawn = BPFinder.Class;

    // OnMoveActorEvent.AddDynamic(this, &ABasicAgentCpp::CubeDanceDemo);

}

// Called when the game starts or when spawned
void ABasicAgentCpp::BeginPlay()
{
	Super::BeginPlay();
    

}

// Called every frame
void ABasicAgentCpp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    OnAgentUpdateStatus();
    AgentAliveTime += DeltaTime;
}

// Called to bind functionality to input
void ABasicAgentCpp::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABasicAgentCpp::OnAgentUpdateStatus_Implementation()
{
    // CubeDanceDemo(0.01);

}

void ABasicAgentCpp::OnAgentUpdateAnimation_Implementation()
{

}

void ABasicAgentCpp::OnAgentUpdateActivity_Implementation()
{

}

void ABasicAgentCpp::SendRequestBackToMatrix(FAgentSummaryStruct agent_summary)
{
    if (SyncMatrix)
    {
        SyncMatrix -> AgentSummaryList.Add(agent_summary);
    }
}

void ABasicAgentCpp::CubeDanceDemo(float DeltaTime)
{
    int M = 16;
    if (AgentAliveTime <= 0)
    {
        for (int i = 0; i < M; i++) //
        {
            FTransform tf = GetActorTransform();
            FVector loc = tf.GetLocation();
            FVector scale = tf.GetScale3D();

            // create initial position
            loc.X += 100 * cos((float)i * (360 / M) / 180 * PI);
            loc.Y += 100 * sin((float)i * (360 / M) / 180 * PI);
            scale.X = 0.1;
            scale.Y = 0.1;
            scale.Z = 0.1;
            tf.SetTranslationAndScale3D(loc, scale);

            // spawn actor
            AActor* NewActor = GetWorld()->SpawnActor<AActor>(BPToSpawn, tf);
            NewActor->SetActorTransform(tf);
            // make actors child actor of `this`
            NewActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
            //NewActor->SetHidden(false);

            FTimerHandle MemberTimerHandle2;
            GetWorld()->GetTimerManager().SetTimer(MemberTimerHandle2,
            FTimerDelegate::CreateLambda([M, i, NewActor, MemberTimerHandle2, this] {
                // GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Timer");
                FVector ParentLocaion = GetActorLocation();
                float RoSpeed_H = 180;
                ParentLocaion.X += 100 * cos((float)(i * (360 / M) + AgentAliveTime * RoSpeed_H) / 180 * PI);
                ParentLocaion.Y += 100 * sin((float)(i * (360 / M) + AgentAliveTime * RoSpeed_H) / 180 * PI);
                ParentLocaion.Z += 10 * sin((float)(i * (4*360 / M) + AgentAliveTime * 6* RoSpeed_H) / 180 * PI);
                NewActor->SetActorLocation(ParentLocaion);
            }), DeltaTime, true);
        }
    }
    else
    {

    }

}


