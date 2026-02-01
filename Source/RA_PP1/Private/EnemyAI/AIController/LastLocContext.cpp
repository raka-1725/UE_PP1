#include "EnemyAI/AIController/LastLocContext.h"
#include "EnemyAI/AIController/EnemyAIController.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"


void ULastLocContext::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	AEnemyAIController* Controller = Cast<AEnemyAIController>(QueryInstance.Owner.Get());
	FVector location = Controller->LastKnownLocation;
	UEnvQueryItemType_Point::SetContextHelper(ContextData, location);
}

