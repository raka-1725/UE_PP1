#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "EnemyAIController.generated.h"


class UEnvQuery;

UENUM(BlueprintType)
enum class EEnemyAIState : uint8
{
	Idle,
	Patrol,
	PatrolWaypoint,
	Search,
	Chase,
};

UCLASS()
class RA_PP1_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
	EEnemyAIState GetCurrentState() const { return CurrentState; }

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	FVector LastKnownLocation;
protected:

	//Perception

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerception;

	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY()
	UAISenseConfig_Hearing* HearingConfig;

	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	//AI STATE
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	EEnemyAIState CurrentState = EEnemyAIState::Idle;

	UPROPERTY()
	AActor* TargetActor;


	//Settings
	UPROPERTY(EditAnywhere, Category = "AISettings")
	float ChaseAcceptanceRadius = 15.0f;

	UPROPERTY(EditAnywhere, Category = "AISettings")
	float SearchRadius = 600.0f;

	UPROPERTY(EditAnywhere, Category = "AISettings")
	float PatrolRadius = 1000.0f;

	//State
	void SetState(EEnemyAIState Newstate);

	void HandleIdle();
	void HandlePatrolWaypoint();
	void HandlePatrol();
	void RunSearchEQS();
	void HandleSearch();
	void HandleChase();

	
	//Search Logic
	int MaxSearchAttempts = 0;
	int SearchAttempts = 0;
	float TimeSinceLost = 0.0f;
	float SearchDuration = 5.0f;
	bool bPlayerVisible = false;


	//EQS
	UPROPERTY(EditAnywhere, Category = "AI")
	UEnvQuery *EnemyEQS;
	
	UPROPERTY(EditAnywhere, Category="AI")
	UEnvQuery* SearchEQS;
	UPROPERTY(EditAnywhere, Category="AI")
	UEnvQuery* PatrolEQS;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void FindHidingSpot();
	void OnEQSFinished(TSharedPtr<FEnvQueryResult, ESPMode::ThreadSafe> Result);
	void RunPatrolEQS();
	void OnPatrolEQSFinished(TSharedPtr<FEnvQueryResult, ESPMode::ThreadSafe> Result);
};
