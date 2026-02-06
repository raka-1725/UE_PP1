#include "EnemyAI/AIController/LastLocContext.h"
#include "EnemyAI/AIController/EnemyAIController.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"


void ULastLocContext::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	UObject* Owner = QueryInstance.Owner.Get();
	AEnemyAIController* Controller = Cast<AEnemyAIController>(QueryInstance.Owner.Get());
	if (!Controller)
	{
		APawn* Pawn = Cast<APawn>(Owner);
		if (!Pawn)
		{
			Controller = Cast<AEnemyAIController>(Pawn->GetController());	
		}
	}
	if (!Controller){ return;}
	const FVector location = Controller->LastKnownLocation;
	if (location.ContainsNaN()){ return;}
	UEnvQueryItemType_Point::SetContextHelper(ContextData, location);
}

