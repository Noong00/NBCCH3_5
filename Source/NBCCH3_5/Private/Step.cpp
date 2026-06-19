// Fill out your copyright notice in the Description page of Project Settings.


#include "Step.h"

// Sets default values
AStep::AStep()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(SceneRoot);
	
	LocationSpeed = 0.0f; //floAT로 바꿔도됨
	LocationDirection = FVector(0.0f, 0.0f, 0.0f);
	IsMaxRange = true;
	MoveRange = 0.0f;
}

// Called when the game starts or when spawned
void AStep::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = GetActorLocation();
}

// Called every frame
void AStep::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector NormalizedFrameMovement = LocationDirection.GetSafeNormal()*LocationSpeed*DeltaTime; //프렘당 움직이는 거리
	FVector TargetDirection = LocationDirection.GetSafeNormal()*MoveRange; //보간해줄 최종
	
	//왔다갔다 이동하기
	if (IsMaxRange)
	{
		
		SetActorLocation(GetActorLocation()+NormalizedFrameMovement);
		
		if (FVector::Dist(StartLocation, GetActorLocation()) >= MoveRange)
		{
			SetActorLocation(StartLocation+TargetDirection);
			
			IsMaxRange = false;
		}
	}
	else
	{
		
		SetActorLocation(GetActorLocation()-NormalizedFrameMovement);
		
		if (FVector::Dist(StartLocation, GetActorLocation()) >= MoveRange)
		{
			SetActorLocation(StartLocation-TargetDirection);
			
			IsMaxRange = true;
		}
	}
}

