// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"

// Sets default values
AWeaponActor::AWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create root scene component for socket attachment
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	// Create weapon static mesh and attach it to the root
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	// Disable weapon mesh collision so it never blocks character movement or traces
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeaponActor::Fire()
{
	// Enforce attack cooldown before executing a new trace
	if (!bCanFire)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Orange, TEXT("Weapon cooling down..."));
		}
		return;
	}

	// Safely validate owner and world references to prevent null pointer crashes
	AActor* WeaponOwner = GetOwner();
	UWorld* World = GetWorld();
	if (!WeaponOwner || !World)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponActor::Fire aborted due to missing Owner or World."));
		return;
	}

	// Lock firing and start the cooldown timer
	bCanFire = false;
	World->GetTimerManager().SetTimer(CooldownTimerHandle, this, &AWeaponActor::ResetFire, FireCooldown, false);

	// Retrieve the owner's eye/camera viewpoint and aim direction
	FVector EyeLocation;
	FRotator EyeRotation;
	WeaponOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	// Calculate trace end point along the aim direction
	FVector TraceEnd = EyeLocation + (EyeRotation.Vector() * AttackRange);

	// Configure trace parameters to ignore both the weapon and its owning character
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(WeaponOwner);
	QueryParams.bTraceComplex = true;

	// Execute single-line visibility trace
	FHitResult HitResult;
	bool bHit = World->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEnd, ECC_Visibility, QueryParams);

	// Determine start point for visual debug line (weapon mesh location if available)
	FVector MuzzleOrigin = WeaponMesh ? WeaponMesh->GetComponentLocation() : EyeLocation;

	if (bHit)
	{
		// Draw red debug trace line and impact sphere on hit
		DrawDebugLine(World, MuzzleOrigin, HitResult.ImpactPoint, FColor::Red, false, 1.5f, 0, 2.0f);
		DrawDebugSphere(World, HitResult.ImpactPoint, 12.0f, 12, FColor::Red, false, 1.5f);

		// Apply standard Unreal damage if a valid actor was struck
		AActor* HitActor = HitResult.GetActor();
		if (HitActor)
		{
			UGameplayStatics::ApplyDamage(HitActor, BaseDamage, WeaponOwner->GetInstigatorController(), this, UDamageType::StaticClass());
		}
	}
	else
	{
		// Draw green debug trace line when shot misses
		DrawDebugLine(World, MuzzleOrigin, TraceEnd, FColor::Green, false, 1.5f, 0, 1.5f);
	}
}

void AWeaponActor::ResetFire()
{
	// Allow the weapon to fire again once cooldown expires
	bCanFire = true;
}

// Called every frame
void AWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



}

