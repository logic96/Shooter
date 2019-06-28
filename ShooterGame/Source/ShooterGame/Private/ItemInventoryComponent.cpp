// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemInventoryComponent.h"
#include "Public/BaseItem.h"
// Sets default values for this component's properties
UItemInventoryComponent::UItemInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UItemInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UItemInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

int32 UItemInventoryComponent::AddToInventory(ABaseItem* ItemToAdd)
{
	
	for (int i = 0; i < ItemList.Num(); i++)
	{
		if (ItemToAdd->ItemName == ItemList[i]->ItemName)
		{
			ItemList[i]->ItemNums += ItemToAdd->ItemNums;
			return true;
		}
	}
	return ItemList.Add(ItemToAdd);
}

void UItemInventoryComponent::RemoveFromInventory(ABaseItem* ItemToRemove)
{
	ItemList.Remove(ItemToRemove);
}

