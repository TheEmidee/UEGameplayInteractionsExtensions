#pragma once

#include <CoreMinimal.h>
#include <GameplayInteractionContext.h>

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

    TFunction< void( const FSmartObjectClaimHandle &, ESmartObjectSlotState ) > OnSlotInvalidatedCallback = {};
};

UCLASS( BlueprintType )
class UGIExtGameplayInteractionContextWrapper : public UObject
{
    GENERATED_BODY()

public:
    bool HasInteractionCompleted() const;
    bool StartInteraction( const FGIExtStartGameplayInteractionContext & context );
    bool TickInteraction( float delta_time );
    void AbortInteraction( EGameplayInteractionAbortReason abort_reason );
    void FinishInteraction();

    UFUNCTION( BlueprintCallable )
    void SendEventToStateTree( const FGameplayTag tag );

private:
    USmartObjectSubsystem * GetSmartObjectSubsystem() const;
    void OnSlotInvalidated( const FSmartObjectClaimHandle & claim_handle, ESmartObjectSlotState state );

    UPROPERTY()
    FGameplayInteractionContext GameplayInteractionContext;

    FGIExtStartGameplayInteractionContext StartInteractionContext;
    bool bInteractionCompleted = false;
};

FORCEINLINE bool UGIExtGameplayInteractionContextWrapper::HasInteractionCompleted() const
{
    return bInteractionCompleted;
}