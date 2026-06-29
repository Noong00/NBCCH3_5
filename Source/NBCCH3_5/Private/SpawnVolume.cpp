// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);
	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);

	ItemDataTable = nullptr;
}

AActor* ASpawnVolume::SpawnRandomItem()
{
	if (FItemSpawnRow* SelectedRow = GetRandomItem())
	{
		if (UClass* ActualClass = SelectedRow->ItemClass.Get())
		{
			return SpawnItem(ActualClass);
		}
	}
	return nullptr;
}

FVector ASpawnVolume::GetRandomPointInVolume() const
{
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent(); //x,y,z 각 방향으로 절반 길이
	FVector BoxOrigin = SpawningBox->GetComponentLocation(); //스폰 볼륨의 중앙


	return BoxOrigin + FVector( //x,y,z 각 방향 -extent ~ +extent
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
	);
}

FItemSpawnRow* ASpawnVolume::GetRandomItem() const
{
	if (!ItemDataTable) return nullptr; //ItemDataTable을 못찾으면 그냥 리턴 (방어코드)

	TArray<FItemSpawnRow*> AllRows;
	static const FString ContextString(TEXT("ItemSpawnContext"));
	ItemDataTable->GetAllRows(ContextString, AllRows); // 모든 행 가져오기

	if (AllRows.IsEmpty()) return nullptr; //ItemDataTable이 비었으면 리턴

	float TotalChance = 0.0f;
	for (const FItemSpawnRow* Row : AllRows) // AllRows 배열의 각 Row를 순회
	{
		if (Row) // Row가 유효한지 확인
		{
			TotalChance += Row->SpawnChance; // SpawnChance 값을 TotalChance에 더해 전체 확률 산출
		}
	}

	const float RandValue = FMath::FRandRange(0.0f, TotalChance); //0부터 전체 확률 사이 랜덤 값
	float AccumulateChance = 0.0f;

	for (FItemSpawnRow* Row : AllRows) // AllRows 배열의 각 Row를 순회
	{
		AccumulateChance += Row->SpawnChance; //누적확률을 저장
		if (RandValue <= AccumulateChance) // 랜덤값보다 커지면 해당 row 리턴
		{
			return Row;
		}
	}

	return nullptr;
}

AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass) return nullptr;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		ItemClass,
		GetRandomPointInVolume(),
		FRotator::ZeroRotator
	);

	return SpawnedActor;
}
