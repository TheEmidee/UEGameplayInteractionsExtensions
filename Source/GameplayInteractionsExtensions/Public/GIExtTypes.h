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

    FSimpleDelegate PreActivationCallback;
    FSimpleDelegate OnInteractionFinishedCallback;
    FOnSlotInvalidated  OnSlotInvalidatedCallback;
};

UCLASS( BlueprintType )
class GAMEPLAYINTERACTIONSEXTENSIONS_API UGIExtGameplayInteractionContextWrapper : public UObject, public FTickableGameObject
{
    GENERATED_BODY()

public:
    bool HasInteractionCompleted() const;
    EGameplayInteractionAbortReason GetAbortReason() const;
    bool StartInteraction( const FGIExtStartGameplayInteractionContext & context );
    // bool TickInteraction( float delta_time );
    void AbortInteraction( EGameplayInteractionAbortReason abort_reason );
    void FinishInteraction();

    void Tick( float delta_time ) override;
    bool IsTickable() const override;
    TStatId GetStatId() const override;

    UFUNCTION( BlueprintCallable )
    void SendEventToStateTree( const FGameplayTag tag );

private:
    USmartObjectSubsystem * GetSmartObjectSubsystem() const;
    void OnSlotInvalidated( const FSmartObjectClaimHandle & claim_handle, ESmartObjectSlotState state );

    UPROPERTY()
    FGameplayInteractionContext GameplayInteractionContext;

    FGIExtStartGameplayInteractionContext StartInteractionContext;
    FGameplayInteractionAbortContext AbortContext;
    bool bInteractionCompleted = false;
};

FORCEINLINE bool UGIExtGameplayInteractionContextWrapper::HasInteractionCompleted() const
{
    return bInteractionCompleted;
}

FORCEINLINE EGameplayInteractionAbortReason UGIExtGameplayInteractionContextWrapper::GetAbortReason() const
{
    return AbortContext.Reason;
}