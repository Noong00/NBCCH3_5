// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Step.generated.h"

UCLASS()
class NBCCH3_5_API AStep : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStep();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="AMyActor2|Components")
	TObjectPtr<USceneComponent> SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="AMyActor2|Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	
	//멤버변수
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AMyActor2|Properties")
	float LocationSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AMyActor2|Properties")
	FVector LocationDirection;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AMyActor2|Properties")
	bool IsMaxRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AMyActor2|Properties")
	float MoveRange;
	
	FVector StartLocation;
};
