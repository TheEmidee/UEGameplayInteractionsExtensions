#include "GIExtGameplayInteractionContextWrapper.h"

#include <GameplayInteractionSmartObjectBehaviorDefinition.h>
#include <SmartObjectComponent.h>

bool UGIExtGameplayInteractionContextWrapper::StartInteraction( const FGIExtStartGameplayInteractionContext & context )
{
    StartInteractionContext = context;

    auto * smart_object_subsystem = GetSmartObjectSubsystem();
    if ( smart_object_subsystem == nullptr )
    {
        return false;
    }

    if ( !smart_object_subsystem->IsClaimedSmartObjectValid( context.ClaimHandle ) )
    {
        UE_VLOG( context.Querier, LogSmartObject, Log, TEXT( "Claim handle: %s refers to an object that is no longer available." ), *LexToString( context.ClaimHandle ) );
        return false;
    }

    GameplayInteractionContext.SetContextActor( context.Querier );
    GameplayInteractionContext.SetClaimedHandle( context.ClaimHandle );

    auto smart_object_actor = context.SmartObjectActor;
    if ( smart_object_actor == nullptr )
    {
        const auto * smart_object_component = smart_object_subsystem->GetSmartObjectComponent( context.ClaimHandle );
        smart_object_actor = smart_object_component != nullptr ? smart_object_component->GetOwner() : nullptr;
    }

    GameplayInteractionContext.SetSmartObjectActor( smart_object_actor );

    const auto * behavior_definition = smart_object_subsystem->MarkSlotAsOccupied< UGameplayInteractionSmartObjectBehaviorDefinition >( context.ClaimHandle );
    if ( behavior_definition == nullptr )
    {
        UE_VLOG_UELOG( context.Querier, LogGameplayInteractions, Error, TEXT( "SmartObject was claimed for a different type of behavior definition. Expecting: %s." ), *UGameplayInteractionSmartObjectBehaviorDefinition::StaticClass()->GetName() );
        return false;
    }

    smart_object_subsystem->RegisterSlotInvalidationCallback( context.ClaimHandle, FOnSlotInvalidated::CreateUObject( this, &ThisClass::OnSlotInvalidated ) );

    StartInteractionContext.PreActivationCallback.ExecuteIfBound();

    return GameplayInteractionContext.Activate( *behavior_definition );
}

void UGIExtGameplayInteractionContextWrapper::AbortInteraction( EGameplayInteractionAbortReason abort_reason )
{
    AbortContext.Reason = abort_reason;

    if ( !bInteractionCompleted )
    {
        GameplayInteractionContext.SetAbortContext( AbortContext );
    }
}

void UGIExtGameplayInteractionContextWrapper::FinishInteraction()
{
    if ( bInteractionCompleted )
    {
        return;
    }

    GameplayInteractionContext.Deactivate();

    if ( StartInteractionContext.ClaimHandle.IsValid() )
    {
        auto * smart_object_subsystem = GetSmartObjectSubsystem();
        check( smart_object_subsystem != nullptr );

        smart_object_subsystem->MarkSlotAsFree( StartInteractionContext.ClaimHandle );
        smart_object_subsystem->UnregisterSlotInvalidationCallback( StartInteractionContext.ClaimHandle );

        StartInteractionContext.ClaimHandle.Invalidate();
    }

    bInteractionCompleted = true;
    StartInteractionContext.OnInteractionFinishedCallback.ExecuteIfBound();
}

void UGIExtGameplayInteractionContextWrapper::Tick( float delta_time )
{
    if ( !GameplayInteractionContext.Tick( delta_time ) )
    {
        FinishInteraction();
    }
}

bool UGIExtGameplayInteractionContextWrapper::IsTickable() const
{
    return !bInteractionCompleted;
}

TStatId UGIExtGameplayInteractionContextWrapper::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT( UGIExtGameplayInteractionContextWrapper, STATGROUP_Tickables );
}

void UGIExtGameplayInteractionContextWrapper::SendEventToStateTree( const FGameplayTag tag )
{
    GameplayInteractionContext.SendEvent( tag );
}

UGIExtGameplayInteractionContextWrapper * UGIExtGameplayInteractionContextWrapper::StartGameplayInteractionFromContext( const FGIExtStartGameplayInteractionContext & context )
{
    auto * wrapper = NewObject< UGIExtGameplayInteractionContextWrapper >();

    if ( !wrapper->StartInteraction( context ) )
    {
        return nullptr;
    }

    return wrapper;
}

USmartObjectSubsystem * UGIExtGameplayInteractionContextWrapper::GetSmartObjectSubsystem() const
{
    const auto * world = StartInteractionContext.Querier->GetWorld();
    auto * smart_object_subsystem = USmartObjectSubsystem::GetCurrent( world );
    if ( !ensureMsgf( smart_object_subsystem != nullptr, TEXT( "SmartObjectSubsystem must be accessible at this point." ) ) )
    {
        return nullptr;
    }

    return smart_object_subsystem;
}

void UGIExtGameplayInteractionContextWrapper::OnSlotInvalidated( const FSmartObjectClaimHandle & claim_handle, ESmartObjectSlotState state )
{
    StartInteractionContext.OnSlotInvalidatedCallback.ExecuteIfBound( claim_handle, state );
}