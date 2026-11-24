// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

AEnemyAIController::AEnemyAIController()
{
	// Create the AI Perception component
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));

	// Create and configure the Sight sense
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	if (SightConfig)
	{
		// Set detection radius, lose-sight radius, and 90-degree half-angle (180-degree field of view)
		SightConfig->SightRadius = 1500.0f;
		SightConfig->LoseSightRadius = 1800.0f;
		SightConfig->PeripheralVisionAngleDegrees = 90.0f;

		// Allow sight to detect neutral, friendly, and enemy actors
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

		// Register Sight as the dominant sense on the perception component
		AIPerceptionComp->ConfigureSense(*SightConfig);
		AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
	}

	// Bind perception update callback
	if (AIPerceptionComp)
	{
		AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
	}
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	// Call parent OnPossess
	Super::OnPossess(InPawn);

	// Safely validate BehaviorTreeAsset before starting AI logic
	if (BehaviorTreeAsset)
	{
		// Start executing the assigned Behavior Tree (automatically initializes its Blackboard)
		RunBehaviorTree(BehaviorTreeAsset);

		// Write the configurable CombatAttackRange into the Blackboard
		if (GetBlackboardComponent())
		{
			GetBlackboardComponent()->SetValueAsFloat(TEXT("AttackRange"), CombatAttackRange);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyAIController: BehaviorTreeAsset is not assigned!"));
	}
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// Safely validate Blackboard and perceived actor
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (!BlackboardComp || !Actor)
	{
		return;
	}

	// Only react to player-controlled pawns so AI agents do not target walls or crates
	APawn* PerceivedPawn = Cast<APawn>(Actor);
	if (!PerceivedPawn || !PerceivedPawn->IsPlayerControlled())
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		// Store active player target and line-of-sight flag in shared Blackboard state
		BlackboardComp->SetValueAsObject(TEXT("TargetActor"), Actor);
		BlackboardComp->SetValueAsBool(TEXT("HasLineOfSight"), true);
		BlackboardComp->SetValueAsVector(TEXT("LastKnownLocation"), Actor->GetActorLocation());
	}
	else
	{
		// Mark line-of-sight lost and record final seen location for search behavior
		BlackboardComp->SetValueAsBool(TEXT("HasLineOfSight"), false);
		BlackboardComp->SetValueAsVector(TEXT("LastKnownLocation"), Stimulus.StimulusLocation);
	}
}