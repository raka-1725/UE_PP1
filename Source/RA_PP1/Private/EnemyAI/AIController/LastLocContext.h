// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "LastLocContext.generated.h"

UCLASS()
class RA_PP1_API ULastLocContext : public UEnvQueryContext
{
	GENERATED_BODY()
public:	
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;

};
