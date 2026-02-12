#include "EnemyAI/AIController/LastLocContext.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyAI/AIController/EnemyAIController.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"


void ULastLocContext::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	UObject* Owner = QueryInstance.Owner.Get();
	AEnemyAIController* Controller = Cast<AEnemyAIController>(QueryInstance.Owner.Get());
	if (!Controller)
	{
		if (APawn* Pawn = Cast<APawn>(QueryInstance.Owner.Get()))
		{
			Controller = Cast<AEnemyAIController>(Pawn->GetController());	
		}
	}
	if (!Controller){ return;}
	AAIController* AIController = Cast<AAIController>(Controller);
	UBlackboardComponent* BB = AIController->GetBlackboardComponent();
	const FVector location = BB->GetValueAsVector("LastKnownLocation");;
	if (location.ContainsNaN()){ return;}
	UEnvQueryItemType_Point::SetContextHelper(ContextData, location);
}

