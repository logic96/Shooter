// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponInventoryComponent.h"
#include "public/BaseWeapon.h"
// Sets default values for this component's properties
UWeaponInventoryComponent::UWeaponInventoryComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called every frame
void UWeaponInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// Called when the game starts
void UWeaponInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

int32 UWeaponInventoryComponent::AddToInventory(ABaseWeapon* WeaponToAdd)
{
	return WeaponList.Add(WeaponToAdd);

}

void UWeaponInventoryComponent::RemoveFromInventory(ABaseWeapon* WeaponToRemove)
{
	WeaponList.Remove(WeaponToRemove);
}


