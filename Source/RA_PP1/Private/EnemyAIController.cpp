// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"


AEnemyAIController::AEnemyAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerception");

	SetPerceptionComponent(*AIPerception);
	
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("SightConfig");
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>("HearingConfig");


	//Sight
	SightConfig->SightRadius = 2000.f;
	SightConfig->LoseSightRadius = 2500.f;
	SightConfig->PeripheralVisionAngleDegrees = 70.f;
	SightConfig->SetMaxAge(5.f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	//Hear
	HearingConfig->HearingRange = 1500.f;
	HearingConfig->SetMaxAge(3.f);


	//Perception

	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->ConfigureSense(*HearingConfig);
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());


	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnPerceptionUpdated);

}

void AEnemyAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!GetBlackboardComponent()) return;

	UE_LOG(LogTemp, Warning, TEXT("Perception: %s sensed=%d"),
	*Actor->GetName(),
	Stimulus.WasSuccessfullySensed()
);

	if (Stimulus.WasSuccessfullySensed())
	{
		GetBlackboardComponent()->SetValueAsObject("TargetActor", Actor);
		GetBlackboardComponent()->SetValueAsVector(
			"LastKnownLocation",
			Stimulus.StimulusLocation
		);
	}
	else
	{
		GetBlackboardComponent()->ClearValue("TargetActor");
	}
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	//Spawning enemy pawn
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(InPawn);

	if(!Enemy) return;
	
	if (UBehaviorTree* BT_Enemy = Enemy -> GetBehaviorTree())
	{
		RunBehaviorTree(BT_Enemy);
	}
	
}




