// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponActor.generated.h"

UCLASS()
class CITA417_LAB_X_API AWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponActor();

	// Public attack entry point called by the owning character
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Fire();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Root scene component for clean attachment to character sockets
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RootScene;

	// Visual mesh representing the weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* WeaponMesh;

	// Base damage applied to any valid hit actor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat")
	float BaseDamage = 25.0f;

	// Maximum trace distance in centimeters
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat")
	float AttackRange = 2000.0f;

	// Cooldown duration in seconds between consecutive attacks (Independent Improvement)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat")
	float FireCooldown = 0.4f;

	// Tracks whether the weapon is ready to fire again
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Combat")
	bool bCanFire = true;

	// Timer handle used to reset bCanFire after FireCooldown elapses
	FTimerHandle CooldownTimerHandle;

	// Re-enables firing once the cooldown period completes
	void ResetFire();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
