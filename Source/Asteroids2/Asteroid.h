// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Floater.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "ConstructorHelpers.h"
#include "Asteroid.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ASTEROIDS2_API AAsteroid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAsteroid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	enum SizeOptions {
		Large,
		Medium,
		Small
	};

	// By default
	SizeOptions Size = Large;

	UPROPERTY(EditAnywhere)
	int AsteroidChildrenCount = 3;

	UPROPERTY(EditAnywhere)
	float LargeScale = 1.0f;

	UPROPERTY(EditAnywhere)
	float MediumScale = 0.7f;

	UPROPERTY(EditAnywhere)
	float SmallScale = 0.4f;

	UPROPERTY(EditAnywhere)
	float RotationSpeed = 3.0f;

	UPROPERTY(EditAnywhere)
	int StartAsteroids = 3;

	UPROPERTY(EditAnywhere)
	int MaxAsteroids = 10;

	UPROPERTY(EditAnywhere)
	int MinSpawnDistanceRadius = 100;

	UPROPERTY(EditAnywhere)
	float AccelerateBy = 0.25f;

	UPROPERTY(EditAnywhere)
	float FloatVelocity = 1.5f;

	UPROPERTY(EditAnywhere)
	float FloatVelocityIncrement = 0.75f;

	float SpawnTime;

	void Accelerate();
	void Float();
	void Rotate();

	void SetRandomRotation();
	void SetRandomRotationDirection();
	void SetFloatDirection();

	void SpawnChildAsteroid(SizeOptions Size);
	void FixAsteroidScaling(AAsteroid* Asteroid);

	FRotator Rotation;
	FRotator RotationDirection;
	FVector FloatDirection;
	FVector Location;

	AActor* Owner = nullptr;
	USphereComponent* CollisionComponent;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
};
