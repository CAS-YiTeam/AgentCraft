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
    MsgQueue.Empty();
    InitWebSocket();
}

void AMatrixCommunication::BeginDestroy()
{
    WebSocketConnectionStat = "Terminated";
    if (WebSocket.IsValid() && WebSocket->IsConnected())
    {
        WebSocket->Close();
    }
    WebSocket.Reset();
    Super::BeginDestroy();
}

// Called every frame
void AMatrixCommunication::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

FString AMatrixCommunication::ParseCommandLineArguments()
{
    const FString ParamName = TEXT("-WsAddr=");
    const int32 ParamNameLength = ParamName.Len();

    FString CommandLine = FCommandLine::Get();
    TArray<FString> Args;
    CommandLine.ParseIntoArrayWS(Args);
    FString ParameterValue;

    for (FString Arg : Args)
    {
        // 检查当前参数是否是我们感兴趣的参数
        if (Arg.StartsWith(ParamName))
        {
            // 获取参数值，这里不再使用魔法数字，而是使用之前定义的常量
            ParameterValue = Arg.Mid(ParamNameLength);
            UE_LOG(LogTemp, Log, TEXT("WsAddr value: %s"), *ParameterValue);
            WebSocketCurrentUrl = ParameterValue;
            return ParameterValue;
        }
    }

    WebSocketCurrentUrl = "ws://127.0.0.1:10101/ws_agentcraft_interface";
    return WebSocketCurrentUrl;
}

// Called when the game starts or when spawned
FString AMatrixCommunication::GetWebSocketAddr()
{
    FString WsUrl = ParseCommandLineArguments();
    return WsUrl;
}

/**
 * Initializes the WebSocket connection.
 * This function creates a WebSocket connection instance, adds event listeners for connection status and message reception,
 * and starts the connection.
 */
void AMatrixCommunication::InitWebSocket()
{
    FString WsUrl = GetWebSocketAddr();

    if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
    {
        FModuleManager::Get().LoadModule("WebSockets");
    }

    // 创建WebSocket连接实例
    WebSocket = FWebSocketsModule::Get().CreateWebSocket(WsUrl);

    // 添加事件监听
    WebSocket->OnConnected().AddLambda([this]() {
        // 连接成功时所做的处理
        UE_LOG(LogTemp, Log, TEXT("Connection Success"));
        WebSocketConnectionStat = "Connection OK";

        // 发送初始连接信息
        FMatrixMsgStruct Msg = FMatrixMsgStruct();

        // 生成UID
        FGuid MyUniqueID = FGuid::NewGuid();
        MatrixComUID = MyUniqueID.ToString();
        Msg.src = MatrixComUID;
        Msg.dst = "matrix";
        Msg.command = "connect_to_matrix";
        Msg.need_reply = true;
        ConvertToJsonAndSendWs(Msg);

    });

    WebSocket->OnConnectionError().AddLambda([this](const FString& Error) {
        UE_LOG(LogTemp, Warning, TEXT("Connection Error: %s"), *Error);
        if (WebSocket)
        {
            // 连接出错时所做的处理
            TryReconnect();
        }

    });

    WebSocket->OnClosed().AddLambda([this](int32 StatusCode, const FString& Reason, bool bWasClean) {
        UE_LOG(LogTemp, Log, TEXT("Connection Close"));
        if (WebSocket)
        {
            // 连接关闭时所做的处理
            TryReconnect();
        }
    });

    WebSocket->OnMessage().AddLambda([this](const FString& Message) {
        UE_LOG(LogTemp, Log, TEXT("Receive Message: %s"), *Message);
        if (WebSocket)
        {
            // 接收到消息时的处理
            WebSocketConnectionStat = "Connection OK";
            if (GEngine)
            {
                // Format the FString using Printf
                FString FormattedMessage = FString::Printf(TEXT("Receive Message: %s"), *Message);
                FMatrixMsgStruct Msg = ParsedFMatrixMsgStruct(Message);
                MsgQueue.Enqueue(Msg); // add to head
                GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FormattedMessage);
            }
        }
    });

    // 开始连接
    WebSocket->Connect();
}

// 定义尝试重新连接的函数
void AMatrixCommunication::TryReconnect()
{
    if (WebSocket)
    { 
        if (!WebSocketConnectionStat.IsEmpty())
        { 
            if (WebSocketConnectionStat != "Terminated")
            {
                WebSocketConnectionStat = "Reconnecting ...";
                FTimerHandle ReconnectTimerHandle;
                // 如果WebSocket仍然存在，它可能处于关闭状态
                if (WebSocket.IsValid())
                {
                    if(!WebSocket->IsConnected())
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Attempting to reconnect..."));
                        WebSocket->Connect();
                    }
                }
            }
        }
    }
}

// 定义尝试重新连接的函数
FMatrixMsgStruct AMatrixCommunication::PopNextMessageFromQueue()
{
    FMatrixMsgStruct Msg;

    if (MsgQueue.IsEmpty())
    {
        Msg.valid = false;
        return Msg;
    }

    Msg.valid = true;
    MsgQueue.Dequeue(Msg);
    return Msg;
}

bool AMatrixCommunication::IsQueueEmpty()
{
    return MsgQueue.IsEmpty();
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
    // 转成字符串
    FString ReplyString;
    auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&ReplyString);
    FJsonSerializer::Serialize(ReplyJson.ToSharedRef(), Writer);
    ensureMsgf(WebSocket->IsConnected(), TEXT("WebSocket Is Not Connected!"));
    WebSocket->Send(ReplyString);
}
