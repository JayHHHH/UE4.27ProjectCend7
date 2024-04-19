// Fill out your copyright notice in the Description page of Project Settings.
#include "ShooterCharacter.h"
#include "ShooterAnimInstance.h"
#include "GameFramework/SpringArmComponent.h"
#include"Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include"Kismet/GameplayStatics.h"
#include"Sound/SoundCue.h"
#include"Engine/SkeletalMeshSocket.h"
#include"DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include"Item.h"
#include"Components/WidgetComponent.h"
#include"Weapon.h"
#include"Components/BoxComponent.h"
#include"Components/WidgetComponent.h"
#include"Components/SphereComponent.h"

//设置初始值
AShooterCharacter::AShooterCharacter() :
	//控制角色在水平和垂直方向上的旋转速度
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	bAiming(false),

	//角色在瞄准和非瞄准状态下的旋转速率
	HipTurnRate(90.f),
	HipLookUpRate(90.f),
	AimingTurnRate(20.f),
	AimingLookUpRate(20.f),

	//鼠标控制的旋转速度的缩放因子
	MouseHipTurnRate(1.0f),
	MouseHipLookUpRate(1.0f),
	MouseAimingTurnRate(0.2f),
	MouseAimingLookUpRate(0.2f),

	//相机视野值
	CameraDefaultFOV(0.f),
	CameraZoomedFOV(35.f),
	CameraCurrentFOV(0.f),
	ZoomInterpSpeed(20.f),

	//准星扩散因素
	CrosshairSpreadMultiplier(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimFactor(0.f),
	CrosshairShootingFactor(0.f),

	//开火计时器元素
	ShootTimeDuration(0.05f),
	bFiringBullet(false),

	//自动开火速率
	AutomaticFireRate(0.1f),
	bShouldFire(true),
	bFireButtonPressed(false),
	//物品追踪元素
	bShouldTraceForItems(false),
	OverlappedItemCount(0),

	//相机过渡位置元素
	CameraInterpDistance(250.f),
	CameraInterpElevation(65.f),

	//初始子弹数量
	Starting9mmAmmo(85),
	StartingARAmmo(120),
	//战斗元素
	CombatState(ECombatState::ECS_Unoccupied)

{
	//游戏中每帧都调用tick()函数
	PrimaryActorTick.bCanEverTick = true;

	//创造一个相机摇杆
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 180.f;//实现相机跟随角色并在一定距离后方(第三人称)
	CameraBoom->bUsePawnControlRotation = true; //根据控制器旋转角色的手臂
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);

	//创造第三人称的相机
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);//将相机附加到弹簧的末端
	FollowCamera->bUsePawnControlRotation = false;//相机不随手臂旋转

	//禁止人物跟随控制器旋转,只旋转相机
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	//配置角色移动
	GetCharacterMovement()->bOrientRotationToMovement=false;//角色的旋转不会根据移动方向进行调整
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);//角色的旋转速率
	GetCharacterMovement()->JumpZVelocity = 600.f;//跳跃控制力
	GetCharacterMovement()->AirControl = 0.2f;//空中速度

	//创建HandSceneComponent
	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComp"));

	
}

//游戏开始或者角色被生成时，调用BeginPlay()函数
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (FollowCamera) {
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
	//角色生成时生成默认武器并装备
	EquipWeapon(SpawnDefaultWeapon());

	InitializeAmmoMap();

}
void AShooterCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{	
		//确定角色的前进方向
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0,Rotation.Yaw,0 };
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };

		AddMovementInput(Direction, Value);	
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		//确定角色的右边方向
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0,Rotation.Yaw,0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };

		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	/*通过传入一个浮点型乘以基本转速再乘以Delta time来确定左右转向的转速*/
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::Turn(float Value)
{
	float TurnScaleFactor{};
	if (bAiming) 
	{
		TurnScaleFactor = MouseAimingTurnRate;
	}
	else
	{
		TurnScaleFactor = MouseHipTurnRate;
	}
	AddControllerYawInput(Value * TurnScaleFactor);
}

void AShooterCharacter::LookUp(float Value)
{
	float LookUpScaleFactor{};
	if (bAiming)
	{
		LookUpScaleFactor = MouseAimingLookUpRate;
	}
	else
	{
		LookUpScaleFactor = MouseHipLookUpRate;
	}
	AddControllerPitchInput(Value * LookUpScaleFactor);
}

void AShooterCharacter::FireWeapon()
{
	if (EquippedWeapon == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if(WeaponHasAmmo())
	{
		PlayFireSound();
		SendBullet();
		PlayGunfireMontage();
		EquippedWeapon->DecrementAmmo();

		 
		StartFireTimer();
	}
}

bool AShooterCharacter::GetBeamEndLocation(
	const FVector& MuzzleSocketLocation,
	FVector& OutBeamLocation)
{

	//检测射线碰撞
	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult,OutBeamLocation);

	if (bCrosshairHit) {
		//临时射线位置，还需要从枪管处发起检测
		OutBeamLocation = CrosshairHitResult.Location;
	}
	else//没有碰撞到目标
	{
     //将 OutBeamLocation 设置为射线的结束位置
	}

	//从枪口执行第二次射线检测
	FHitResult WeaponTraceHit;
	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	const FVector StartToEnd{ OutBeamLocation - MuzzleSocketLocation };
	const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd * 1.25f };
	GetWorld()->LineTraceSingleByChannel(
		WeaponTraceHit,
		WeaponTraceStart,
		WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility);
	if (WeaponTraceHit.bBlockingHit)//检查是否有物体位于枪口和射线终点之间
	{
		OutBeamLocation = WeaponTraceHit.Location;
		return true;
	}
	return false;
}

void AShooterCharacter::AimingButtonPressed()
{
	bAiming = true;

}

void AShooterCharacter::AimingButtonReleased()
{
	bAiming = false;

}

void AShooterCharacter::CameraInterpZoom(float DeltaTime)
{
	//设置当前摄像机的视角
	if (bAiming)
	{
		//实现平滑缩放POV
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraZoomedFOV,
			DeltaTime,
			ZoomInterpSpeed);
		GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
	}
	else
	{
		//平滑恢复原POV
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraDefaultFOV,
			DeltaTime,
			ZoomInterpSpeed);
		GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
	}
}


void AShooterCharacter::SetLookRates()
{

	if (bAiming){
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AimingLookUpRate;
	}
	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}
}

void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange{ 0.f,600.f };
	FVector2D VelocityMultiplierRange{ 0.f,1.f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f;

	//计算准星扩散因素
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(
		WalkSpeedRange,
		VelocityMultiplierRange,
		Velocity.Size());

	//计算准星在空中的因素
	if (GetCharacterMovement()->IsFalling())
	{
		//在空中时缓慢扩散准星
		CrosshairInAirFactor = FMath::FInterpTo(
			CrosshairInAirFactor,
			2.25f,
			DeltaTime,
			2.25f);
	}
	else
	{
		//落地后快速收缩准星
		CrosshairInAirFactor = FMath::FInterpTo(
			CrosshairInAirFactor,
			0.f,
			DeltaTime,
			30.f);
	}

	//计算准星瞄准因素
	if (bAiming)
	{
		//瞄准时将准星很快地收缩在一起
		CrosshairAimFactor = FMath::FInterpTo(
			CrosshairAimFactor,
			0.6f,
			DeltaTime,
			30.f);
	}
	else//未瞄准时
	{
		//让准星快速恢复原状态
		CrosshairAimFactor = FMath::FInterpTo(
			CrosshairAimFactor,
			0.f,
			DeltaTime,
			30.f);
	}
	
	//开火0.05秒后为真
	if (bFiringBullet) {
		CrosshairShootingFactor = FMath::FInterpTo(
			CrosshairShootingFactor, 
			0.3f, 
			DeltaTime,
			60.f);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(
			CrosshairShootingFactor,
			0.f,
			DeltaTime,
			60.f);
	}

	CrosshairSpreadMultiplier = 
		0.5f + 
		CrosshairVelocityFactor + 
		CrosshairInAirFactor -
		CrosshairAimFactor +
		CrosshairShootingFactor;

}

void AShooterCharacter::StartCrosshairBulletFire()
{
	bFiringBullet = true;
	GetWorldTimerManager().SetTimer(
		CrosshairShootTimer, 
		this,
		&AShooterCharacter::FinishCrosshairBulletFire, 
		ShootTimeDuration);
}

void AShooterCharacter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
}

void AShooterCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;
	FireWeapon();
}
void AShooterCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooterCharacter::StartFireTimer() 
{
	CombatState = ECombatState::ECS_FireTimerInProgress;
		GetWorldTimerManager().SetTimer(
			AutoFireTimer,
			this,
			&AShooterCharacter::AutoFireReset,
			AutomaticFireRate);
}

void AShooterCharacter::AutoFireReset()
{
	CombatState = ECombatState::ECS_Unoccupied;

	if (WeaponHasAmmo())
	{ 
		if (bFireButtonPressed)
		{
			FireWeapon();
		}
	}
	else
	{
		//Reload Weapon
		ReloadWeapon();
	}


}



bool AShooterCharacter::TraceUnderCrosshairs(
	FHitResult& OutHitResult,
	FVector& OutHitLocation)
{
	//获取视口大小
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}


	//获取十字准星的屏幕空间位置
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	//获取十字准星的世界位置和方向
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld) {
		 
	//从十字准星的世界位置向外进行射线检测
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ Start + CrosshairWorldDirection * 50'000.f };
		OutHitLocation = End;
		GetWorld()->LineTraceSingleByChannel(
			OutHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);
		if (OutHitResult.bBlockingHit) 
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}
	return false;
}

void AShooterCharacter::TraceForItems()
{
	if (bShouldTraceForItems) {
		
		FHitResult ItemTraceResult;
		FVector HitLocation;
		TraceUnderCrosshairs(ItemTraceResult, HitLocation);
		if (ItemTraceResult.bBlockingHit)
		{
			 TraceHitItem = Cast<AItem>(ItemTraceResult.Actor);
			if (TraceHitItem && TraceHitItem->GetPickupWidget())
			{
				//显示物品的拾取窗口小部件
				TraceHitItem->GetPickupWidget()->SetVisibility(true);
			}

			//检测在上一帧是否击中了一个AItem对象
			if (TraceHitItemLastFrame)
			{
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					//在当前帧是否击中了一个与上一帧不同的AItem对象
					//ATtem为空
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
				}

			}
			//在下一帧中存储对HitItem的引用
			TraceHitItemLastFrame = TraceHitItem;
		}
	}
	else if (TraceHitItemLastFrame)
	{
		//检查是否不再与任何物品重叠
		//上一帧的物品不应显示小部件
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
	}
}

AWeapon* AShooterCharacter::SpawnDefaultWeapon()
{
	
	if (DefaultWeaponClass)
	{
		//生成一个武器
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}
	return nullptr;
}

void AShooterCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip) 
	{
		//获得手部插槽
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(
			FName("RightHAndSocket"));
		if (HandSocket)
		{
			//将武器附着于手部插槽"RightHandSocket"
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}

		//将EquippedWeapon设置为最新生成的武器
		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
}

void AShooterCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		FDetachmentTransformRules DetachnebtTransformRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachnebtTransformRules);

		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();
	}

}

void AShooterCharacter::SelectButtonPressed()
{
	if (TraceHitItem) 
	{
		TraceHitItem->StartItemCurve(this);

		if (TraceHitItem->GetPickupSound())
		{
			UGameplayStatics::PlaySound2D(this, TraceHitItem->GetPickupSound());
		}
	}
}

void AShooterCharacter::SelectButtonReleased()
{
}

void AShooterCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	DropWeapon();
	EquipWeapon(WeaponToSwap);


}

void AShooterCharacter::InitializeAmmoMap()
{

	AmmoMap.Add(EAmmoType::EAT_9mm, Starting9mmAmmo);
	AmmoMap.Add(EAmmoType::EAT_AR, StartingARAmmo);

}

bool AShooterCharacter::WeaponHasAmmo()
{
	if(EquippedWeapon == nullptr) return false;

	return EquippedWeapon->GetAmmo() > 0;
}

void AShooterCharacter::PlayFireSound()
{
	//播放开火音效
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);//播放开枪声音
	}
}

void AShooterCharacter::SendBullet()
{
	//Send bullet
	const USkeletalMeshSocket* BarrelSocket =
		EquippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");//获得枪管插座
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(
			EquippedWeapon->GetItemMesh());

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);//产生枪口火焰
		}

		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(
			SocketTransform.GetLocation(), BeamEnd);
		if (bBeamEnd)
		{
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					BeamEnd);//产生撞击效果粒子
			}
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				BeamParticles,
				SocketTransform);
			if (Beam)
			{

				Beam->SetVectorParameter(FName("Target"), BeamEnd);

			}
		}
	}
}

void AShooterCharacter::PlayGunfireMontage()
{
	//获取并初始化实例，播放开火动画蒙太奇
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
	//启动子弹射击计时器
	StartCrosshairBulletFire();

}

 void AShooterCharacter::ReloadButtonPressed()
{
	 ReloadWeapon();
}

 void AShooterCharacter::ReloadWeapon()
 {
	 if (CombatState != ECombatState::ECS_Unoccupied ) return;
	 if (EquippedWeapon == nullptr) return;

	 // 我们是否有正确的弹药类型？
	 if (CarryingAmmo() )
	 {
		 CombatState = ECombatState::ECS_Reloading;

		 UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		 if (AnimInstance && ReloadMontage)
		 {
			 AnimInstance->Montage_Play(ReloadMontage);
			 AnimInstance->Montage_JumpToSection(
				 EquippedWeapon->GetReloadMontageSection());
		 }
	 }
 }

 bool AShooterCharacter::CarryingAmmo()
 {

	 if (EquippedWeapon == nullptr) return false;

	 auto AmmoType = EquippedWeapon->GetAmmoType();

	 if (AmmoMap.Contains(AmmoType))
	 {
		 return AmmoMap[AmmoType] > 0;
	 }

	 return false;
 }

 void AShooterCharacter::GrabClip()
 {
	 if (EquippedWeapon == nullptr)return;
	 if (HandSceneComponent == nullptr)return;


	 //已装备武器的弹夹骨骼索引 
	 int32 ClipBoneIndex{ EquippedWeapon->GetItemMesh()->GetBoneIndex(EquippedWeapon->GetClipBoneName()) };
	 //存储弹夹的转换
	 ClipTransform= EquippedWeapon->GetItemMesh()->GetBoneTransform(ClipBoneIndex);

	 FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);
     HandSceneComponent->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("Hand_L")));
	 HandSceneComponent->SetWorldTransform(ClipTransform);


	 EquippedWeapon->SetMovingClip(true);

 }

 void AShooterCharacter::ReleaseClip()

 {
	 EquippedWeapon->SetMovingClip(false);
 }

 // Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//处理瞄准时的缩放插值
	CameraInterpZoom(DeltaTime);
	//根据瞄准状态更改视角敏感度
	SetLookRates();
	//计算准星散布倍数
	CalculateCrosshairSpread(DeltaTime);
	//检查重叠的物品总数，然后进行追踪
	TraceForItems();



	

}


void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);

	PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::LookUp);

	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, & ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this,
		&AShooterCharacter::FireButtonPressed);

	PlayerInputComponent->BindAction("FireButton", IE_Released, this,
		&AShooterCharacter::FireButtonReleased);


	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this,
		&AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, 
		&AShooterCharacter::AimingButtonReleased);

	PlayerInputComponent->BindAction("Select", IE_Pressed, this,
		&AShooterCharacter::SelectButtonPressed);
	PlayerInputComponent->BindAction("Select", IE_Released, this,
		&AShooterCharacter::SelectButtonReleased);


	PlayerInputComponent->BindAction("ReloadButton", IE_Released, this,
		&AShooterCharacter::ReloadButtonPressed);

}

void AShooterCharacter::FinishReloading()
{
	// Update the Combat State
	CombatState = ECombatState::ECS_Unoccupied;
	if (EquippedWeapon == nullptr) return;
	const auto AmmoType{ EquippedWeapon->GetAmmoType() };

	// Update the AmmoMap
	if (AmmoMap.Contains(AmmoType))
	{
		// Amount of ammo the Character is carrying of the EquippedWeapon type
		int32 CarriedAmmo = AmmoMap[AmmoType];

		// Space left in the magazine of EquippedWeapon
		const int32 MagEmptySpace =
			EquippedWeapon->GetMagazineCapacity() -
			EquippedWeapon->GetAmmo();

		if (MagEmptySpace > CarriedAmmo)
		{
			// Reload the magazine with all the ammo we are carrying
			EquippedWeapon->ReloadAmmo(CarriedAmmo);
			CarriedAmmo = 0;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
		else
		{
			// fill the magazine
			EquippedWeapon->ReloadAmmo(MagEmptySpace);
			CarriedAmmo -= MagEmptySpace;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
	}
}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

void AShooterCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}

FVector AShooterCharacter::GetCameraInterpLocation()
{
	const FVector CameraWorldLocation{ FollowCamera->GetComponentLocation() };
	const FVector CameraForward{ FollowCamera->GetForwardVector() };
	//Desired = CameraWorldLocation + Forward * A + Up * B
	return CameraWorldLocation + CameraForward * CameraInterpDistance
		+ FVector(0.f, 0.f, CameraInterpElevation);
	
}


void AShooterCharacter::GetPickupItem(AItem* Item)
{
	if (Item->GetEquipSound())
	{
		UGameplayStatics::PlaySound2D(this, Item->GetEquipSound());

	}
	auto Weapon = Cast<AWeapon>(Item);
	if (Weapon)
	{
		SwapWeapon(Weapon);
	}
}



