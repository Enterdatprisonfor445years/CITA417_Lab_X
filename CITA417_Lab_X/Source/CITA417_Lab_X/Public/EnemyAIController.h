#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

UCLASS()
class CITA417_LAB_X_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values and configures AI Perception Sight
	AEnemyAIController();

protected:
	// Starts the Behavior Tree and initializes Blackboard values when possessing a pawn
	virtual void OnPossess(APawn* InPawn) override;

	// Perception component responsible for sensing actors in the world
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Perception")
	class UAIPerceptionComponent* AIPerceptionComp;

	// Sight configuration object defining vision radius and peripheral angle
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Perception")
	class UAISenseConfig_Sight* SightConfig;

	// Behavior Tree asset executed by this controller
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	class UBehaviorTree* BehaviorTreeAsset;

	// Configurable attack range written to the Blackboard (Independent Improvement)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float CombatAttackRange = 600.0f;

	// Callback triggered whenever sight perception gains or loses a target
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};

