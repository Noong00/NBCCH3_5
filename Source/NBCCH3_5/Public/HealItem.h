// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "HealItem.generated.h"

/**
 * 
 */
UCLASS()
class NBCCH3_5_API AHealItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	AHealItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 HealAmount;

	virtual void ActivateItem(AActor* Activator) override;
};
