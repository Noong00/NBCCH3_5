// Fill out your copyright notice in the Description page of Project Settings.


#include "CoinItem.h"
#include "Engine/World.h"
#include "MyGameState.h"

ACoinItem::ACoinItem()
{
	PointValue = 0;
	ItemType = "DefaultCoin";
}

void ACoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
	if (Activator && Activator->ActorHasTag("Player")) //플레이어인지 태그로 확인
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Player gained %d points!"), PointValue));
		if (UWorld* World = GetWorld())
		{
			if (AMyGameState* GameState = World->GetGameState<AMyGameState>())
			{
				GameState->AddScore(PointValue);
				GameState->OnCoinCollected(); //코인 먹었다고 호출
			}
		}
		
		DestroyItem();// 부모 클래스 (BaseItem)에 정의된 함수
	}
}