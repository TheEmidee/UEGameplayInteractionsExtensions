#pragma once

#include "GIExtTypes.h"

#include <CoreMinimal.h>
#include <Kismet/BlueprintFunctionLibrary.h>
#include <SmartObjectSubsystem.h>

#include "GIExtFunctionLibrary.generated.h"

class USmartObjectComponent;

USTRUCT( BlueprintType )
struct GAMEPLAYINTERACTIONSEXTENSIONS_API FGIExtFindSlotQuery
{
    GENERATED_BODY()

    UPROPERTY( BlueprintReadWrite )
    TObjectPtr< AActor > Querier;

    UPROPERTY( BlueprintReadWrite )
    TObjectPtr< USmartObjectComponent > SmartObjectComponent;

    UPROPERTY( BlueprintReadWrite )
    FSmartObjectRequestFilter RequestFilter;

    UPROPERTY( BlueprintReadWrite )
    EGIExtSmartObjectComponentSlotSelection SlotSelection = EGIExtSmartObjectComponentSlotSelection::First;

    UPROPERTY( BlueprintReadWrite )
    ESmartObjectClaimPriority ClaimPriority = ESmartObjectClaimPriority::Normal;
};

UCLASS()
class GAMEPLAYINTERACTIONSEXTENSIONS_API UGIExtFunctionLibrary final : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION( BlueprintCallable )
    static FGIExtStartGameplayInteractionContext CreateGameplayInteractionContext( const FGIExtFindSlotQuery & query );
};
