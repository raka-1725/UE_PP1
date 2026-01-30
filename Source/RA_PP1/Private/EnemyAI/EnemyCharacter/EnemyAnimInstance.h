// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAI/AIController/EnemyAIController.h"
#include "EnemyAnimInstance.generated.h"

UCLASS()
class UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()


	public:
	UPROPERTY(BlueprintReadOnly, Category = "AI")
	EEnemyAIState AIState;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float MovementSpeed;

	protected:
	virtual void NativeUpdateAnimation(float DeltaTime) override;
};
