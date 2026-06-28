// Fill out your copyright notice in the Description page of Project Settings.


#include "HealItem.h"

#include "MyCharacter.h"

AHealItem::AHealItem()
{
	HealAmount = 20.0f;
	ItemType = "Heal";
}

void AHealItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
	if (Activator && Activator->ActorHasTag("Player"))
	{
		// 회복 디버그 메시지
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Player gained %d HP!"), HealAmount));
		
		if (AMyCharacter* PlayerCharacter = Cast<AMyCharacter>(Activator)) //Player클래스에 캐스트하여 직접 함수 호출
		{
			PlayerCharacter->AddHealth(HealAmount);
		}
	}
	DestroyItem();
}
