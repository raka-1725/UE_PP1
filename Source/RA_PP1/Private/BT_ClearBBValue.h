// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_ClearBBValue.generated.h"

UCLASS()
class RA_PP1_API UBT_ClearBBValue : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBT_ClearBBValue();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector SelectedKey;
};
