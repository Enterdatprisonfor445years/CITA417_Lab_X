#include "BTTask_AttackTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CITA417_Lab_XCharacter.h"

UBTTask_AttackTarget::UBTTask_AttackTarget()
{
	// Set display name for the Behavior Tree graph
	NodeName = TEXT("Attack Target (Lab 09 Reuse)");
}

EBTNodeResult::Type UBTTask_AttackTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Retrieve the AI Controller and Blackboard safely
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!AIController || !BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	// Cast the controlled pawn to our shared Lab 08/09 Character class
	ACITA417_Lab_XCharacter* AICharacter = Cast<ACITA417_Lab_XCharacter>(AIController->GetPawn());
	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("TargetActor")));
	if (!AICharacter || !TargetActor)
	{
		return EBTNodeResult::Failed;
	}

	// Read configurable AttackRange from Blackboard and check distance to target
	float AllowedRange = BlackboardComp->GetValueAsFloat(TEXT("AttackRange"));
	float CurrentDistance = FVector::Dist(AICharacter->GetActorLocation(), TargetActor->GetActorLocation());

	if (CurrentDistance > AllowedRange)
	{
		// Fail task if target is outside attack range so Behavior Tree pursues closer
		return EBTNodeResult::Failed;
	}

	// Rotate AI character smoothly to face the target before firing
	FVector LookDirection = (TargetActor->GetActorLocation() - AICharacter->GetActorLocation()).GetSafeNormal2D();
	if (!LookDirection.IsNearlyZero())
	{
		AICharacter->SetActorRotation(LookDirection.Rotation());
	}

	// Reuse Lab 09 Attack() pipeline (handles cooldowns, line trace, and ApplyDamage)
	AICharacter->Attack();

	// Return Succeeded so the Behavior Tree sequence completes cleanly
	return EBTNodeResult::Succeeded;
}
