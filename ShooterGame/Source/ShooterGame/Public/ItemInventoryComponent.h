// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemInventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTERGAME_API UItemInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UItemInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Weapons)
		TArray<class ABaseItem*>ItemList;
	UFUNCTION(BlueprintCallable)
		int32 AddToInventory(ABaseItem* ItemToAdd);
	UFUNCTION(BlueprintCallable)
		void RemoveFromInventory(ABaseItem* ItemToRemove);
		
};
