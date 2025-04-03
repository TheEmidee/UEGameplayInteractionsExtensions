#pragma once

#include <CoreMinimal.h>
#include <SmartObjectRuntime.h>

#include "GIExtTypes.generated.h"

struct FGameplayTag;

UENUM( BlueprintType )
enum class EGIExtSmartObjectComponentSlotSelection : uint8
{
    First,
    Closest,
    Random
};

USTRUCT( BlueprintType )
struct GAMEPLAYINTERACTIONSEXTENSIONS_API FGIExtStartGameplayInteractionContext
{
    GENERATED_BODY()

    bool IsValid() const;

    UPROPERTY( BlueprintReadWrite )
    TObjectPtr< AActor > Querier;

    UPROPERTY( BlueprintReadWrite )
    TObjectPtr< AActor > SmartObjectActor;

    UPROPERTY( BlueprintReadWrite )
    FSmartObjectClaimHandle ClaimHandle;

    FSimpleDelegate PreActivationCallback;
    FSimpleDelegate OnInteractionFinishedCallback;
    FOnSlotInvalidated OnSlotInvalidatedCallback;
};