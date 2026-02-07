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

	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	FVector LastKnownLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBehaviorTree* BT_Enemy;
protected:

	//Perception

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	UAIPerceptionComponent* AIPerception;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	UAISenseConfig_Hearing* HearingConfig;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	void UpdateBBState(EEnemyAIState NewState);

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
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetState(EEnemyAIState Newstate);
	
	void HandleIdle();
	void HandlePatrol();
	void RunSearchEQS();
	void HandleSearch();
	void HandleChase();

	
	//Search Logic


	//EQS
	UPROPERTY(EditAnywhere, Category = "AI")
	UEnvQuery *EnemyEQS;
	
	UPROPERTY(EditAnywhere, Category="AI")
	UEnvQuery* SearchEQS;
	UPROPERTY(EditAnywhere, Category="AI")
	UEnvQuery* PatrolEQS;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI/EQS")
	int MaxSearchAttempts = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI/EQS")
	int SearchAttempts = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI/EQS")
	float TimeSinceLost = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI/EQS")
	float TimeSinceSeen = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI/EQS")
	float SightTransitionSeconds = 1.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI/EQS")
	float SearchDuration = 5.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI/EQS")
	bool bPlayerVisible = false;
	



	UFUNCTION(BlueprintCallable, Category = "AI")
	void FindHidingSpot();
	
	void OnEQSFinished(TSharedPtr<FEnvQueryResult, ESPMode::ThreadSafe> Result);
	void RunPatrolEQS();
	void OnPatrolEQSFinished(TSharedPtr<FEnvQueryResult, ESPMode::ThreadSafe> Result);
};
