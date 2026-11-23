// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "CITA417_Lab_XCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class ACITA417_Lab_XCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;
	
	// Sprint Input Action
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SprintAction;

	// Attack Input Action
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* AttackAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;



public:

	/** Constructor */
	ACITA417_Lab_XCharacter();

	// Called every fram to update the player's movement and animation state.
	virtual void Tick(float DeltaTime) override;

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	// Called when the game starts to spawn and equip the default weapon
	virtual void BeginPlay() override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	// Current horizontal ground speed of the character in cm/s
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State")
	float Speed = 0.0f;

	// Movement angle relative to the character's forward facing direction (-180 to 180 degrees)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State")
	float Direction = 0.0f;

	// True when the character is jumping or falling in mid-air
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State")
	bool bIsAirborne = false;

	// True when the player is actively holding the sprint input
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State")
	bool bIsSprinting = false;

	// Default movement speed when walking or jogging
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement Settings")
	float WalkSpeed = 500.0f;

	// Increased movement speed when sprinting
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement Settings")
	float SprintSpeed = 800.0f;

	// Blueprint weapon class to spawn and attach at game start
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<class AWeaponActor> WeaponClass;

	// Reference to the currently equipped weapon instance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AWeaponActor* EquippedWeapon;

	// True briefly when the character triggers an attack (readable by AnimBP)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State")
	bool bIsAttacking = false;

	// Socket name on the skeletal mesh where the weapon attaches
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	FName WeaponSocketName = TEXT("hand_r");

	// Timer handle to reset bIsAttacking after the attack pulse
	FTimerHandle AttackAnimTimerHandle;

	// Handles player attack input and delegates firing to EquippedWeapon
	void Attack();

	// Resets bIsAttacking flag back to false
	void ResetAttackState();

	// Updates Speed, Direction, and bIsAirborne every frame
	void UpdateCharacterState();

	// Enables sprinting state and increases MaxWalkSpeed
	void StartSprinting();

	// Disables sprinting state and resets MaxWalkSpeed
	void StopSprinting();

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

