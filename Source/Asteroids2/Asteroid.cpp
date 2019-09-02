// Fill out your copyright notice in the Description page of Project Settings.

#include "Asteroid.h"
#include "UnrealType.h"
#include "ShipComponent.h"

// Sets default values
AAsteroid::AAsteroid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Asteroid"));
	CollisionComponent->InitSphereRadius(25.0f);
	CollisionComponent->OnComponentHit.AddDynamic(this, &AAsteroid::OnHit);
	RootComponent = CollisionComponent;

	UStaticMeshComponent* MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	MyMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderAsset(TEXT("/Game/Asteroid/Asteroid_M"));
	
	if (CylinderAsset.Succeeded())
	{
		MyMesh->SetStaticMesh(CylinderAsset.Object);
		MyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		MyMesh->SetWorldScale3D(FVector(1.f));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Mesh for Asteroid actor missing!"))
	}
}

void AAsteroid::Accelerate()
{
	FloatVelocity += AccelerateBy;
}

void AAsteroid::Rotate()
{
	Rotation = Rotation + (RotationDirection * RotationSpeed);
	Owner->SetActorRotation(Rotation);
}

void AAsteroid::SetFloatDirection()
{
	FloatDirection = Owner->GetActorForwardVector();
}

void AAsteroid::SetRandomRotationDirection()
{
	RotationDirection = FRotator(
		FMath::RandRange(0.1f, 0.5f),
		FMath::RandRange(0.1f, 0.5f),
		0.0f
	);
}

void AAsteroid::SetRandomRotation()
{
	Rotation = FRotator(
		FMath::FMath::RandRange(0.1f, 360.0f),
		FMath::FMath::RandRange(0.1f, 360.0f),
		FMath::FMath::RandRange(0.1f, 360.0f)
	);
}


// Float the asteroid each frame.
void AAsteroid::Float()
{
	bool Collide = true;

	Location = Location + (FloatDirection * FloatVelocity);

	// Teleport object to opposite side in case it's out of screen range. (less elegant)
	if (Location.X < ScreenXMin)
	{
		Location.X = ScreenXMax;
		Collide = false;
	}
	else if (Location.X > ScreenXMax)
	{
		Location.X = ScreenXMin;
		Collide = false;
	}

	if (Location.Y < ScreenYMin)
	{
		Location.Y = ScreenYMax;
		Collide = false;
	}
	else if (Location.Y > ScreenYMax)
	{
		Location.Y = ScreenYMin;
		Collide = false;
	}

	Location.Z = 200;

	Owner->SetActorLocation(Location, Collide);
}

// Called when the game starts or when spawned
void AAsteroid::BeginPlay()
{
	Super::BeginPlay();

	Owner = this;
	Location = Owner->GetActorLocation();
	SpawnTime = GetWorld()->GetTimeSeconds();

	SetRandomRotation();
	SetRandomRotationDirection();
	SetFloatDirection();
	FixAsteroidScaling(this);
}

// Called every frame
void AAsteroid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Float();
	Rotate();

	// Set random Float Directions for a tiny bit.
	if (SpawnTime + 0.5 > GetWorld()->GetTimeSeconds())
	{
		SetFloatDirection();
	}
}

void AAsteroid::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	FString OtherActorClassName = *OtherActor->GetClass()->GetName();

	if(OtherActor != NULL && ( OtherActorClassName == "Laser_C" || OtherActorClassName == "Ship_BP_C"))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s Asteroid destroyed at %f, %f, %f"), *this->GetName(), Location.X, Location.Y, Location.Z);

		switch (Size)
		{
			case Large:

				for (int i = 0; i < AsteroidChildrenCount; i++)
				{
					SpawnChildAsteroid(Medium);
				}

			break;

			case Medium:

				for (int i = 0; i < AsteroidChildrenCount; i++)
				{
					SpawnChildAsteroid(Small);
				}

			break;
		}

		Destroy();
	}
}

void AAsteroid::SpawnChildAsteroid(SizeOptions Size)
{
	UWorld* World = GetWorld();

	if (World)
	{
		AAsteroid* Child = World->SpawnActor<AAsteroid>(this->GetClass(), Location, Rotation, FActorSpawnParameters());

		Child->Size = Size;
		Child->FloatVelocity += FloatVelocityIncrement;

		FixAsteroidScaling(Child);
		FVector ChildActorLocation = Child->GetActorLocation();

		UE_LOG(LogTemp, Warning, TEXT("%s Asteroid spawned at %f, %f, %f"), *Child->GetName(), ChildActorLocation.X, ChildActorLocation.Y, ChildActorLocation.Z)
	}
}

void AAsteroid::FixAsteroidScaling(AAsteroid* Asteroid)
{
	switch(Asteroid->Size)
	{
		case Large:
			Asteroid->SetActorScale3D(FVector(LargeScale, LargeScale, LargeScale));
		break;

		case Medium:
			Asteroid->SetActorScale3D(FVector(MediumScale, MediumScale, MediumScale));
		break;

		case Small:
			Asteroid->SetActorScale3D(FVector(SmallScale, SmallScale, SmallScale));
		break;
	}
}