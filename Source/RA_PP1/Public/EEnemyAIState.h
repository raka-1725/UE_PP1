// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EEnemyAIState.generated.h"

UENUM(BlueprintType)
enum class EEnemyAIState : uint8
{
	Idle,
	Patrol,
	Search,
	Chase,
};