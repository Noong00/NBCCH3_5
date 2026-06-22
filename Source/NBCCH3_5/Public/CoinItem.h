// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "CoinItem.generated.h"

/**
 * 
 */
UCLASS()
class NBCCH3_5_API ACoinItem : public ABaseItem //코인아이템들의 부모 아이템
{
	GENERATED_BODY()
public:
	ACoinItem();

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 PointValue;
};
