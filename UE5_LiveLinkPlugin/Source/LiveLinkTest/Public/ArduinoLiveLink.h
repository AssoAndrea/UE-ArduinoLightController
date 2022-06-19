// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ILiveLinkSource.h"
#include "ILiveLinkClient.h"
#include "Roles/LiveLinkBasicRole.h"
#include "Roles/LiveLinkLightTypes.h"
#include "Roles/LiveLinkLightRole.h"
#include "SocketSubsystem.h"
#include "Common/UdpSocketBuilder.h"
#include "Modules/ModuleManager.h"


class LIVELINKTEST_API FSimpleAIVLiveLinkSource : public ILiveLinkSource, public FTickableGameObject
{
public:
    FSimpleAIVLiveLinkSource();

    bool IsSourceStillValid() const override
    {
        return true;
    }

    void ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid) override;

    bool RequestSourceShutdown() override;

    FText GetSourceType() const override
    {
        return FText::FromString("Arduino Source");
    }

    FText GetSourceMachineName() const override
    {
        return FText::FromString(FPlatformProcess::ComputerName());
    }

    FText GetSourceStatus() const override
    {
        return FText::FromString("Active");
    }
	
    TStatId GetStatId() const override
    {
        return TStatId();
    }

    bool IsTickableInEditor() const override
    {
        return true;
    }

    void Tick(float DeltaTime) override;
	
    FLiveLinkSubjectKey LightSubject;
    FLiveLinkSubjectKey CommandSubject;
    ILiveLinkClient* LiveLinkClient;
    FSocket* Socket;

};

class FLiveLinkTestModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
