// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

UENUM(BlueprintType)
enum class EItemTypes :uint8 {
	Item_Weapon,
	Item_Medicine,
	Item_Projectile,
};
UCLASS()
class SHOOTERGAME_API ABaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseItem();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ItemInfo)
		EItemTypes ItemType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ItemInfo)
		FName ItemName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ItemInfo)
		int32 ItemNums=0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ItemInfo)
		float ItemBuff;
	//子类的BeginPlay或者是用蓝图设置属性
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PickUp)
		bool bCanbePickUp = false;
	UFUNCTION(BlueprintCallable)
	virtual	void PickUp();
	UFUNCTION(BlueprintCallable)
	virtual	void Drop(FTransform TargetLocation);


};
