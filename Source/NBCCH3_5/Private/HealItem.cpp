// Fill out your copyright notice in the Description page of Project Settings.


#include "HealItem.h"

AHealItem::AHealItem()
{
	HealAmount = 20.0f;
	ItemType = "Heal";
}

void AHealItem::ActivateItem(AActor* Activator)
{
	DestroyItem();
}
