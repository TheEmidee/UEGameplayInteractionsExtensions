#pragma once

#include <Abilities/Tasks/AbilityTask.h>
#include <CoreMinimal.h>
#include <GameplayInteractionContext.h>
#include <SmartObjectRuntime.h>
#include <SmartObjectTypes.h>

#include "GIExtWaitUseSmartObjectGameplayBehavior.generated.h"

class UGameplayBehavior;
class USmartObjectComponent;

UENUM( BlueprintType )
enum class EGIExtSmartObjectComponentSlotSelection : uint8
{
    First,
    Closest,
    Random
};

UCLASS( BlueprintType )
class UGIExtWaitUseSmartObjectProxy : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION( BlueprintCallable )
    void SendEventToStateTree( const FGameplayTag tag );

    UPROPERTY()
    FGameplayInteractionContext GameplayInteractionContext;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnWaitUseSmartObjectGameplayBehaviorActivated, UGIExtWaitUseSmartObjectProxy *, Proxy );

UCLASS()
class GAMEPLAYINTERACTIONSEXTENSIONS_API UGIExtWaitUseSmartObjectGameplayBehavior final : public UAbilityTask
{
    GENERATED_BODY()

public:
    explicit UGIExtWaitUseSmartObjectGameplayBehavior( const FObjectInitializer & object_initializer );

    void Activate() override;

    UFUNCTION( BlueprintCallable, Category = "Ability|Tasks", meta = ( HidePin = "owning_ability", DefaultToSelf = "owning_ability", BlueprintInternalUseOnly = "TRUE" ) )
    static UGIExtWaitUseSmartObjectGameplayBehavior * WaitUseSmartObjectGameplayBehaviorWithSmartObjectComponent( UGameplayAbility * owning_ability, USmartObjectComponent * smart_object_component, EGIExtSmartObjectComponentSlotSelection slot_selection, const TArray< TSubclassOf< USmartObjectBehaviorDefinition > > behavior_definition_classes, const FGameplayTagQuery activity_tags, ESmartObjectClaimPriority claim_priority = ESmartObjectClaimPriority::Normal );

    UFUNCTION( BlueprintCallable, Category = "Ability|Tasks", meta = ( HidePin = "owning_ability", DefaultToSelf = "owning_ability", BlueprintInternalUseOnly = "TRUE" ) )
    static UGIExtWaitUseSmartObjectGameplayBehavior * WaitUseSmartObjectGameplayBehaviorWithSlotHandle( UGameplayAbility * owning_ability, FSmartObjectSlotHandle slot_handle, ESmartObjectClaimPriority claim_priority = ESmartObjectClaimPriority::Normal );

    UFUNCTION( BlueprintCallable, Category = "Ability|Tasks", meta = ( HidePin = "owning_ability", DefaultToSelf = "owning_ability", BlueprintInternalUseOnly = "TRUE" ) )
    static UGIExtWaitUseSmartObjectGameplayBehavior * WaitUseSmartObjectGameplayBehaviorWithClaimHandle( UGameplayAbility * owning_ability, FSmartObjectClaimHandle claim_handle );

private:
    bool StartInteraction();
    void Abort( const EGameplayInteractionAbortReason reason );
    void OnDestroy( bool ability_ended ) override;
    void TickTask( float delta_time ) override;

    void OnSlotInvalidated( const FSmartObjectClaimHandle & claim_handle, ESmartObjectSlotState state );
    void OnSmartObjectBehaviorFinished( UGameplayBehavior & behavior, AActor & avatar, const bool interrupted );

    UPROPERTY( BlueprintAssignable )
    FOnWaitUseSmartObjectGameplayBehaviorActivated OnActivated;

    UPROPERTY( BlueprintAssignable )
    FGenericGameplayTaskDelegate OnSucceeded;

    UPROPERTY( BlueprintAssignable )
    FGenericGameplayTaskDelegate OnFailed;

    UPROPERTY()
    TObjectPtr< UGIExtWaitUseSmartObjectProxy > SmartObjectProxy;

    UPROPERTY()
    FGameplayInteractionAbortContext AbortContext;

    FSmartObjectClaimHandle ClaimedHandle;
    FDelegateHandle OnBehaviorFinishedNotifyHandle;
    bool bInteractionCompleted;
    ESmartObjectClaimPriority ClaimPriority;
};
