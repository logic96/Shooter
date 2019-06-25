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
	UFUNCTION(BlueprintCallable)
	EWeaponTypes GetCurrentWeaponType();
	//UFUNCTION(BlueprintCallable)
	//ABaseWeapon* GetCurrentWeapon();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual FVector GetPawnViewLocation()const;//��ȡ�������

public:
	UPROPERTY(EditDefaultsOnly, Category = Player)
	class ABaseWeapon* CurrentWeapon;
protected:
	UPROPERTY(EditDefaultsOnly, Category = Player)
		TSubclassOf<ABaseWeapon>StarterWeaponClass;
	UPROPERTY(EditDefaultsOnly, Category = Player)
		FName WeaponAttachSocketName;
	void OnFire();
	void StopFire();
	//�ڴ����л�ȡAimOffset
	UFUNCTION(BlueprintCallable)
	FRotator GetAimOffset()const;
	//�������
	EWeaponTypes CurrentWeaponType;
	bool bWantsToZoom;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = player)
	float ZoomedFOV;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = player, meta = (ClampMin = 0.0, ClampMax = 100))
	float ZoomInterSpeed;
	float DefaultFOV;
	void BeginZoom();
	void EndZoom();
};
