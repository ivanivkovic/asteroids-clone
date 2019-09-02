// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Floater.h" // Contains screen size constants
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ShipComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFirstShot);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASTEROIDS2_API UShipComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UShipComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AActor> ProjectileSpawn;

	UPROPERTY(BlueprintAssignable)
	FOnFirstShot OnFirstShot;

	UPROPERTY(EditAnywhere)  // 1 = ( 1 * (project fps) )° / sec
	float RotationSpeed = 4.5;

	UPROPERTY(EditAnywhere)
	float MaxFloatVelocity = 10;

	UPROPERTY(EditAnywhere)
	float AccelerateBy = 0.5f;

	UPROPERTY(EditAnywhere)
	float AccelerateEvery = 0.125f;

	UPROPERTY(EditAnywhere)
	float DecelerateEvery = 0.25f;

	UPROPERTY(EditAnywhere)
	float DecelerateBy = 0.25f;

	UPROPERTY(EditAnywhere)
	float ShootRange = 800;

	enum TurnDirectionOptions {
		None,
		Left,
		Right
	};

	TurnDirectionOptions TurnDirection = None;

	bool AccelerateNow = false;
	bool ShotsFired = false;
	float LastVelocityChange;
	float LastAcceleration;
	float FloatVelocity = 0;

	void Rotate();
	void SetTurnDirectionLeft();
	void SetTurnDirectionRight();
	void SetTurnDirectionNone();

	void Accelerate();
	void Decelerate();
	void StartAcceleration();
	void StopAcceleration();

	void Shoot();
	void Float();

	UInputComponent* InputComponent = nullptr;
	AActor* Owner = nullptr;
	FRotator Rotation;
	FVector Location;
	FVector Direction;

	void SetupInput();
};