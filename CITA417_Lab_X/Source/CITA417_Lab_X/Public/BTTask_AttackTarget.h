// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AttackTarget.generated.h"

/**
 * 
 */
UCLASS()
class CITA417_LAB_X_API UBTTask_AttackTarget : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	// Sets default node name shown inside the Behavior Tree editor
	UBTTask_AttackTarget();

protected:
	// Executes the attack decision and invokes Lab 09 combat on the controlled character
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
