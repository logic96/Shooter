// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Public/BaseItem.h"
#include "BaseWeapon.generated.h"

UENUM(BlueprintType)
enum class  EWeaponTypes :uint8 {
	Weapon_ShotGun,
	Weapon_Rifle,
	Weapon_HandGun,
	Weapon_Launcher,
	Weapon_NoGun,
};
UENUM(BlueprintType)
enum class EFireMode :uint8 {
	SingleShot,
	Burst,
	FullAuto,
};

UCLASS()
class SHOOTERGAME_API ABaseWeapon : public ABaseItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	class USkeletalMeshComponent* MeshComp;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Muzzle")
	FName MuzzleSocketName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Muzzle")
	UParticleSystem* MuzzleFlash;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectile)
	TSubclassOf<AActor>ProjectileClass;
	//这里是因为C++中不能直接使用蓝图类
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponType")
	EWeaponTypes GunType;
	UFUNCTION(BlueprintCallable)
	void Fire();
	//射击相关的
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Trigger")
	bool bIsTriggerPulled=false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FireMode")
	EFireMode CurrentFireMode;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FireMode")
	bool bCanShot = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FireMode")
	bool bIsInFireTimeDelay = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FireMode")
	float FireTimeDelay;
	//定时器，用于设置发射子弹相关的内容
	FTimerHandle TimerHandle;
	bool bUseTimeHandle = false;
	//连发相关
	bool bInBrustRound = false;//表明是否连发完成（如果进行的话）
	float BrustCount = 0;
	UFUNCTION(BlueprintCallable)
		void BrustFire();
	UFUNCTION(BlueprintCallable)
		void PullTrigger();
	UFUNCTION(BlueprintCallable)
		void ReleaseTrigger();
	UFUNCTION(BlueprintCallable)
	bool CanShot();
	UFUNCTION(BlueprintCallable)
		void ShotInSingle();
	UFUNCTION(BlueprintCallable)
		void ShotInBrust();
	UFUNCTION(BlueprintCallable)
		void ShotInAuto();
	//PickUp相关
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PickUp)
	//	bool bCanbePickUp = false;

	//子弹计数相关
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Ammo)
		FName AmmoName;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Ammo)
		int32 BulletCapacity=30;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Ammo)
		int32 BulletNums=0;//每次开火时检测子弹数量,数量<=0 无法开火
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Ammo)
		bool bHasBullet = false;//每次开火时检测子弹数量,数量<=0 无法开火
	bool CheckBullet();
	void Reload();

};
