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
	FirstLevelFirstWaveDuration = 120.0f; 
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
	
	//시작시 첫 레벨부터
	//GetWorldTimerManager().SetTimer(HUDUpdateTimerHandle, this,&AMyGameState::UpdateHUD,0.1f,true);
	//UpdateHUD();
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
	if (bGameStarted) return;

	bGameStarted = true;
	GetWorldTimerManager().SetTimer(HUDUpdateTimerHandle, this,&AMyGameState::UpdateHUD,0.1f,true);
	UpdateHUD();
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (MyGameInstance)
		{
			CurrentLevelIndex = MyGameInstance->CurrentLevelIndex;
		}
	}
	
	CurrentWaveIndex = 0;
	
	StartWave();
}

void AMyGameState::StartWave()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			MyPlayerController->ShowGameHUD();
		}
	}
	
	// if (UGameInstance* GameInstance = GetGameInstance())
	// {
	// 	UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
	// 	if (MyGameInstance)
	// 	{
	// 		CurrentWaveIndex = MyGameInstance->CurrentWaveIndex;
	// 	}
	// }
	
	//기존 스폰된 아이템 삭제
	for (int32 i = 0; i < SpawnedItems.Num(); i++)
	{
		AActor* Item = SpawnedItems[i];
		if (IsValid(Item)) //GC의 pending Kill 체크
		{
			Item->Destroy();
		}
	}

	SpawnedItems.Empty();
	//시작 시 카운트 초기화
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes); // 레벨에 배치된  SpawnVolume을 모두 탐색
	
	int32 FixedCoinCount = 10 + ((CurrentLevelIndex * 3) + CurrentWaveIndex) * 5;
	int32 RandomItemCount = 10 + ((CurrentLevelIndex * 3) + CurrentWaveIndex) * 10;;
	
	if (FoundVolumes.Num() > 0)
             	{
             		ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
             		if (SpawnVolume)
             		{
             			for (int32 i = 0; i < FixedCoinCount; i++)
             			{
             				if (CoinClass.Num() > 0)
             				{
             					int32 Index = FMath::RandRange(0, CoinClass.Num() - 1);
             					UClass* SelectedClass = CoinClass[Index];
             
             					if (SelectedClass)
             					{
             						AActor* SpawnedCoin = SpawnVolume->SpawnItem(SelectedClass);
             						if (SpawnedCoin)
             						{
             							SpawnedCoinCount++;
             							SpawnedItems.Add(SpawnedCoin);
             						}
             					}
             				}
             			}
			for (int32 i = 0; i < RandomItemCount; i++)
			{
				AActor* SpawnedRandom = SpawnVolume->SpawnRandomItem();
				if (SpawnedRandom)
				{
					SpawnedItems.Add(SpawnedRandom);
				}
			}
		}
	}
	
	if (CurrentWaveIndex != 0)
	{
		FirstLevelFirstWaveDuration -= 10;
	}
	
	if (CurrentLevelIndex >= 1)
	{
		FirstLevelFirstWaveDuration -= 10;
	}
	
	GetWorldTimerManager().SetTimer(
		LevelTimerHandle,
		this,
		&AMyGameState::OnWaveTimeUp,
		FirstLevelFirstWaveDuration, //LevelDuration만큼 시간이 흐른 후 OnLevelTimeUp함수 호출 
		false
	);

	//UpdateHUD();
	GEngine->AddOnScreenDebugMessage(
		-1,
		10.0f,
		FColor::Cyan,
		FString::Printf(TEXT("Level %d  / Wave %d Start!, Spawned %d coin"),CurrentLevelIndex+1, CurrentWaveIndex+1, SpawnedCoinCount));
	UE_LOG(LogTemp, Warning, TEXT("Wave %d Start!, Spawned %d coin"),CurrentLevelIndex + 1,SpawnedCoinCount);
}

void AMyGameState::OnWaveTimeUp()
{
	OnGameOver();// 시간이 다 되면 게임오버
	//EndWave();
}

void AMyGameState::OnCoinCollected()
{
	CollectedCoinCount++;
	
	GEngine->AddOnScreenDebugMessage(
			-1,
			10.0f,
			FColor::Blue,
			FString::Printf(TEXT("Spawned coin / Collected coin : %d / %d"), SpawnedCoinCount, CollectedCoinCount));
	UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"),CollectedCoinCount,SpawnedCoinCount)
	
	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount) // 코인 다주으면 웨이브 바로 종료
	{
		EndWave();
	}
}

void AMyGameState::EndWave()
{
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);// 타이머 해제
	CurrentWaveIndex++;
	
	UMyGameInstance* MyGI = Cast<UMyGameInstance>(GetGameInstance());
	if (MyGI)
	{
		MyGI->CurrentWaveIndex = CurrentWaveIndex; // GameInstance에 저장
	}
	
	if (CurrentWaveIndex >= MaxWaveCount)//맥스 웨이브면 다음 레벨
	{
		EndLevel(); // 레벨 종료
		return;
	}
	
	StartWave();
}

void AMyGameState::EndLevel()
{
	CurrentWaveIndex = 0;
	CurrentLevelIndex++; //레벨 올림

	UMyGameInstance* MyGI = Cast<UMyGameInstance>(GetGameInstance());
	if (MyGI)
	{
		MyGI->CurrentLevelIndex = CurrentLevelIndex; // GameInstance에 저장
		MyGI->CurrentWaveIndex = CurrentWaveIndex;
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
		int32 RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
		//if (RemainingTime < 0.f) RemainingTime = 0.f;
		TimeText->SetText(FText::FromString(FString::Printf(TEXT("%d"), RemainingTime)));
	}

	if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
	{
		UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GetGameInstance());
		if (MyGameInstance)
		{
			ScoreText->SetText(FText::FromString(FString::Printf(TEXT("%d"), MyGameInstance->TotalScore)));
		}
	}
	if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
	{
		LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("%d"), CurrentLevelIndex + 1)));
	}
	if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
	{
		LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("%d/3"), CurrentWaveIndex + 1)));
	}
	if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("TargetCoin"))))
	{
		LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("%d Coin"), SpawnedCoinCount)));
	}
	if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("CollectedCoin"))))
	{
		LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("%d Coin"), CollectedCoinCount)));
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