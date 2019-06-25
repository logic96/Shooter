// Fill out your copyright notice in the Description page of Project Settings.


#include "public/BaseWeapon.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystem.h"
// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MuzzleSocketName = "Muzzle";
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseWeapon::Fire()
{
	AActor* MyOwner = GetOwner();
	if (MyOwner) {
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);//��Ҫ��д�÷�����ʹ�÷����������

//		EyeLocation=<>MyOwner->GetPawnViewLocation
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);//�ӵ��ķ����
		FVector TraceEnd = EyeLocation + (EyeRotation.Vector()) * 10000;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		FHitResult Hit;
		FVector HitLocation;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))//Ϊǹ���忼��
		{
			//Do damage
		    HitLocation= Hit.ImpactPoint;
		//	DrawDebugLine(GetWorld(), MuzzleLocation, HitLocation, FColor::Green, false,2.0f, 0, 1.0f);//���ӵ��ķ���㻭һ���������ڸ���
		//	DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 2.0f, 0, 1.0f);		//�������׼�������壬��ô����ĽǶȾ������������		   
		}
		else {
			HitLocation = TraceEnd;
		//	DrawDebugLine(GetWorld(), MuzzleLocation, TraceEnd, FColor::Red, false, 1.0f, 0, 1.0f);
		}
		FRotator ShotDirection = (HitLocation - MuzzleLocation).Rotation();
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//��ǹ�ڷ����ӵ�
		GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, ShotDirection, SpawnParams);
	}
}



