
#include "EnemyAIController.h"

#include "GameFramework/Pawn.h"
#include "Navigation/PathFollowingComponent.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "Kismet/KismetMathLibrary.h"
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
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
	AIPerception->SetDominantSense(HearingConfig->GetSenseImplementation());


	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnPerceptionUpdated);
	
}
//Possess player
void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	SetState(EEnemyAIState::Patrol);
	RunBehaviorTree(BT_Enemy);
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
	if (!bPlayerVisible && CurrentState == EEnemyAIState::Chase && TimeSinceSeen > SightTransitionSeconds)
	{
		SetState(EEnemyAIState::Search);
	}

	/*
	switch (CurrentState)
	{
	case EEnemyAIState::Idle:
		HandleIdle();
		UE_LOG(LogAIPerception,Verbose,TEXT("Idle"));
		break;
	case EEnemyAIState::Patrol:
		HandlePatrol();
		UE_LOG(LogAIPerception,Verbose,TEXT("Patrol"));
		break;
	case EEnemyAIState::Chase:
		HandleChase();
		UE_LOG(LogAIPerception,Verbose,TEXT("Chase"));
		break;
	case EEnemyAIState::Search:
		HandleSearch();
		UE_LOG(LogAIPerception,Verbose,TEXT("Search"));
		break;
	default:
		break;
	}
	*/
}

void AEnemyAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	
	//Sight percep
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		bPlayerVisible = Stimulus.WasSuccessfullySensed();
		if (bPlayerVisible)
		{
			BB->SetValueAsObject("TargetActor", Actor);
			BB->SetValueAsEnum("State", (uint8)EEnemyAIState::Chase);
			
		}
		else
		{
			BB->ClearValue("TargetActor");
			BB->SetValueAsVector("TargetLocation", Stimulus.StimulusLocation);
			BB->SetValueAsEnum("AIState", (uint8)EEnemyAIState::Search);
		}
	}
	//Hear
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
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
	//StopMovement();
}

void AEnemyAIController::HandleIdle()
{
	
}

void AEnemyAIController::HandlePatrol()
{
	if (GetMoveStatus() != EPathFollowingStatus::Idle)
		return;

	RunPatrolEQS();
}

void AEnemyAIController::HandleChase()
{
	if (!TargetActor)
	{
		SetState(EEnemyAIState::Search);
		return;	
	}

	//MoveToActor(TargetActor, ChaseAcceptanceRadius);
}

void AEnemyAIController::HandleSearch()
{
	if (bPlayerVisible){SetState(EEnemyAIState::Chase); return;}
	if (SearchAttempts < MaxSearchAttempts && GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		RunSearchEQS();
		SearchAttempts++;
		return;
	}

	if (SearchAttempts >= MaxSearchAttempts)
	{
		GetPawn()->AddControllerYawInput(60.0f * GetWorld()->DeltaTimeSeconds);
		UE_LOG(LogAIPerception,Verbose,TEXT("Rotate to scan"));
	}

	if (TimeSinceSeen > SearchDuration)
	{
		SearchAttempts = 0;
		SetState(EEnemyAIState::Patrol);
	}
}

//EQS
void AEnemyAIController::FindHidingSpot()
{
	FEnvQueryRequest SpotQueryRequest = FEnvQueryRequest(EnemyEQS, GetPawn());
	SpotQueryRequest.Execute(EEnvQueryRunMode::SingleResult, this, &AEnemyAIController::OnEQSFinished);
}

void AEnemyAIController::RunSearchEQS()
{
	FEnvQueryRequest Query(SearchEQS, GetPawn());
	Query.Execute(EEnvQueryRunMode::SingleResult, this, &AEnemyAIController::OnEQSFinished);
}

void AEnemyAIController::OnEQSFinished(TSharedPtr<FEnvQueryResult, ESPMode::ThreadSafe> result)
{
	//EQS 0 return check
	if (!result.IsValid()){UE_LOG(LogAIPerception,Verbose,TEXT("EQS returned no result")); return;}

	
	FVector location = result->GetItemAsLocation(0);
	//MoveToLocation(location);

#if WITH_EDITOR
	DrawDebugSphere(GetWorld(), location, 50.f, 12, FColor::Yellow, false, 2.f);
	#endif
}

void AEnemyAIController::RunPatrolEQS()
{
	FEnvQueryRequest Query(PatrolEQS, GetPawn());
	Query.Execute(EEnvQueryRunMode::SingleResult, this, &AEnemyAIController::OnPatrolEQSFinished);
}

void AEnemyAIController::OnPatrolEQSFinished(TSharedPtr<FEnvQueryResult, ESPMode::ThreadSafe> result)
{
	FVector location = result->GetItemAsLocation(0);
	//MoveToLocation(location);

#if WITH_EDITOR
	DrawDebugSphere(GetWorld(), location, 50.f, 12, FColor::Blue, false, 2.f);
#endif
}





