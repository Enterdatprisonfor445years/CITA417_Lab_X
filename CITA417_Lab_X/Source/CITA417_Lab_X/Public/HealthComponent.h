// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

// Broadcasts whenever the owning actor takes damage and health changes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChangedSignature, float, CurrentHealth, float, MaxHealth, float, DamageAmount);

// Broadcasts once when CurrentHealth reaches zero
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CITA417_LAB_X_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	// Sets default values for this component's properties
	UHealthComponent();

	// Event dispatcher triggered when health changes
	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FOnHealthChangedSignature OnHealthChanged;

	// Event dispatcher triggered once when health reaches zero
	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FOnDeathSignature OnDeath;

	// Returns true if the target has already entered the death/disabled state
	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const { return bIsDead; }
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Maximum health capacity for the owning target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.0f;

	// Current remaining health of the owning target
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 0.0f;

	// Tracks whether this target has already died so death triggers only once
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	bool bIsDead = false;

	// Callback function bound to the owning actor's OnTakeAnyDamage event
	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
