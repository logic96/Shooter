// Fill out your copyright notice in the Description page of Project Settings.


#include "public/BaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "Public/BaseWeapon.h"
#include "kismet/GameplayStatics.h"
#include "Public/WeaponInventoryComponent.h"
#include "Engine/World.h"
#include "Public/BaseItem.h"
#include "Public/ItemInventoryComponent.h"
// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Components"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = false;


	WeaponAttachSocketName = "SKT_Hand_R";
	ZoomedFOV = 60.f;
	ZoomInterSpeed = 20.f;
	//创建一个武器库库存列表
	MyWeaponInventory = CreateDefaultSubobject<UWeaponInventoryComponent>("MyWeaponInventory");
	//创建一个物品库
	MyItemInventory = CreateDefaultSubobject<UItemInventoryComponent>("MyItemInventory");
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = 330.f;
	//spawns a default weapon
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	InitialWeapon = GetWorld()->SpawnActor<ABaseWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (InitialWeapon)
	{	
		WeaponInUse = InitialWeapon;
		WeaponInUse->SetActorEnableCollision(false);
		WeaponInUse->SetOwner(this);
		WeaponInUse->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		//暂时采用这种方式使得当前枪支不和人触发overlap拾取事件
		//将当前Weapon加入MyWeaponInventory
		MyWeaponInventory->AddToInventory(WeaponInUse);
		GetCurrentWeaponType();
	}
	DefaultFOV = CameraComp->FieldOfView;
}


EWeaponTypes ABaseCharacter::GetCurrentWeaponType()
{
	if (WeaponInUse) { return WeaponInUse->GunType; }
	else return EWeaponTypes::Weapon_ShotGun;
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (CurrentWeaponType == EWeaponTypes::Weapon_ShotGun)
	{
		float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;
		float CurrentFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterSpeed);
		CameraComp->SetFieldOfView(CurrentFOV);
	}
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ABaseCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ABaseCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABaseCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ABaseCharacter::EndCrouch);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ABaseCharacter::BeginSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ABaseCharacter::EndSprint);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABaseCharacter::Jump);	
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABaseCharacter::OnFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABaseCharacter::StopFire);
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ABaseCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ABaseCharacter::EndZoom);
	PlayerInputComponent->BindAction("DropWeapon", IE_Pressed, this, &ABaseCharacter::DropWeapon);
	PlayerInputComponent->BindAction("PickUp", IE_Pressed, this, &ABaseCharacter::EnablePickUp);
	PlayerInputComponent->BindAction("PickUp", IE_Released, this, &ABaseCharacter::DisablePickUp);
	PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, this, &ABaseCharacter::NextWeapon);
	PlayerInputComponent->BindAction("PrevWeapon", IE_Pressed, this, &ABaseCharacter::PrevWeapon);

}

void ABaseCharacter::OnFire() 
{
	if (WeaponInUse != nullptr)
	{

		IsFiring = true;
	//	CurrentWeapon->Fire();
		WeaponInUse->PullTrigger();
	}
}

void ABaseCharacter::StopFire()
{
	if (WeaponInUse != nullptr)
	{

		IsFiring = false;
		//	CurrentWeapon->Fire();
		WeaponInUse->ReleaseTrigger();
	}
}

FRotator ABaseCharacter::GetAimOffset() const
{

	//ControlRotation
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC) {
		FRotator AimRot = (PC->GetControlRotation() - GetActorRotation()).GetNormalized();
		
		return AimRot;
	}
	return (GetBaseAimRotation()-GetActorRotation()).GetNormalized();//如果不行的话，就用最原始的
}

void ABaseCharacter::BeginZoom()
{
	bWantsToZoom = true;
	CurrentWeaponType = GetCurrentWeaponType();
	if (CurrentWeaponType == EWeaponTypes::Weapon_ShotGun) {
		//DoNothing
	}
	else {
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC) {
			PC->SetViewTargetWithBlend(WeaponInUse, 0.1f, EViewTargetBlendFunction::VTBlend_Linear);
		}
	}
}

void ABaseCharacter::EndZoom()
{
	CurrentWeaponType = GetCurrentWeaponType();
	if (CurrentWeaponType == EWeaponTypes::Weapon_ShotGun) {
		//DoNothing
	}
	else {
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC) {
			PC->SetViewTargetWithBlend(this, 0.1f, EViewTargetBlendFunction::VTBlend_Linear);
		}
	}
	bWantsToZoom = false;
}

void ABaseCharacter::NextWeapon()
{
	if (MyWeaponInventory->WeaponList.Num() <= 1) {
		return;//列表只有一杠枪，退出
	}
	if (MyWeaponInventory->WeaponList.Num() - 1 == WeaponIndex) {
		WeaponIndex = 0;
	}//再回到开头 
	//播放动画，在Notify中进行切枪逻辑	
	WeaponInUse->SetActorHiddenInGame(true);//手头的枪隐藏

	WeaponIndex++;
	WeaponInUse = MyWeaponInventory->WeaponList[WeaponIndex];
	WeaponInUse->SetActorHiddenInGame(false);
	WeaponInUse->SetActorEnableCollision(false);
	WeaponInUse->SetOwner(this);
	WeaponInUse->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
	//暂时采用这种方式使得当前枪支不和人触发overlap拾取事件
	
}

void ABaseCharacter::PrevWeapon()
{
	if (MyWeaponInventory->WeaponList.Num() <= 1) {
		return;//列表只有一杠枪，退出
	}
	if (WeaponIndex == 0) {WeaponIndex = MyWeaponInventory->WeaponList.Num() - 1;
	}
//播放动画，在Notify中进行切枪逻辑	
		WeaponInUse->SetActorHiddenInGame(true);//手头的枪隐藏
//ToDo:解除Attach
		WeaponIndex--;
		WeaponInUse = MyWeaponInventory->WeaponList[WeaponIndex];
		WeaponInUse->SetActorHiddenInGame(false);
		WeaponInUse->SetActorEnableCollision(false);
		WeaponInUse->SetOwner(this);
		WeaponInUse->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
}

FVector ABaseCharacter::GetPawnViewLocation()const {
	//if (CameraComp) {
	//	return CameraComp->GetComponentLocation();
	//}
	//尝试用PlayerCameraManager来获取相机Location
	APlayerCameraManager* PCM = UGameplayStatics::GetPlayerCameraManager(this,0);
	if (PCM) {
		return PCM->GetCameraLocation();
	}

	return Super::GetPawnViewLocation();
}

void ABaseCharacter::DropWeapon()
{
	if (MyWeaponInventory->WeaponList.Num() == 0) { return; }
//Drop的话，修改丢弃当前的枪吧，或者选择枪支
	ABaseWeapon* Weapon = MyWeaponInventory->WeaponList.Last();
	MyWeaponInventory->RemoveFromInventory(Weapon);
	Weapon->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	//待修改，扔到正前方，掉到地面上
	FVector ItemOrigin=GetActorLocation() + FVector(0, 0, -70)+GetActorForwardVector()*120;
	FRotator ItemRotation = FRotator(90,GetBaseAimRotation().Yaw,0);
	
	FTransform PutDownLocation(ItemRotation, ItemOrigin);
	Weapon->Drop(PutDownLocation);
}

void ABaseCharacter::EnablePickUp()
{
//	bCanPickUp = true;
}

void ABaseCharacter::DisablePickUp()
{
//	bCanPickUp = false;
}

void ABaseCharacter::PickUpItem(ABaseItem* ItemToPickUp)
{
	ItemToPickUp->PickUp();
	if (ItemToPickUp->ItemType == EItemTypes::Item_Weapon)
	{ //如果是武器，放到武器列表中
		ABaseWeapon* WeaponToPickUp = Cast<ABaseWeapon>(ItemToPickUp);
		if(WeaponToPickUp){ MyWeaponInventory->AddToInventory(WeaponToPickUp); }		
	}
	else {
		MyItemInventory->AddToInventory(ItemToPickUp);
	}
}

void ABaseCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{

	ABaseItem* Item = Cast<ABaseWeapon>(OtherActor);
	if (Item != nullptr)
	{
		PickUpItem(Item);
	}
}




void ABaseCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}

void ABaseCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}


void ABaseCharacter::BeginCrouch()
{
	Crouch();
}
void ABaseCharacter::EndCrouch()
{
	UnCrouch();
}

void ABaseCharacter::BeginSprint() {
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	IsSprinting = true;
}
void ABaseCharacter::EndSprint() {
	GetCharacterMovement()->MaxWalkSpeed = 330.f;
	IsSprinting = false;
}
