#pragma once

#include "GIExtTypes.h"

#include <CoreMinimal.h>
#include <GameplayInteractionContext.h>

#include "GIExtGameplayInteractionContextWrapper.generated.h"

UCLASS( BlueprintType )
class GAMEPLAYINTERACTIONSEXTENSIONS_API UGIExtGameplayInteractionContextWrapper final : public UObject, public FTickableGameObject
{
    GENERATED_BODY()

public:
    bool HasInteractionCompleted() const;
    EGameplayInteractionAbortReason GetAbortReason() const;
    bool StartInteraction( const FGIExtStartGameplayInteractionContext & context );
    void AbortInteraction( EGameplayInteractionAbortReason abort_reason );

    UFUNCTION( BlueprintCallable )
    void FinishInteraction();

    void Tick( float delta_time ) override;
    bool IsTickable() const override;
    TStatId GetStatId() const override;

    UFUNCTION( BlueprintCallable )
    void SendEventToStateTree( const FGameplayTag tag );

    UFUNCTION( BlueprintCallable )
    static UGIExtGameplayInteractionContextWrapper * StartGameplayInteractionFromContext( const FGIExtStartGameplayInteractionContext & context );

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