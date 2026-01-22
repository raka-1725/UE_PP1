// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"


AEnemyAIController::AEnemyAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerception");

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("SightConfig");
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>("HearingConfig");


	//Sight
	SightConfig->SightRadius = 200.f;
	SightConfig->LoseSightRadius = 2200.f;
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


