// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponInventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTERGAME_API UWeaponInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=Weapons)
	TArray<class ABaseWeapon*>WeaponList;
	UFUNCTION(BlueprintCallable)
		int32 AddToInventory(ABaseWeapon* WeaponToAdd);
	UFUNCTION(BlueprintCallable)
		void RemoveFromInventory(ABaseWeapon* WeaponToRemove);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
