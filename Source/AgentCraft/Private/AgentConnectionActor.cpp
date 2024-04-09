// Fill out your copyright notice in the Description page of Project Settings.


#include "AgentConnectionActor.h"

// Sets default values
AAgentConnectionActor::AAgentConnectionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // "/Game" is the content folder
    ConstructorHelpers::FClassFinder<AActor> BPFinder(TEXT("/Game/Dynamic/Agent/Agent_Level_1/CubeAgent/FloatingCube"));
    BPToSpawn = BPFinder.Class;
}

// Called when the game starts or when spawned
void AAgentConnectionActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAgentConnectionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    CubeDanceDemo(DeltaTime);
    AgentAliveTime += DeltaTime;

}


void AAgentConnectionActor::CubeDanceDemo(float DeltaTime)
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
            scale.X = 0.;
            scale.Y = 0.;
            scale.Z = 0.;
            tf.SetTranslationAndScale3D(loc, scale);

            // spawn actor
            AActor* NewActor = GetWorld()->SpawnActor<AActor>(BPToSpawn, tf);
            NewActor->SetActorTransform(tf);
            // make actors child actor of `this`
            NewActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
            // NewActor->SetHidden(true);


            FTimerHandle MemberTimerHandle2;
            GetWorld()->GetTimerManager().SetTimer(MemberTimerHandle2,
                FTimerDelegate::CreateLambda([M, i, NewActor, MemberTimerHandle2, DeltaTime, this] {
                // GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Timer");
                if (IsValid(ConnectFromAgent) && IsValid(ConnectToAgent))
                {
                    FVector scale = GetActorScale3D();
                    scale.X = 0.1;
                    scale.Y = 0.1;
                    scale.Z = 0.1;
                    NewActor->SetActorScale3D(scale);

                    // set the location of the NewActor between ConnectFromAgent and ConnectToAgent, (i/M) is the ratio
                    float ratio = (float)i / M;

                    // ratio increase over time, and back to 0 when > 1
                    ratio += AgentAliveTime * RunningSpeed;
                    ratio = fmod(ratio, 1);

                    FVector ParentLocaion =
                        ConnectFromAgent->GetActorLocation() * (1 - ratio) +
                        ConnectToAgent->GetActorLocation() * (ratio);
                    NewActor->SetActorLocation(ParentLocaion);


             /*       FVector ParentLocaion = GetActorLocation();
                    float RoSpeed_H = 180;
                    ParentLocaion.X += 100 * cos((float)(i * (360 / M) + AgentAliveTime * RoSpeed_H) / 180 * PI);
                    ParentLocaion.Y += 100 * sin((float)(i * (360 / M) + AgentAliveTime * RoSpeed_H) / 180 * PI);
                    ParentLocaion.Z += 10 * sin((float)(i * (4 * 360 / M) + AgentAliveTime * 6 * RoSpeed_H) / 180 * PI);
                    NewActor->SetActorLocation(ParentLocaion);*/
                }
                else
                {
                    FVector scale = GetActorScale3D();
                    scale *= 0.;
                    NewActor->SetActorScale3D(scale);

                }
            }), DeltaTime, true);
        }
    }
    else
    {

    }

}


