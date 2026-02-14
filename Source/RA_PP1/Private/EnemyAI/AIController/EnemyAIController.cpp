
#include "EnemyAIController.h"

#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"
#include "InterchangeResult.h"
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
	SightConfig->SetMaxAge(5.0f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	//Hear
	HearingConfig->HearingRange = 2000.f;
	HearingConfig->SetMaxAge(5.0f);
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

	if (UBlackboardComponent* BB = GetBlackboardComponent()){BB->ClearValue("LastKnownLocation");}
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

	if (CurrentState == EEnemyAIState::Search)
	{
		SearchTimer += DeltaTime;
		if (SearchTimer >= SearchDuration)
		{
			SearchTimer = 0.0f;
			SetState(EEnemyAIState::Patrol);
		}
	}
	else
	{
		SearchTimer = 0.0f;
	}
}

void AEnemyAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	
	//Sight percept
	if (bSightPerception && Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		bPlayerVisible = Stimulus.WasSuccessfullySensed();
		if (bPlayerVisible)
		{
			GetBlackboardComponent()->SetValueAsObject("TargetActor", Actor);
			SearchAttempts = 0;
			SetState(EEnemyAIState::Chase);
			BB->SetValueAsVector("LastKnownLocation", Actor->GetActorLocation());
			
		}
		else
		{
			//add timer here for grace period
			TargetActor = nullptr;
			//BB->ClearValue("TargetActor");
			BB->SetValueAsVector("LastKnownLocation", Stimulus.StimulusLocation);
			SetState(EEnemyAIState::Search);
		}
	}
	//Hear
	else if (bHearPerception && Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Hear sound at %s"), *Stimulus.StimulusLocation.ToString());
		if (Stimulus.WasSuccessfullySensed())
		{
			BB->SetValueAsVector("LastKnownLocation", Stimulus.StimulusLocation);
			BB->SetValueAsVector("EQSLocation", Stimulus.StimulusLocation);

			if (CurrentState == EEnemyAIState::Patrol)
			{
				SetState(EEnemyAIState::Search);
			}

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
	BB->SetValueAsEnum("EnemyAIState", (uint8(NewState)));

	//UE_LOG(LogTemp, Warning, TEXT("GripState: %i"), CurrentState);
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






