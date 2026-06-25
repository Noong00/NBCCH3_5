// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameState.h"

#include "CoinItem.h"
#include "MyGameInstance.h"
#include "MyPlayerController.h"
#include "SpawnVolume.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

AMyGameState::AMyGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 60.0f; // 한 레벨당 30초
	CurrentLevelIndex = 0;
	MaxLevels = 3;
}

void AMyGameState::BeginPlay()
{
	Super::BeginPlay();
	
	UMyGameInstance* MyGI = Cast<UMyGameInstance>(GetGameInstance());
	if (MyGI)
	{
		CurrentLevelIndex = MyGI->CurrentLevelIndex; //게임 인스턴스에서 가져오기
	}
	
	StartLevel();//시작시 첫 레벨부터
	GetWorldTimerManager().SetTimer(HUDUpdateTimerHandle, this,&AMyGameState::UpdateHUD,0.1f,true);
	UpdateHUD();
}

int32 AMyGameState::GetScore() const
{
	return Score;
}

void AMyGameState::AddScore(int32 Amount)
{
	Score += Amount;
	// GameInstance에도 반영
	UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GetGameInstance());
	if (MyGameInstance)
	{
		MyGameInstance->TotalScore += Amount;
	}
}

void AMyGameState::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* SpartaPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD();
		}
	}
	
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UMyGameInstance* SpartaGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		}
	}
	
	//시작 시 카운트 초기화
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes); // 레벨에 배치된  SpawnVolume을 모두 탐색
	const int32 ItemToSpawn = 40; //아이템을 40개만 스폰하기 위한 변수

	if (FoundVolumes.Num() > 0)
	{
		ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
		if (SpawnVolume)
		{
			for (int32 i = 0; i < ItemToSpawn; i++)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}

	GetWorldTimerManager().SetTimer(
		LevelTimerHandle,
		this,
		&AMyGameState::OnLevelTimeUp,
		LevelDuration, //LevelDuration만큼 시간이 흐른 후 OnLevelTimeUp함수 호출 
		false
	);

	//UpdateHUD();
	
	UE_LOG(LogTemp, Warning, TEXT("Level %d Start!, Spawned %d coin"),CurrentLevelIndex + 1,SpawnedCoinCount);
}

void AMyGameState::OnLevelTimeUp()
{
	EndLevel();// 시간이 다 되면 레벨을 종료
}

void AMyGameState::OnCoinCollected()
{
	CollectedCoinCount++;

	UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"),CollectedCoinCount,SpawnedCoinCount)
	
	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount) // 코인 다주으면 레벨 바로 종료
	{
		EndLevel();
	}
}

void AMyGameState::EndLevel()
{
	
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);// 타이머 해제
	
	CurrentLevelIndex++; //레벨 올림

	UMyGameInstance* MyGI = Cast<UMyGameInstance>(GetGameInstance());
	if (MyGI)
	{
		MyGI->CurrentLevelIndex = CurrentLevelIndex; // GameInstance에 저장
	}
	
	if (CurrentLevelIndex >= MaxLevels) //맥스 레벨이면 게임오버
	{
		OnGameOver();
		return;
	}

	// 레벨 맵 이름이 있다면 해당 맵 불러오기
	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		// 맵 이름이 없으면 게임오버
		OnGameOver();
	}
}

void AMyGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* SpartaPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true);
			SpartaPlayerController->ShowMainMenu(true);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Game Over!!"));
}

void AMyGameState::UpdateHUD()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController) return;

	AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController);
	if (!MyPlayerController) return;

	UUserWidget* HUDWidget = MyPlayerController->GetHUDWidget();
	if (!HUDWidget) return;

	if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
	{
		float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
		if (RemainingTime < 0.f) RemainingTime = 0.f;
		TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
	}

	if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
	{
		UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GetGameInstance());
		if (MyGameInstance)
		{
			ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), MyGameInstance->TotalScore)));
		}
	}

	if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
	{
		LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));
	}
}
// void AMyGameStateBase::UpdateHUD()
// {
// 	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
// 	if (!PlayerController) { UE_LOG(LogTemp, Error, TEXT("PlayerController NULL")); return; }
//
// 	AMyPlayerController* MyPC = Cast<AMyPlayerController>(PlayerController);
// 	if (!MyPC) { UE_LOG(LogTemp, Error, TEXT("Cast to MyPlayerController FAILED")); return; }
//
// 	UUserWidget* HUDWidget = MyPC->GetHUDWidget();
// 	if (!HUDWidget) { UE_LOG(LogTemp, Error, TEXT("HUDWidget NULL")); return; }
//
// 	UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time")));
// 	if (!TimeText) { UE_LOG(LogTemp, Error, TEXT("Time TextBlock NOT FOUND")); }
// 	else { 
// 		float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
// 		if (RemainingTime < 0.f) RemainingTime = 0.f;
// 		TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
// 	}
//
// 	UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score")));
// 	if (!ScoreText) { UE_LOG(LogTemp, Error, TEXT("Score TextBlock NOT FOUND")); }
// 	else {
// 		UMyGameInstance* MyGI = Cast<UMyGameInstance>(GetGameInstance());
// 		if (!MyGI) { UE_LOG(LogTemp, Error, TEXT("GameInstance NULL")); }
// 		else { ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), MyGI->TotalScore))); }
// 	}
//
// 	UTextBlock* LevelText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level")));
// 	if (!LevelText) { UE_LOG(LogTemp, Error, TEXT("Level TextBlock NOT FOUND")); }
// 	else { LevelText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1))); }
// }