// Fill out your copyright notice in the Description page of Project Settings.


#include "public/BaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "Public/BaseItem.h"
#include "Public/BaseWeapon.h"
#include "kismet/GameplayStatics.h"
#include "Public/ItemInventoryComponent.h"
#include "Public/WeaponInventoryComponent.h"
#include "Engine/World.h"


// Sets default values
ABaseCharacter::ABaseCharacter()
{

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
	//创建一个物品库
	MyItemInventory = CreateDefaultSubobject<UItemInventoryComponent>("MyItemInventory");
	//创建一个武器库库存列表
	MyWeaponInventory = CreateDefaultSubobject<UWeaponInventoryComponent>("MyWeaponInventory");

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = 330.f;
	//初始化CurrentWeapon
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CurrentWeapon = GetWorld()->SpawnActor<ABaseWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (CurrentWeapon)
	{	
		CurrentWeapon->SetActorEnableCollision(false);
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		//将当前Weapon加入MyWeaponInventory
		MyWeaponInventory->AddToInventory(CurrentWeapon);
	}
	//初始化焦距
	DefaultFOV = CameraComp->FieldOfView;
}


EWeaponTypes ABaseCharacter::GetCurrentWeaponType()
{
	if (CurrentWeapon) { return CurrentWeapon->GunType; }
	else return EWeaponTypes::Weapon_NoGun;
}

ABaseWeapon* ABaseCharacter::GetCurrentWeapon() const
{
	return CurrentWeapon;
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//除了狙击枪外，其他的ShotGun和榴弹枪等是距离拉近,仿照堡垒之夜，开镜这一块还能优化
	if (CurrentWeaponType != EWeaponTypes::Weapon_Rifle)
	{
		float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;
		float CurrentFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterSpeed);
		CameraComp->SetFieldOfView(CurrentFOV);
		//TODO:还需要设置移动速度
	}
}


void ABaseCharacter::StartFire()
{
	if (CurrentWeapon != nullptr)
	{

		IsFiring = true;
		CurrentWeapon->PullTrigger();
	}
}

void ABaseCharacter::StopFire()
{
	if (CurrentWeapon != nullptr)
	{

		IsFiring = false;
		CurrentWeapon->ReleaseTrigger();
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

void ABaseCharacter::StartTargeting()
{
	bWantsToZoom = true;
	CurrentWeaponType = GetCurrentWeaponType();
	//只有狙击枪是开瞄准镜
	if (CurrentWeaponType == EWeaponTypes::Weapon_Rifle) {
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC) {
			PC->SetViewTargetWithBlend(CurrentWeapon, 0.1f, EViewTargetBlendFunction::VTBlend_Linear);
		}
	}
}

void ABaseCharacter::StopTargeting()
{
	bWantsToZoom = false;
	CurrentWeaponType = GetCurrentWeaponType();
	if (CurrentWeaponType == EWeaponTypes::Weapon_Rifle) {
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC) {
			PC->SetViewTargetWithBlend(this, 0.1f, EViewTargetBlendFunction::VTBlend_Linear);
		}
	}
}

void ABaseCharacter::ReloadAmmo()
{
	if (CurrentWeapon == nullptr)return;
	for (int i = 0; i < MyItemInventory->ItemList.Num(); i++)
	{
		if(CurrentWeapon->AmmoName== MyItemInventory->ItemList[i]->ItemName){
			if (MyItemInventory->ItemList[i]->ItemNums+ CurrentWeapon->BulletNums > CurrentWeapon->BulletCapacity) {
				//背包内子弹数量加上当前枪内子弹数量多于弹夹数量，
				MyItemInventory->ItemList[i]->ItemNums -= (CurrentWeapon->BulletCapacity - CurrentWeapon->BulletNums);//这一句应该放在前面
				CurrentWeapon->BulletNums = CurrentWeapon->BulletCapacity;
				
			}
			else {
				//背包内子弹数量加上当前枪内子弹数量小于弹夹容量
				CurrentWeapon->BulletNums += MyItemInventory->ItemList[i]->ItemNums;
				MyItemInventory->ItemList[i]->ItemNums = 0;
				//从列表中移除子弹盒子
				MyItemInventory->RemoveFromInventory(MyItemInventory->ItemList[i]);
			//	MyItemInventory->ItemList[i]->Destroy();
			}
			
		}
	}
}

void ABaseCharacter::NextWeapon()
{
	int32 ListLength = MyWeaponInventory->WeaponList.Num();
	if (ListLength == 0) {
		//一把武器也没有，退出
		return;
	}
	if (CurrentWeapon == nullptr)
	{  //手头没武器，取列表第一个武器
		CurrentWeapon = MyWeaponInventory->WeaponList[0];
	}
	else if(ListLength == 1){
		//就只有手头上的一把武器，退出
		return;
	}
	else {
		//手头有武器，列表中也有武器，首先将手头的武器隐藏,并且不进行Tick
		CurrentWeapon->SetActorHiddenInGame(true);
		CurrentWeapon->SetActorTickEnabled(false);
		int index = FindWeapon(CurrentWeapon);
		if (index == INDEX_NONE)return;//如果找不到就返回
		if (index == ListLength - 1)
		{
			//手头的武器为列表的最后一把,取列表第一把武器
			CurrentWeapon = MyWeaponInventory->WeaponList[0];			
		}
		else {
			//取下一把
			CurrentWeapon= MyWeaponInventory->WeaponList[index+1];
		}
	}
	//把所取的武器在手头上显示出来
	CurrentWeapon->SetActorTickEnabled(true);
	CurrentWeapon->SetActorHiddenInGame(false);
	CurrentWeapon->SetActorEnableCollision(false);
	CurrentWeapon->SetOwner(this);
	CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
	//暂时采用这种方式使得当前枪支不和人触发overlap拾取事件
	
}

void ABaseCharacter::PrevWeapon()
{
	int32 ListLength = MyWeaponInventory->WeaponList.Num();
	if (ListLength == 0) {
		//一把武器也没有，退出
		return;
	}
	if (CurrentWeapon == nullptr)
	{  //手头没武器，取列表第一个武器
		CurrentWeapon = MyWeaponInventory->WeaponList[0];
	}
	else if (ListLength == 1) {
		//就只有手头上的一把武器，退出
		return;
	}
	else {
		//手头有武器，列表中也有武器，首先将手头的武器隐藏,并且不进行Tick
		CurrentWeapon->SetActorHiddenInGame(true);
		CurrentWeapon->SetActorTickEnabled(false);
		int index = FindWeapon(CurrentWeapon);
		if (index == INDEX_NONE)return;//如果找不到就返回
		if (index == 0)
		{
			//手头的武器为列表的第一把,取列表最后一把武器
			CurrentWeapon = MyWeaponInventory->WeaponList[ListLength-1];
		}
		else {
			//取前一把
			CurrentWeapon = MyWeaponInventory->WeaponList[index - 1];
		}
	}
	//把所取的武器在手头上显示出来
	CurrentWeapon->SetActorTickEnabled(true);
	CurrentWeapon->SetActorHiddenInGame(false);
	CurrentWeapon->SetActorEnableCollision(false);
	CurrentWeapon->SetOwner(this);
	CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
	//暂时采用这种方式使得当前枪支不和人触发overlap拾取事件
}

FVector ABaseCharacter::GetPawnViewLocation()const {
	//用PlayerCameraManager来获取相机Location
	APlayerCameraManager* PCM = UGameplayStatics::GetPlayerCameraManager(this,0);
	if (PCM) {
		return PCM->GetCameraLocation();
	}
	return Super::GetPawnViewLocation();
}
void ABaseCharacter::PickUpItem(ABaseItem* ItemToPickUp)
{
	ItemToPickUp->PickUp();
	MyItemInventory->AddToInventory(ItemToPickUp);
}

//需要提供DropItem


void ABaseCharacter::PickUpWeapon(ABaseWeapon* WeaponToPickUp)
{ //如果要细化逻辑的话在这儿添加
	WeaponToPickUp->PickUp();
	MyWeaponInventory->AddToInventory(WeaponToPickUp);
}

int32 ABaseCharacter::FindWeapon(ABaseWeapon* WeaponToFind)
{
	for (int i = 0; i < MyWeaponInventory->WeaponList.Num(); i++)
	{
		if (MyWeaponInventory->WeaponList[i] == WeaponToFind)//待检验
			return i;
	}
	return INDEX_NONE;
}

//丢弃手上的枪
void ABaseCharacter::DropWeapon()
{	
	//没有枪的时候不能够丢弃
	if (MyWeaponInventory->WeaponList.Num() == 0) {
		return; }
	//手头没枪的时候不用丢
	if (CurrentWeapon == nullptr)
	{
		return;
	}
	MyWeaponInventory->RemoveFromInventory(CurrentWeapon);
	CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	//待修改，扔到正前方，掉到地面上
	FVector ItemOrigin=GetActorLocation() + FVector(0, 0, -70)+GetActorForwardVector()*120;
	FRotator ItemRotation = FRotator(90,GetBaseAimRotation().Yaw,0);
	
	FTransform PutDownLocation(ItemRotation, ItemOrigin);
	CurrentWeapon->Drop(PutDownLocation);
	CurrentWeapon = nullptr;
}


//碰撞检测事件，触发拾取
void ABaseCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{

	ABaseItem* Item = Cast<ABaseItem>(OtherActor);
	if (Item != nullptr)
	{
		if (Item->ItemType == EItemTypes::Item_Weapon)
		{ //如果是武器，放到武器列表中
			ABaseWeapon* WeaponToPickUp = Cast<ABaseWeapon>(Item);
			if (WeaponToPickUp) { PickUpWeapon(WeaponToPickUp); }
		}
		else {
			PickUpItem(Item);
		}
	}
}


//移动相关
void ABaseCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}

void ABaseCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}


//下蹲相关
void ABaseCharacter::BeginCrouch()
{
	Crouch();
}
void ABaseCharacter::EndCrouch()
{
	UnCrouch();
}

//冲刺相关
void ABaseCharacter::BeginSprint() {
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	IsSprinting = true;
}
void ABaseCharacter::EndSprint() {
	GetCharacterMovement()->MaxWalkSpeed = 330.f;
	IsSprinting = false;
}

// 将输入绑定放在文本末尾
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
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABaseCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABaseCharacter::StopFire);
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ABaseCharacter::StartTargeting);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ABaseCharacter::StopTargeting);
	PlayerInputComponent->BindAction("DropWeapon", IE_Pressed, this, &ABaseCharacter::DropWeapon);
	PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, this, &ABaseCharacter::NextWeapon);
	PlayerInputComponent->BindAction("PrevWeapon", IE_Pressed, this, &ABaseCharacter::PrevWeapon);
	PlayerInputComponent->BindAction("ReloadAmmo", IE_Pressed, this, &ABaseCharacter::ReloadAmmo);
}