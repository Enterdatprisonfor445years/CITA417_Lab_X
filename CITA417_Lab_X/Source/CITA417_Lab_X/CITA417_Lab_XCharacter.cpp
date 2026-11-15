// Copyright Epic Games, Inc. All Rights Reserved.

#include "CITA417_Lab_XCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "CITA417_Lab_X.h"

ACITA417_Lab_XCharacter::ACITA417_Lab_XCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Enables the %Tick so the player state variables update every frame.
	PrimaryActorTick.bCanEverTick = true;

	// Setting the intial movement speed to the WalkSpeed variable.
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ACITA417_Lab_XCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACITA417_Lab_XCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ACITA417_Lab_XCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACITA417_Lab_XCharacter::Look);

		// Binding Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ACITA417_Lab_XCharacter::StartSprinting);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACITA417_Lab_XCharacter::StopSprinting);
	}
	else
	{
		UE_LOG(LogCITA417_Lab_X, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ACITA417_Lab_XCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ACITA417_Lab_XCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ACITA417_Lab_XCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ACITA417_Lab_XCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ACITA417_Lab_XCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void ACITA417_Lab_XCharacter::DoJumpEnd()
{
	// Signals the character to stop jumping.
	StopJumping();
}

void ACITA417_Lab_XCharacter::Tick(float DeltaTime)
{
	// Calling the parent class Tick behavior.
	Super::Tick(DeltaTime);

	// Recalculating Speed, Direction, and bIsAirborne every frame.
	UpdateCharacterState();
}

void ACITA417_Lab_XCharacter::UpdateCharacterState()
{
	// Retrieving the character's current velocity vector.
	FVector Velocity = GetVelocity();

	// Zero out the Z axis so jumping or falling does not affect horizontal ground speed.
	Velocity.Z = 0.0f;

	// Calculate horizontal ground speed in cm/s
	Speed = Velocity.Size();

	// Checks the CharacterMovementComponent to determine if the character is in mid-air.
	if (GetCharacterMovement())
	{
		bIsAirborne = GetCharacterMovement()->IsFalling();
	}

	// Calculates movement angle relative to forward direction when moving.
	if (Speed > 3.0f)
	{
		// Normalizing the velocity to get a pure direction unit vector.
		FVector MoveDir = Velocity.GetSafeNormal();

		// Getting the player's forward and right facing vectors.
		FVector Forward = GetActorForwardVector();
		FVector Right = GetActorRightVector();

		// Projecting movement direction onto forward and right axes.
		float ForwardDot = FVector::DotProduct(Forward, MoveDir);
		float RightDot = FVector::DotProduct(Right, MoveDir);

		// Computing signed angle in degrees.
		Direction = FMath::RadiansToDegrees(FMath::Atan2(RightDot, ForwardDot));
	}
	else
	{
		// Reseting the angle to zero when standing still.
		Direction = 0.0f;
	}

	// Displaying live animation input state on screen for debugging verification.
	if (GEngine)
	{
		FString DebugMsg = FString::Printf(
			TEXT("Speed: %.1f | Direction: %.1f | Airborne: %s | Sprinting: %s"),
			Speed,
			Direction,
			bIsAirborne ? TEXT("TRUE") : TEXT("FALSE"),
			bIsSprinting ? TEXT("TRUE") : TEXT("FALSE")
		);

		// Key 1 ensures the message updates in place rather than flooding the screen.
		GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Cyan, DebugMsg);
	}
}

void ACITA417_Lab_XCharacter::StartSprinting()
{
	// Set sprinting flag to true
	bIsSprinting = true;

	// Increasing character movement speed to SprintSpeed.
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void ACITA417_Lab_XCharacter::StopSprinting()
{
	// Set sprinting flag to false
	bIsSprinting = false;

	// Reset character movement speed back to WalkSpeed.
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}