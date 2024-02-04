// Fill out your copyright notice in the Description page of Project Settings.


#include "MatrixCommunication.h"
#include "Misc/CommandLine.h"
#include "IWebSocket.h"
#include "WebSocketsModule.h"


// Sets default values
AMatrixCommunication::AMatrixCommunication()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMatrixCommunication::BeginPlay()
{
    Super::BeginPlay();
    InitWebSocket();
}


void AMatrixCommunication::BeginDestroy()
{
    Super::BeginDestroy();
    if (WebSocket && WebSocket->IsConnected()) {
        WebSocket->Close();
    }
}

// Called every frame
void AMatrixCommunication::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

FString ParseCommandLineArguments()
{
    const FString ParamName = TEXT("-WsAddr=");
    const int32 ParamNameLength = ParamName.Len();

    FString CommandLine = FCommandLine::Get();
    TArray<FString> Args;
    CommandLine.ParseIntoArrayWS(Args);
    FString ParameterValue;

    for (FString Arg : Args)
    {
        // ��鵱ǰ�����Ƿ������Ǹ���Ȥ�Ĳ���
        if (Arg.StartsWith(ParamName))
        {
            // ��ȡ����ֵ�����ﲻ��ʹ��ħ�����֣�����ʹ��֮ǰ����ĳ���
            ParameterValue = Arg.Mid(ParamNameLength);
            UE_LOG(LogTemp, Log, TEXT("WsAddr value: %s"), *ParameterValue);
            return ParameterValue;
        }
    }

    return "ws://127.0.0.1:10101/ws_agentcraft_interface";
}

// Called when the game starts or when spawned
FString AMatrixCommunication::GetWebSocketAddr()
{
    FString WsUrl = ParseCommandLineArguments();
    return WsUrl;
}

void AMatrixCommunication::InitWebSocket()
{
    FString WsUrl = GetWebSocketAddr();

    if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
    {
        FModuleManager::Get().LoadModule("WebSockets");
    }

    // ����WebSocket����ʵ��
    WebSocket = FWebSocketsModule::Get().CreateWebSocket(WsUrl);

    // ����¼�����
    WebSocket->OnConnected().AddLambda([this]() {
        // ���ӳɹ�ʱ�����Ĵ���
        UE_LOG(LogTemp, Log, TEXT("Connection Success"));
        
        // ���ͳ�ʼ������Ϣ
        FMatrixMsgStruct Msg = FMatrixMsgStruct();

        // ����UID
        FGuid MyUniqueID = FGuid::NewGuid();
        FString MatrixComUID = MyUniqueID.ToString();
        Msg.src = MatrixComUID;
        Msg.dst = "matrix";
        Msg.command = "connect_to_matrix";
        ConvertToJsonAndSendWs(Msg);

    });

    WebSocket->OnConnectionError().AddLambda([](const FString& Error) {
        // ���ӳ���ʱ�����Ĵ���
        UE_LOG(LogTemp, Warning, TEXT("Connection Error: %s"), *Error);
    });

    WebSocket->OnClosed().AddLambda([](int32 StatusCode, const FString& Reason, bool bWasClean) {
        // ���ӹر�ʱ�����Ĵ���
        UE_LOG(LogTemp, Log, TEXT("Connection Close"));
    });

    WebSocket->OnMessage().AddLambda([](const FString& Message) {
        // ���յ���Ϣʱ�Ĵ���
        UE_LOG(LogTemp, Log, TEXT("Receive Message: %s"), *Message);
        if (GEngine)
        {
            // Format the FString using Printf
            FString FormattedMessage = FString::Printf(TEXT("Receive Message: %s"), *Message);
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FormattedMessage);
        }
    });

    // ��ʼ����
    WebSocket->Connect();
}


FMatrixMsgStruct AMatrixCommunication::ParsedFMatrixMsgStruct(FString TcpLatestRecvString)
{

    FMatrixMsgStruct MatrixMsg;
    TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(TcpLatestRecvString);
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    FJsonSerializer::Deserialize(JsonReader, JsonObject);
    FJsonObjectConverter::JsonObjectToUStruct<FMatrixMsgStruct>(JsonObject.ToSharedRef(), &MatrixMsg, 0, 0);
    return MatrixMsg;

}


void AMatrixCommunication::ConvertToJsonAndSendWs(FMatrixMsgStruct MatrixMsg)
{
    TSharedRef<FJsonObject> MatrixMsgJson = MakeShareable(new FJsonObject);
    FJsonObjectConverter::UStructToJsonObject(FMatrixMsgStruct::StaticStruct(), &MatrixMsg, MatrixMsgJson, 0, 0);
    WsSendJson(MatrixMsgJson);
}


void AMatrixCommunication::WsSendJson(TSharedPtr<FJsonObject> ReplyJson)
{
    // ת���ַ���
    FString ReplyString;
    auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&ReplyString);
    FJsonSerializer::Serialize(ReplyJson.ToSharedRef(), Writer);
    ensureMsgf(WebSocket->IsConnected(), TEXT("WebSocket Is Not Connected!"));
    WebSocket->Send(ReplyString);
}
