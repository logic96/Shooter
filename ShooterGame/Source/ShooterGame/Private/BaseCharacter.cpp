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
	//����һ����Ʒ��
	MyItemInventory = CreateDefaultSubobject<UItemInventoryComponent>("MyItemInventory");
	//����һ�����������б�
	MyWeaponInventory = CreateDefaultSubobject<UWeaponInventoryComponent>("MyWeaponInventory");

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = 330.f;
	//��ʼ��CurrentWeapon
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CurrentWeapon = GetWorld()->SpawnActor<ABaseWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (CurrentWeapon)
	{	
		CurrentWeapon->SetActorEnableCollision(false);
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		//����ǰWeapon����MyWeaponInventory
		MyWeaponInventory->AddToInventory(CurrentWeapon);
	}
	//��ʼ������
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
	//���˾ѻ�ǹ�⣬������ShotGun����ǹ���Ǿ�������,���ձ���֮ҹ��������һ�黹���Ż�
	if (CurrentWeaponType != EWeaponTypes::Weapon_Rifle)
	{
		float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;
		float CurrentFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterSpeed);
		CameraComp->SetFieldOfView(CurrentFOV);
		//TODO:����Ҫ�����ƶ��ٶ�
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
	return (GetBaseAimRotation()-GetActorRotation()).GetNormalized();//������еĻ���������ԭʼ��
}

void ABaseCharacter::StartTargeting()
{
	bWantsToZoom = true;
	CurrentWeaponType = GetCurrentWeaponType();
	//ֻ�оѻ�ǹ�ǿ���׼��
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
				//�������ӵ��������ϵ�ǰǹ���ӵ��������ڵ���������
				MyItemInventory->ItemList[i]->ItemNums -= (CurrentWeapon->BulletCapacity - CurrentWeapon->BulletNums);//��һ��Ӧ�÷���ǰ��
				CurrentWeapon->BulletNums = CurrentWeapon->BulletCapacity;
				
			}
			else {
				//�������ӵ��������ϵ�ǰǹ���ӵ�����С�ڵ�������
				CurrentWeapon->BulletNums += MyItemInventory->ItemList[i]->ItemNums;
				MyItemInventory->ItemList[i]->ItemNums = 0;
				//���б����Ƴ��ӵ�����
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
		//һ������Ҳû�У��˳�
		return;
	}
	if (CurrentWeapon == nullptr)
	{  //��ͷû������ȡ�б��һ������
		CurrentWeapon = MyWeaponInventory->WeaponList[0];
	}
	else if(ListLength == 1){
		//��ֻ����ͷ�ϵ�һ���������˳�
		return;
	}
	else {
		//��ͷ���������б���Ҳ�����������Ƚ���ͷ����������,���Ҳ�����Tick
		CurrentWeapon->SetActorHiddenInGame(true);
		CurrentWeapon->SetActorTickEnabled(false);
		int index = FindWeapon(CurrentWeapon);
		if (index == INDEX_NONE)return;//����Ҳ����ͷ���
		if (index == ListLength - 1)
		{
			//��ͷ������Ϊ�б�����һ��,ȡ�б��һ������
			CurrentWeapon = MyWeaponInventory->WeaponList[0];			
		}
		else {
			//ȡ��һ��
			CurrentWeapon= MyWeaponInventory->WeaponList[index+1];
		}
	}
	//����ȡ����������ͷ����ʾ����
	CurrentWeapon->SetActorTickEnabled(true);
	CurrentWeapon->SetActorHiddenInGame(false);
	CurrentWeapon->SetActorEnableCollision(false);
	CurrentWeapon->SetOwner(this);
	CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
	//��ʱ�������ַ�ʽʹ�õ�ǰǹ֧�����˴���overlapʰȡ�¼�
	
}

void ABaseCharacter::PrevWeapon()
{
	int32 ListLength = MyWeaponInventory->WeaponList.Num();
	if (ListLength == 0) {
		//һ������Ҳû�У��˳�
		return;
	}
	if (CurrentWeapon == nullptr)
	{  //��ͷû������ȡ�б��һ������
		CurrentWeapon = MyWeaponInventory->WeaponList[0];
	}
	else if (ListLength == 1) {
		//��ֻ����ͷ�ϵ�һ���������˳�
		return;
	}
	else {
		//��ͷ���������б���Ҳ�����������Ƚ���ͷ����������,���Ҳ�����Tick
		CurrentWeapon->SetActorHiddenInGame(true);
		CurrentWeapon->SetActorTickEnabled(false);
		int index = FindWeapon(CurrentWeapon);
		if (index == INDEX_NONE)return;//����Ҳ����ͷ���
		if (index == 0)
		{
			//��ͷ������Ϊ�б�ĵ�һ��,ȡ�б����һ������
			CurrentWeapon = MyWeaponInventory->WeaponList[ListLength-1];
		}
		else {
			//ȡǰһ��
			CurrentWeapon = MyWeaponInventory->WeaponList[index - 1];
		}
	}
	//����ȡ����������ͷ����ʾ����
	CurrentWeapon->SetActorTickEnabled(true);
	CurrentWeapon->SetActorHiddenInGame(false);
	CurrentWeapon->SetActorEnableCollision(false);
	CurrentWeapon->SetOwner(this);
	CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
	//��ʱ�������ַ�ʽʹ�õ�ǰǹ֧�����˴���overlapʰȡ�¼�
}

FVector ABaseCharacter::GetPawnViewLocation()const {
	//��PlayerCameraManager����ȡ���Location
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

//��Ҫ�ṩDropItem


void ABaseCharacter::PickUpWeapon(ABaseWeapon* WeaponToPickUp)
{ //���Ҫϸ���߼��Ļ���������
	WeaponToPickUp->PickUp();
	MyWeaponInventory->AddToInventory(WeaponToPickUp);
}

int32 ABaseCharacter::FindWeapon(ABaseWeapon* WeaponToFind)
{
	for (int i = 0; i < MyWeaponInventory->WeaponList.Num(); i++)
	{
		if (MyWeaponInventory->WeaponList[i] == WeaponToFind)//������
			return i;
	}
	return INDEX_NONE;
}

//�������ϵ�ǹ
void ABaseCharacter::DropWeapon()
{	
	//û��ǹ��ʱ���ܹ�����
	if (MyWeaponInventory->WeaponList.Num() == 0) {
		return; }
	//��ͷûǹ��ʱ���ö�
	if (CurrentWeapon == nullptr)
	{
		return;
	}
	MyWeaponInventory->RemoveFromInventory(CurrentWeapon);
	CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	//���޸ģ��ӵ���ǰ��������������
	FVector ItemOrigin=GetActorLocation() + FVector(0, 0, -70)+GetActorForwardVector()*120;
	FRotator ItemRotation = FRotator(90,GetBaseAimRotation().Yaw,0);
	
	FTransform PutDownLocation(ItemRotation, ItemOrigin);
	CurrentWeapon->Drop(PutDownLocation);
	CurrentWeapon = nullptr;
}


//��ײ����¼�������ʰȡ
void ABaseCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{

	ABaseItem* Item = Cast<ABaseItem>(OtherActor);
	if (Item != nullptr)
	{
		if (Item->ItemType == EItemTypes::Item_Weapon)
		{ //������������ŵ������б���
			ABaseWeapon* WeaponToPickUp = Cast<ABaseWeapon>(Item);
			if (WeaponToPickUp) { PickUpWeapon(WeaponToPickUp); }
		}
		else {
			PickUpItem(Item);
		}
	}
}


//�ƶ����
void ABaseCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}

void ABaseCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}


//�¶����
void ABaseCharacter::BeginCrouch()
{
	Crouch();
}
void ABaseCharacter::EndCrouch()
{
	UnCrouch();
}

//������
void ABaseCharacter::BeginSprint() {
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	IsSprinting = true;
}
void ABaseCharacter::EndSprint() {
	GetCharacterMovement()->MaxWalkSpeed = 330.f;
	IsSprinting = false;
}

// ������󶨷����ı�ĩβ
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