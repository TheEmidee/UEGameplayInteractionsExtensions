#include "GIExtFunctionLibrary.h"

#include "BlueprintLibraries/CoreExtArrayBlueprintLibrary.h"

#include <GameplayInteractionSmartObjectBehaviorDefinition.h>
#include <SmartObjectComponent.h>
#include <SmartObjectSubsystem.h>

bool FGIExtStartGameplayInteractionContext::IsValid() const
{
    return Querier != nullptr && ClaimHandle.IsValid();
}

FGIExtStartGameplayInteractionContext UGIExtFunctionLibrary::CreateGameplayInteractionContext( const FGIExtFindSlotQuery & query )
{
    if ( query.SmartObjectComponent == nullptr )
    {
        return {};
    }

    auto * world = query.SmartObjectComponent->GetWorld();
    auto * smart_object_subsystem = USmartObjectSubsystem::GetCurrent( world );

    TArray< FSmartObjectSlotHandle > slots;
    smart_object_subsystem->FindSlots( query.SmartObjectComponent->GetRegisteredHandle(), query.RequestFilter, slots );

    slots.RemoveAll( [ & ]( FSmartObjectSlotHandle handle ) {
        return !smart_object_subsystem->CanBeClaimed( handle );
    } );

    if ( slots.Num() == 0 )
    {
        return {};
    }

    FSmartObjectSlotHandle selected_slot;

    switch ( query.SlotSelection )
    {
        case EGIExtSmartObjectComponentSlotSelection::First:
        {
            selected_slot = slots[ 0 ];
        }
        break;
        case EGIExtSmartObjectComponentSlotSelection::Closest:
        {
            if ( !ensureAlways( query.Querier != nullptr ) )
            {
                return {};
            }

            const auto avatar_location = query.Querier->GetActorLocation();
            auto max_distance_squared = TNumericLimits< float >::Max();

            for ( const auto slot_handle : slots )
            {
                const auto slot_location = smart_object_subsystem->GetSlotLocation( slot_handle ).GetValue();
                const auto distance_squared = ( slot_location - avatar_location ).SquaredLength();

                if ( distance_squared < max_distance_squared )
                {
                    max_distance_squared = distance_squared;
                    selected_slot = slot_handle;
                }
            }
        }
        break;
        case EGIExtSmartObjectComponentSlotSelection::Random:
        {
            selected_slot = UCoreExtArrayBlueprintLibrary::GetRandomArrayValue( slots );
        }
        break;
        default:
        {
            checkNoEntry();
            break;
        }
    }

    FGIExtStartGameplayInteractionContext context;
    context.Querier = query.Querier;
    context.ClaimHandle = smart_object_subsystem->MarkSlotAsClaimed( selected_slot, query.ClaimPriority );
    context.SmartObjectActor = query.SmartObjectComponent != nullptr ? query.SmartObjectComponent->GetOwner() : nullptr;

    return context;
}