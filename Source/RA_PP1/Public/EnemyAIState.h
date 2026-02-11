#pragma once

#include "CoreMinimal.h"
#include "EnemyAIState.generated.h"


//UMETA for editing in editor and integrate
UENUM(BlueprintType)
enum class EEnemyAIState : uint8
{
	Idle    UMETA(DisplayName = "Idle"),
	Patrol  UMETA(DisplayName = "Patrol"),
	Chase   UMETA(DisplayName = "Chase"),
	Search  UMETA(DisplayName = "Search")
};
