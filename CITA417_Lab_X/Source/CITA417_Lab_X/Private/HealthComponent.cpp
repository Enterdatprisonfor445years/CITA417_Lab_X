// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Initialize current health to full capacity at the start of play
	CurrentHealth = MaxHealth;

	// Safely retrieve the owning actor and bind to its damage event
	AActor* Owner = GetOwner();
	if (Owner)
	{
		// Ensure the owning actor is allowed to receive Unreal damage events
		Owner->SetCanBeDamaged(true);
		// Register HandleTakeAnyDamage to run whenever Unreal's ApplyDamage hits this actor
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
	}
	else
	{
		// Log a warning if the component somehow has no valid owner
		UE_LOG(LogTemp, Warning, TEXT("HealthComponent has no valid Owner in BeginPlay."));
	}
	
}

void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// Ignore invalid/zero damage or repeated damage after target is already dead
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}

	// Subtract incoming damage and clamp health between 0 and MaxHealth
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);

	// Print live damage feedback to the screen for testing and demo verification
	if (GEngine && DamagedActor)
	{
		FString HealthMsg = FString::Printf(
			TEXT("%s took %.1f damage | Health: %.1f / %.1f"),
			*DamagedActor->GetName(),
			Damage,
			CurrentHealth,
			MaxHealth
		);
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Yellow, HealthMsg);
	}

	// Notify any bound UI or feedback listeners that health changed
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, Damage);

	// Check if health has reached zero to trigger the death/disabled state once
	if (CurrentHealth <= 0.0f)
	{
		// Lock bIsDead to true so death logic never fires twice
		bIsDead = true;

		// Notify the owning target to enter its death/disabled state
		OnDeath.Broadcast();
	}
}

// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

