// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "MineItem.generated.h"

/**
 * 
 */
UCLASS()
class NBCCH3_5_API AMineItem : public ABaseItem
{
	GENERATED_BODY()
	
	public:
	AMineItem();
	
	protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	USphereComponent* ExplosionCollision;
	
	// 폭발 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mine")
	float ExplosionDelay;
	// 폭발 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mine")
	float ExplosionRadius;
	// 폭발 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mine")
	float ExplosionDamage;

	FTimerHandle ExplosionTimerHandle;
	void Explode();
	
	virtual void ActivateItem(AActor* Activator) override;
};
