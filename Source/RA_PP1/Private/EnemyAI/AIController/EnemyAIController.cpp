
#include "EnemyAIController.h"

#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Perception/AISenseEvent.h"


AEnemyAIController::AEnemyAIController()
{
	//AI perception
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerception");

	SetPerceptionComponent(*AIPerception);
	
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));


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
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	
	//Perception

	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->ConfigureSense(*HearingConfig);
	


	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnPerceptionUpdated);
	
}
//Possess player
void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	RunBehaviorTree(BT_Enemy);
	SetState(EEnemyAIState::Patrol);
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bPlayerVisible)
	{
		TimeSinceSeen += DeltaTime;
		//UE_LOG(LogAIPerception,Warning,TEXT("TImer since lost: %f"),TimeSinceLost);
	}
	else
	{
		TimeSinceSeen = 0.0f;
	}
	
	if (!bPlayerVisible && CurrentState == EEnemyAIState::Chase && TimeSinceSeen > SightTransitionSec)
	{
		SetState(EEnemyAIState::Search);
	}
}

void AEnemyAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	BB->SetValueAsEnum("EnemyAIState", (uint8)EEnemyAIState::Patrol);
	
	//Sight percept
	if (bSightPerception && Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		bPlayerVisible = Stimulus.WasSuccessfullySensed();
		if (bPlayerVisible)
		{
			TargetActor = Actor;
			BB->SetValueAsObject("TargetActor", Actor);
			BB->SetValueAsEnum("EnemyAIState", (uint8)EEnemyAIState::Chase);
			
		}
		else
		{
			TargetActor = nullptr;
			BB->ClearValue("TargetActor");
			BB->SetValueAsVector("LastKnownLocation", Stimulus.StimulusLocation);
			BB->SetValueAsEnum("EnemyAIState", (uint8)EEnemyAIState::Search);
		}
	}
	//Hear
	else if (bHearPerception && Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		if (!bPlayerVisible)
		{
			BB->SetValueAsVector("LastKnownLocation", Stimulus.StimulusLocation);
		}
		#if WITH_EDITOR
				DrawDebugSphere(GetWorld(), Stimulus.StimulusLocation, 50.f, 12, FColor::Magenta, false, 2.f);
		#endif
	}

}

void AEnemyAIController::SetState(EEnemyAIState NewState)
{
	if (CurrentState == NewState)
		return;

	CurrentState = NewState;
	 UBlackboardComponent* BB = GetBlackboardComponent();
	BB->SetValueAsEnum("EnemyAIState", (uint8)NewState);
}

void AEnemyAIController::RunEQS()
{
	if (!bUseEQS) return;
	FEnvQueryRequest Query(EQS, GetPawn());
	Query.Execute(EEnvQueryRunMode::SingleResult, this, &AEnemyAIController::OnEQSFinished);
}

void AEnemyAIController::OnEQSFinished(TSharedPtr<FEnvQueryResult, ESPMode::ThreadSafe> result)
{
	//EQS 0 return check
	if (!result.IsValid()){UE_LOG(LogAIPerception,Verbose,TEXT("EQS returned no result")); return;}
	
	FVector location = result->GetItemAsLocation(0);

	UBlackboardComponent* BB = GetBlackboardComponent();
	BB->SetValueAsVector("EQSLocation", location);

#if WITH_EDITOR
	DrawDebugSphere(GetWorld(), location, 50.f, 12, FColor::Yellow, false, 2.f);
	#endif
}






