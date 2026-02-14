// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "EnemyCharacter.generated.h"

class UBehaviorTree;

UCLASS()
class RA_PP1_API AEnemyCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	//Team ID
	FGenericTeamId TeamId;

	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	virtual FGenericTeamId GetGenericTeamId() const override {return TeamId;}
	void SetTeanID(uint8 ID) {TeamId = FGenericTeamId(ID);}
};
