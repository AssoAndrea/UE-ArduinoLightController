// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArduinoLiveLink.h"

#define LOCTEXT_NAMESPACE "FLiveLinkTestModule"

void FLiveLinkTestModule::StartupModule()
{
	ILiveLinkClient& LiveLinkClient = IModularFeatures::Get().GetModularFeature<ILiveLinkClient>(ILiveLinkClient::ModularFeatureName);
	LiveLinkClient.AddSource(MakeShared<FSimpleAIVLiveLinkSource>());
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FLiveLinkTestModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

FSimpleAIVLiveLinkSource::FSimpleAIVLiveLinkSource()
{
	LiveLinkClient = nullptr;
	Socket = nullptr;
}

void FSimpleAIVLiveLinkSource::ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid)
{
	if (Socket)
	{
		Socket->Close();
	}
	
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	
	TSharedPtr<FInternetAddr> EndpointInternetAddr = SocketSubsystem->CreateInternetAddr();
	bool bIsValid = false;
	EndpointInternetAddr->SetIp(TEXT("192.168.1.200"), bIsValid);
	EndpointInternetAddr->SetPort(12345);
	
	FIPv4Endpoint Endpoint = FIPv4Endpoint(EndpointInternetAddr);

	Socket = FUdpSocketBuilder(TEXT("LiveLink Server"))
		.AsNonBlocking()
		.AsReusable()
		.BoundToEndpoint(Endpoint);
	

	LiveLinkClient = InClient;

	LightSubject.SubjectName = FLiveLinkSubjectName(TEXT("Light"));
	LightSubject.Source = InSourceGuid;

	CommandSubject.SubjectName = FLiveLinkSubjectName(TEXT("Command"));
	CommandSubject.Source = InSourceGuid;

	FLiveLinkStaticDataStruct LightStaticData;
	LightStaticData.InitializeWith(FLiveLinkLightStaticData::StaticStruct(), nullptr);
	FLiveLinkLightStaticData* LightStaticDataPtr = LightStaticData.Cast<FLiveLinkLightStaticData>();
	LightStaticDataPtr->bIsLightColorSupported = true;

	InClient->PushSubjectStaticData_AnyThread(LightSubject, ULiveLinkLightRole::StaticClass(), MoveTemp(LightStaticData));

	FLiveLinkStaticDataStruct CommandStaticData;
	CommandStaticData.InitializeWith(FLiveLinkBaseStaticData::StaticStruct(), nullptr);
	FLiveLinkBaseStaticData* CommandStaticDataPtr = CommandStaticData.Cast<FLiveLinkBaseStaticData>();
	CommandStaticDataPtr->PropertyNames.Add("CommandValue");

	InClient->PushSubjectStaticData_AnyThread(CommandSubject, ULiveLinkBasicRole::StaticClass(), MoveTemp(CommandStaticData));
}

bool FSimpleAIVLiveLinkSource::RequestSourceShutdown()
{
	LiveLinkClient = nullptr;
	if (Socket)
	{
		Socket->Close();
	}
	Socket = nullptr;
	return true;
}

void FSimpleAIVLiveLinkSource::Tick(float DeltaTime)
{
	if (Socket && LiveLinkClient)
	{
		uint32 RequiredSize;
		if (!Socket->HasPendingData(RequiredSize))
		{
			LiveLinkClient->ClearAllSubjectsFrames_AnyThread();
			return;
		}
		TArray<uint8> Data;
		Data.AddUninitialized(RequiredSize);
		int32 ReceivedDataSize;
		if (!Socket->Recv(Data.GetData(), Data.Num(), ReceivedDataSize))
		{
			return;
		}
		if (ReceivedDataSize == sizeof(float))
		{
			UE_LOG(LogTemp, Warning, TEXT("command"));
			float* command = reinterpret_cast<float*>(Data.GetData());
			FLiveLinkFrameDataStruct CommandFrameData = FLiveLinkFrameDataStruct(FLiveLinkBaseFrameData::StaticStruct());
			FLiveLinkBaseFrameData* CommandFrameDataPtr = CommandFrameData.Cast<FLiveLinkBaseFrameData>();
			CommandFrameDataPtr->PropertyValues.Add(*command);
			CommandFrameDataPtr->WorldTime = FPlatformTime::Seconds();
			LiveLinkClient->PushSubjectFrameData_AnyThread(CommandSubject, MoveTemp(CommandFrameData));
		}
		if (ReceivedDataSize == sizeof(float) * 4)
		{
			UE_LOG(LogTemp, Warning, TEXT("color"));
			float* light = reinterpret_cast<float*>(Data.GetData());
			FLiveLinkFrameDataStruct LightFrameData = FLiveLinkFrameDataStruct(FLiveLinkLightFrameData::StaticStruct());
			FLiveLinkLightFrameData* LightFrameDataPtr = LightFrameData.Cast<FLiveLinkLightFrameData>();
			LightFrameDataPtr->LightColor = FLinearColor(light[0], light[1], light[2], light[3]).ToFColor(false);
			LightFrameDataPtr->WorldTime = FPlatformTime::Seconds();
			LiveLinkClient->PushSubjectFrameData_AnyThread(LightSubject, MoveTemp(LightFrameData));
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLiveLinkTestModule, LiveLinkTest)


