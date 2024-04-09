// Copyright 2023 NaN_Name, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"

class IIM4U : public IModuleInterface
{

public:

	static inline IIM4U& Get()
	{
		return FModuleManager::LoadModuleChecked< IIM4U >("IM4U");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded( "IM4U" );
	}
};

