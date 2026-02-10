// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAI/EnemyCharacter/EnemyAnimInstance.h"
#include "GameFramework/Pawn.h"
#include "EnemyAI/AIController/EnemyAIController.h"


void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* Pawn = TryGetPawnOwner();

	MovementSpeed = Pawn->GetVelocity().Size();
	
	AAIController* AIController = Pawn->GetController<AAIController>();
	AEnemyAIController* EnemyAI = Cast<AEnemyAIController>(AIController);
	//AIState = EnemyAI->GetCurrentState();
}
