#pragma once

#include <Modules/ModuleManager.h>

class FGameplayInteractionsExtensionsModule final : public IModuleInterface
{
public:
    void StartupModule() override;
    void ShutdownModule() override;
};
