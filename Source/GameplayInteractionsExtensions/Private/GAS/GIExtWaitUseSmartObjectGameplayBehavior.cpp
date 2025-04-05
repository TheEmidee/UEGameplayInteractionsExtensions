#include "GAS/GIExtWaitUseSmartObjectGameplayBehavior.h"

#include "GIExtFunctionLibrary.h"
#include "GIExtGameplayInteractionContextWrapper.h"

#include <AbilitySystemComponent.h>
#include <GameplayInteractionSmartObjectBehaviorDefinition.h>
#include <Misc/ScopeExit.h>
#include <SmartObjectSubsystem.h>
#include <VisualLogger/VisualLogger.h>

UGIExtWaitUseSmartObjectGameplayBehavior::UGIExtWaitUseSmartObjectGameplayBehavior( const FObjectInitializer & object_initializer ) :
    Super( object_initializer )
{
    bTickingTask = true;
}

void UGIExtWaitUseSmartObjectGameplayBehavior::Activate()
{
    Super::Activate();

    SetWaitingOnAvatar();

    bool bSuccess = false;
    ON_SCOPE_EXIT
    {
        if ( !bSuccess )
        {
            EndTask();
        }
    };

    if ( !Context.IsValid() )
    {
        return;
    }

    Context.OnSlotInvalidatedCallback.BindLambda( [ this ]( const FSmartObjectClaimHandle & /*claim_handle*/, ESmartObjectSlotState /*state*/ ) {
        Abort( EGameplayInteractionAbortReason::InternalAbort );
    } );

    Context.OnInteractionFinishedCallback.BindLambda( [ & ]() {
        EndTask();
    } );

    GameplayInteractionContextWrapper = NewObject< UGIExtGameplayInteractionContextWrapper >();
    if ( GameplayInteractionContextWrapper->StartInteraction( Context ) )
    {
        OnActivated.Broadcast( GameplayInteractionContextWrapper );
        bSuccess = true;
    }
}

UGIExtWaitUseSmartObjectGameplayBehavior * UGIExtWaitUseSmartObjectGameplayBehavior::WaitUseSmartObjectGameplayBehaviorWithSmartObjectComponent( UGameplayAbility * owning_ability, USmartObjectComponent * smart_object_component, EGIExtSmartObjectComponentSlotSelection slot_selection, const TArray< TSubclassOf< USmartObjectBehaviorDefinition > > behavior_definition_classes, const FGameplayTagQuery activity_tags, ESmartObjectClaimPriority claim_priority )
{
    FGIExtFindSlotQuery query;
    query.Querier = owning_ability->GetAvatarActorFromActorInfo();
    query.SmartObjectComponent = smart_object_component;
    query.SlotSelection = slot_selection;
    query.ClaimPriority = claim_priority;

    owning_ability->GetAbilitySystemComponentFromActorInfo()->GetOwnedGameplayTags( query.RequestFilter.UserTags );
    query.RequestFilter.BehaviorDefinitionClasses = behavior_definition_classes;
    query.RequestFilter.ActivityRequirements = activity_tags;

    const auto context = UGIExtFunctionLibrary::CreateGameplayInteractionContext( query );

    return WaitUseSmartObjectGameplayBehaviorWithContext( owning_ability, context );
}

UGIExtWaitUseSmartObjectGameplayBehavior * UGIExtWaitUseSmartObjectGameplayBehavior::WaitUseSmartObjectGameplayBehaviorWithSlotHandle( UGameplayAbility * owning_ability, FSmartObjectSlotHandle slot_handle, ESmartObjectClaimPriority claim_priority )
{
    auto * smart_object_subsystem = USmartObjectSubsystem::GetCurrent( owning_ability->GetWorld() );
    const auto claim_handle = smart_object_subsystem->MarkSlotAsClaimed( slot_handle, claim_priority );

    return WaitUseSmartObjectGameplayBehaviorWithClaimHandle( owning_ability, claim_handle );
}

UGIExtWaitUseSmartObjectGameplayBehavior * UGIExtWaitUseSmartObjectGameplayBehavior::WaitUseSmartObjectGameplayBehaviorWithClaimHandle( UGameplayAbility * owning_ability, FSmartObjectClaimHandle claim_handle )
{
    auto * task = NewAbilityTask< UGIExtWaitUseSmartObjectGameplayBehavior >( owning_ability );

    FGIExtStartGameplayInteractionContext context;
    context.Querier = owning_ability->GetAvatarActorFromActorInfo();
    context.ClaimHandle = claim_handle;

    task->Context = context;
    return task;
}

UGIExtWaitUseSmartObjectGameplayBehavior * UGIExtWaitUseSmartObjectGameplayBehavior::WaitUseSmartObjectGameplayBehaviorWithContext( UGameplayAbility * owning_ability, const FGIExtStartGameplayInteractionContext & context )
{
    auto * task = NewAbilityTask< UGIExtWaitUseSmartObjectGameplayBehavior >( owning_ability );
    task->Context = context;
    return task;
}

void UGIExtWaitUseSmartObjectGameplayBehavior::Abort( const EGameplayInteractionAbortReason reason )
{
    GameplayInteractionContextWrapper->AbortInteraction( reason );
    EndTask();
}

void UGIExtWaitUseSmartObjectGameplayBehavior::OnDestroy( bool ability_ended )
{
    GameplayInteractionContextWrapper->FinishInteraction();

    if ( TaskState != EGameplayTaskState::Finished )
    {
        if ( GameplayInteractionContextWrapper->GetAbortReason() == EGameplayInteractionAbortReason::Unset && GameplayInteractionContextWrapper->HasInteractionCompleted() )
        {
            OnSucceeded.Broadcast();
        }
        else
        {
            OnFailed.Broadcast();
        }
    }

    GameplayInteractionContextWrapper = nullptr;
    Super::OnDestroy( ability_ended );
}
