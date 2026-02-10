#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
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
	

	//Toggles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bSightPerception;
	bool bHearPerception;
	bool bUseEQS;
	
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

	
	//AI STATE
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	EEnemyAIState CurrentState = EEnemyAIState::Idle;

	UPROPERTY()
	AActor* TargetActor;

	//EQS
	UPROPERTY(EditAnywhere, Category = "AI | EQS")
	UEnvQuery* EQS;

	UFUNCTION(BlueprintCallable, Category = "AI | EQS")
	void RunEQS();
	void OnEQSFinished(TSharedPtr<FEnvQueryResult> EnvQueryResult);



	
	//Search
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI | EQS")
	bool bPlayerVisible = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI | EQS")
	float TimeSinceSeen = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | EQS")
	float SightTransitionSec = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | EQS")
	float SearchDuration = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | EQS")
	int MaxSearchAttempt = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | EQS")
	int SearchAttempts = 0;
	
	//Settings

	UPROPERTY(EditAnywhere, Category = "AISettings")
	float ChaseAcceptanceRadius = 15.0f;

	UPROPERTY(EditAnywhere, Category = "AISettings")
	float SearchRadius = 600.0f;

	UPROPERTY(EditAnywhere, Category = "AISettings")
	float PatrolRadius = 1000.0f;


public:
	void SetState(EEnemyAIState NewState);
};
