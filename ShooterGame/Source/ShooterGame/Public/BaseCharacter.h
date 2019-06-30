// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Public/BaseWeapon.h"
#include "BaseCharacter.generated.h"


UCLASS()
class SHOOTERGAME_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//Movement
	void MoveRight(float Value);
	void MoveForward(float Value);
	void BeginCrouch();
	void EndCrouch();
	void BeginSprint();
	void EndSprint();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component)
	class	UCameraComponent* CameraComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component)
	class	USpringArmComponent* SpringArmComp;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "status")
	bool IsSprinting = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "status")
	bool IsFiring = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "status")
	bool IsPlayingAnims = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "status")
	bool IsDead=false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "status")
	int DeathAnimNum = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "status")
	int Health = 200.f;


	UFUNCTION(BlueprintCallable)
	inline	bool GetSprinting() { return IsSprinting; }
	UFUNCTION(BlueprintCallable)
	inline	bool GetFireing() { return IsFiring; }
	UFUNCTION(BluePrintCallable)
	inline int GetHealth() { return Health; }
	UFUNCTION(BluePrintCallable)
	inline bool GetDeath() { return IsDead; }	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual FVector GetPawnViewLocation()const;//获取相机中心

public:
	//规则说明，枪支数量固定，已满，将手头的枪和地上的枪进行切换<最好是按键交互拾取CCD开>
	//ItemInventory相关，需要在游戏中也能访问与修改
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Player)
		class UItemInventoryComponent* MyItemInventory;
	UFUNCTION(BlueprintCallable)
		void PickUpItem(class ABaseItem* ItemToPickUp);

	//WeaponInventory相关,在游戏中也能够访问与修改
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Player)
		class UWeaponInventoryComponent* MyWeaponInventory;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
		ABaseWeapon* CurrentWeapon;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor)override;
	void DropWeapon();
	void PickUpWeapon(class ABaseWeapon* WeaponToPickUp);
	int32 FindWeapon(class ABaseWeapon* WeaponToFind);
	void StartFire();
	void StopFire();
	void NextWeapon();
	void PrevWeapon();
	void StartTargeting();
	void StopTargeting();
	void ReloadAmmo();
	UFUNCTION(BlueprintCallable)
		EWeaponTypes GetCurrentWeaponType();
	UFUNCTION(BlueprintCallable)
		ABaseWeapon* GetCurrentWeapon()const;
	//武器信息
	EWeaponTypes CurrentWeaponType;
	bool bWantsToZoom;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = player)
		float ZoomedFOV;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = player, meta = (ClampMin = 0.0, ClampMax = 100))
		float ZoomInterSpeed;
	float DefaultFOV;
protected:
	UPROPERTY(EditDefaultsOnly, Category = Player)
		TSubclassOf<ABaseWeapon>StarterWeaponClass;
	UPROPERTY(EditDefaultsOnly, Category = Player)
		FName WeaponAttachSocketName;
	//在代码中获取AimOffset
	UFUNCTION(BlueprintCallable)
	FRotator GetAimOffset()const;
};
