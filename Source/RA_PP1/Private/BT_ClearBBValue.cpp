// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_ClearBBValue.h"
#include "BehaviorTree/BlackboardComponent.h" 

UBT_ClearBBValue::UBT_ClearBBValue()
{
	NodeName = "Clear Blackboard Value";
}

EBTNodeResult::Type UBT_ClearBBValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (Blackboard)
	{
		Blackboard-> ClearValue(SelectedKey.SelectedKeyName);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
