// Fill out your copyright notice in the Description page of Project Settings.


#include "public/BaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "public/BaseWeapon.h"
#include "kismet/GameplayStatics.h"
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
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = 330.f;
	//spawns a default weapon
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CurrentWeapon = GetWorld()->SpawnActor<ABaseWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (CurrentWeapon)
	{
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
	}
	DefaultFOV = CameraComp->FieldOfView;
}


EWeaponTypes ABaseCharacter::GetCurrentWeaponType()
{
	if (CurrentWeapon) { return CurrentWeapon->GunType; }
	else return EWeaponTypes::NoWeapon;
}
//这个函数在蓝图中的调用有问题
//ABaseWeapon* ABaseCharacter::GetCurrentWeapon()
//{
//	return CurrentWeapon;
//}
// Called every frame
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
}

void ABaseCharacter::OnFire() 
{
	if (CurrentWeapon != nullptr)
	{

		IsFiring = true;
	//	CurrentWeapon->Fire();
		CurrentWeapon->PullTrigger();
	}
}

void ABaseCharacter::StopFire()
{
	if (CurrentWeapon != nullptr)
	{

		IsFiring = false;
		//	CurrentWeapon->Fire();
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

void ABaseCharacter::BeginZoom()
{
	bWantsToZoom = true;
	CurrentWeaponType = GetCurrentWeaponType();
	if (CurrentWeaponType == EWeaponTypes::NoWeapon) {
		//DoNothing
	}
	else if (CurrentWeaponType == EWeaponTypes::Weapon_ShotGun) {
		//DoNothing,让Tick处理
	}
	else {
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC) {
			PC->SetViewTargetWithBlend(CurrentWeapon, 0.1f, EViewTargetBlendFunction::VTBlend_Linear);
		}
	}
}

void ABaseCharacter::EndZoom()
{
	CurrentWeaponType = GetCurrentWeaponType();
	if (CurrentWeaponType == EWeaponTypes::NoWeapon) {
		//DoNothing
	}
	else if (CurrentWeaponType == EWeaponTypes::Weapon_ShotGun) {
		//DoNothing,让Tick处理
	}
	else {
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC) {
			PC->SetViewTargetWithBlend(this, 0.1f, EViewTargetBlendFunction::VTBlend_Linear);
		}
	}
	bWantsToZoom = false;
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
