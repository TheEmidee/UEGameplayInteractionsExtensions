#pragma once

#include "GIExtFunctionLibrary.h"
#include "GIExtTypes.h"

#include <Abilities/Tasks/AbilityTask.h>
#include <CoreMinimal.h>
#include <SmartObjectRuntime.h>
#include <SmartObjectTypes.h>

#include "GIExtWaitUseSmartObjectGameplayBehavior.generated.h"

class UGameplayBehavior;
class USmartObjectComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnWaitUseSmartObjectGameplayBehaviorActivated, UGIExtGameplayInteractionContextWrapper *, Proxy );

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

    UFUNCTION( BlueprintCallable, Category = "Ability|Tasks", meta = ( HidePin = "owning_ability", DefaultToSelf = "owning_ability", BlueprintInternalUseOnly = "TRUE" ) )
    static UGIExtWaitUseSmartObjectGameplayBehavior * WaitUseSmartObjectGameplayBehaviorWithContext( UGameplayAbility * owning_ability, const FGIExtStartGameplayInteractionContext & context );

private:
    void Abort( const EGameplayInteractionAbortReason reason );
    void OnDestroy( bool ability_ended ) override;
    void TickTask( float delta_time ) override;

    UPROPERTY( BlueprintAssignable )
    FOnWaitUseSmartObjectGameplayBehaviorActivated OnActivated;

    UPROPERTY( BlueprintAssignable )
    FGenericGameplayTaskDelegate OnSucceeded;

    UPROPERTY( BlueprintAssignable )
    FGenericGameplayTaskDelegate OnFailed;

    UPROPERTY()
    TObjectPtr< UGIExtGameplayInteractionContextWrapper > GameplayInteractionContextWrapper;

    FGIExtStartGameplayInteractionContext Context;
};
