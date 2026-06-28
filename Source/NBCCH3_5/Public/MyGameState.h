// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MyGameState.generated.h"

/**
 * 
 */
UCLASS()
class NBCCH3_5_API AMyGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	AMyGameState();
	
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Score")
	int32 Score; //점수
		
	// 현재 레벨에서 스폰된 코인 개수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;
	// 플레이어가 수집한 코인 개수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;
	// 첫레벨 첫웨이브에 주어지는 시간
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	float FirstLevelFirstWaveDuration;
	// 현재 진행 중인 레벨 인덱스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;
	// 전체 레벨의 개수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevels;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentWaveIndex = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 MaxWaveCount = 3;
	
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AActor>> CoinClass;
	UPROPERTY()
	TArray<AActor*> SpawnedItems; //스폰된 아이템 담 웨이브 전에 삭제하기 위한 배열
	
	bool bGameStarted = false;
	
	// 실제 레벨 맵 이름 배열. 여기 있는 인덱스를 차례대로 연동
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;
	
	// 매 레벨이 끝나기 전까지 시간이 흐르도록 관리하는 타이머
	FTimerHandle LevelTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;
	
	UFUNCTION(BlueprintPure, Category="Score")
	int32 GetScore() const; //현재 점수
	UFUNCTION(BlueprintCallable, Category="Score")
	void AddScore(int32 Amount); //점수 추가
	
	// 게임이 완전히 끝났을 때 (모든 레벨 종료) 실행되는 함수
	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();
	
	void StartWave();
	// 레벨을 시작할 때, 아이템 스폰 및 타이머 설정
	void StartLevel();
	// 레벨 제한 시간이 만료되었을 때 호출
	void OnWaveTimeUp();
	// 코인을 주웠을 때 호출
	void OnCoinCollected();
	void EndWave();
	// 레벨을 강제 종료하고 다음 레벨로 이동
	void EndLevel();
	
	void UpdateHUD();
};
