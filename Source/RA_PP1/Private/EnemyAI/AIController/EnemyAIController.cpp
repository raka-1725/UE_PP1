
#include "EnemyAIController.h"

#include "GameFramework/Pawn.h"
#include "Navigation/PathFollowingComponent.h"
#include "DrawDebugHelpers.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "Kismet/KismetMathLibrary.h"


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


	//Perception

	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->ConfigureSense(*HearingConfig);
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());


	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnPerceptionUpdated);
	
}
//Possess player
void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	SetState(EEnemyAIState::PatrolWaypoint);
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (CurrentState)
	{
	case EEnemyAIState::Idle:
		HandleIdle();
		break;
	case EEnemyAIState::PatrolWaypoint:
		HandlePatrolWaypoint();
		break;
	case EEnemyAIState::Patrol:
		HandlePatrol();
		break;
	case EEnemyAIState::Chase:
		HandleChase();
		break;
	case EEnemyAIState::Search:
		HandleSearch();
		break;
	default:
		break;
	}
}

void AEnemyAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		TargetActor = Actor;
		SetState(EEnemyAIState::Chase);
	}
	else if (Actor == TargetActor)
	{
		LastKnownLocation = Stimulus.StimulusLocation;
		TargetActor = nullptr;
		SetState(EEnemyAIState::Search);
	}
}

void AEnemyAIController::SetState(EEnemyAIState NewState)
{
	if (CurrentState == NewState)
		return;

	CurrentState = NewState;
	StopMovement();
}

void AEnemyAIController::HandleIdle()
{
	
}

void AEnemyAIController::HandlePatrolWaypoint()
{
	
}

void AEnemyAIController::HandlePatrol()
{
	if (GetMoveStatus() == EPathFollowingStatus::Moving)
		return;

	const FVector origin = GetPawn()->GetActorLocation();
	//Selecting random point to patrol
	const FVector PatrolPoint = UKismetMathLibrary::RandomPointInBoundingBox(origin,FVector(PatrolRadius));

	MoveToLocation(PatrolPoint);



	
#if WITH_EDITOR
	DrawDebugSphere(
		GetWorld(),
		PatrolPoint,
		50.f,
		12,
		FColor::Green,
		false,
		2.f
	);
#endif
}

void AEnemyAIController::HandleChase()
{
	if (!TargetActor)
	{
		SetState(EEnemyAIState::Search);
		return;	
	}

	MoveToActor(TargetActor, ChaseAcceptanceRadius);
}

void AEnemyAIController::HandleSearch()
{
	if (GetMoveStatus() == EPathFollowingStatus::Moving)
		return;
	FindHidingSpot();
	//MoveToLocation(LastKnownLocation);

#if WITH_EDITOR
	DrawDebugSphere(
		GetWorld(),
		LastKnownLocation,
		75.f,
		16,
		FColor::Yellow,
		false,
		2.f
	);
#endif
	
	SetState(EEnemyAIState::Patrol);
}

//EQS
void AEnemyAIController::FindHidingSpot()
{
	FEnvQueryRequest SpotQueryRequest = FEnvQueryRequest(EnemyEQS, GetPawn());
	SpotQueryRequest.Execute(EEnvQueryRunMode::SingleResult, this, &AEnemyAIController::OnEQSFinished);
}

void AEnemyAIController::OnEQSFinished(TSharedPtr<FEnvQueryResult, ESPMode::ThreadSafe> Result)
{
	FVector HideLocation = Result->GetItemAsLocation(0);
	MoveToLocation(HideLocation);

#if WITH_EDITOR
	DrawDebugSphere(GetWorld(), HideLocation, 50.f, 12, FColor::Yellow, false, 2.f);
	#endif
}





