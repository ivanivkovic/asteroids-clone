// Fill out your copyright notice in the Description page of Project Settings.

#include "ShipComponent.h"

// Sets default values for this component's properties
UShipComponent::UShipComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UShipComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();

	Rotation = Owner->GetActorRotation();
	Location = Owner->GetActorLocation();
	Direction = Owner->GetActorForwardVector();

	UE_LOG(LogTemp, Error, TEXT("%s has begun playing"), *GetOwner()->GetName());

	SetupInput();
}

void UShipComponent::SetTurnDirectionLeft() { TurnDirection = Left; }
void UShipComponent::SetTurnDirectionRight() { TurnDirection = Right; }
void UShipComponent::SetTurnDirectionNone() { TurnDirection = None; }


// If direction is set, perform rotation based on turn direction and rotation speed.
void UShipComponent::Rotate()
{
	switch (TurnDirection)
	{
	case Left:
		Rotation.Yaw = Rotation.Yaw - (1.0f * RotationSpeed);
		break;

	case Right:
		Rotation.Yaw = Rotation.Yaw + (1.0f * RotationSpeed);
		break;

	default:
		return;
	}

	Owner->SetActorRotation(Rotation);
}

void UShipComponent::Shoot()
{
	// Broadcast initial shoot event to Blueprint, so that we may remove the intro HUD.
	if (!ShotsFired)
	{
		OnFirstShot.Broadcast();
		ShotsFired = true;

		UE_LOG(LogTemp, Warning, TEXT("First shot, closing Intro HUD"));
	}

	if (ProjectileSpawn)
	{
		UWorld* World = GetWorld();

		if (World)
		{
			FActorSpawnParameters SpawnParams;

			FVector SpawnLocation = Location + (Owner->GetActorForwardVector() * 55);
			AActor* SpawnedActor = World->SpawnActor<AActor>(ProjectileSpawn, SpawnLocation, Rotation, SpawnParams);

			//UE_LOG(LogTemp, Warning, TEXT("Projectile spawned %s at location: %f, %f, %f"), *SpawnedActor->GetName(), SpawnedActor->GetActorLocation().X, SpawnedActor->GetActorLocation().Y, SpawnedActor->GetActorLocation().Z);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Projectile Spawn not set in Ship_BP!"));
	}
}

void UShipComponent::Accelerate()
{
	float Seconds = GetWorld()->GetTimeSeconds();

	// If user triggered acceleration, ship speed is under the limit, and if it passed "AccelerateEvery seconds" interval, accelerate float speed.
	if (
		AccelerateNow &&
		FloatVelocity < MaxFloatVelocity &&
		Seconds - LastAcceleration > AccelerateEvery
		)
	{
		// If passing MaxFloatVelocity, set the value to MaxFloatVelocity, else increment by AccelerateBy value.
		if (FloatVelocity + AccelerateBy > MaxFloatVelocity)
		{
			FloatVelocity = MaxFloatVelocity;
		}
		else
		{
			FloatVelocity += AccelerateBy;
		}

		Direction = Owner->GetActorForwardVector();
		LastAcceleration = Seconds;

		//UE_LOG(LogTemp, Warning, TEXT("%f, %f"), Direction.X, Direction.Y);
	}
}

void UShipComponent::StartAcceleration()
{
	AccelerateNow = true;
}

void UShipComponent::StopAcceleration()
{
	AccelerateNow = false;
}

void UShipComponent::SetupInput()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();

	if (InputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has input component"), *GetOwner()->GetName());

		InputComponent->BindAction("Shoot", IE_Pressed, this, &UShipComponent::Shoot);
		InputComponent->BindAction("RotateLeft", IE_Pressed, this, &UShipComponent::SetTurnDirectionLeft);
		InputComponent->BindAction("RotateRight", IE_Pressed, this, &UShipComponent::SetTurnDirectionRight);
		InputComponent->BindAction("RotateLeft", IE_Released, this, &UShipComponent::SetTurnDirectionNone);
		InputComponent->BindAction("RotateRight", IE_Released, this, &UShipComponent::SetTurnDirectionNone);
		InputComponent->BindAction("Accelerate", IE_Pressed, this, &UShipComponent::StartAcceleration);
		InputComponent->BindAction("Accelerate", IE_Released, this, &UShipComponent::StopAcceleration);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing input component"), *GetOwner()->GetName());
	}
}

// Float the ship each frame.
void UShipComponent::Float()
{
	Location = Location + (Direction * FloatVelocity);

	// Teleport object to opposite side in case it's out of screen range. (less elegant)
	if (Location.X < ScreenXMin)
	{
		Location.X = ScreenXMax;
	}
	else if (Location.X > ScreenXMax)
	{
		Location.X = ScreenXMin;
	}

	if (Location.Y < ScreenYMin)
	{
		Location.Y = ScreenYMax;
	}
	else if (Location.Y > ScreenYMax)
	{
		Location.Y = ScreenYMin;
	}

	Owner->SetActorLocation(Location);
}

// Each FloatVelocityFadeEachSeconds seconds we will decelerate the ship by FloatVelocityFadeBy
void UShipComponent::Decelerate()
{
	float Seconds = GetWorld()->GetTimeSeconds();

	if (Seconds - LastVelocityChange > DecelerateEvery && FloatVelocity > 0)
	{
		// In case FloatVelocity is 1.5 and FloatVelocityFadeBy is 2, set FloatVelocity to 0.
		if (DecelerateBy > FloatVelocity)
		{
			FloatVelocity = 0;
		}
		// Else normally subtract.
		else
		{
			FloatVelocity -= DecelerateBy;
		}

		LastVelocityChange = Seconds;
	}
}

void UShipComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	Rotate();
	Float();
	Accelerate();
	Decelerate();
}