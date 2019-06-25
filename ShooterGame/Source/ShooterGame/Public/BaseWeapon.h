// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

UENUM(BlueprintType)
enum class  EWeaponTypes :uint8 {
	NoWeapon,
	Weapon_ShotGun,
	Weapon_Rifile,
};
UENUM(BlueprintType)
enum class EFireMode :uint8 {
	SingleShot,
	Burst,
	FullAuto,
};

UCLASS()
class SHOOTERGAME_API ABaseWeapon : public AActor
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
	//��������ΪC++�в���ֱ��ʹ����ͼ��
public:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponType")
	EWeaponTypes GunType;
	UFUNCTION(BlueprintCallable)
	void Fire();
	//�����ص�
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Trigger")
	bool bIsTriggerPulled=false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FireMode")
	EFireMode CurrentFireMode;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FireMode")
	bool bCanShot = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FireMode")
	bool bIsInFireTimeDelay = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FireMode")
	//��ʱ�����������÷����ӵ���ص�����
	float FireTimeDelay;
	FTimerHandle TimerHandle;
	bool bUseTimeHandle = false;
	//�������
	bool bInBrustRound = false;//�����Ƿ�������ɣ�������еĻ���
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
};
