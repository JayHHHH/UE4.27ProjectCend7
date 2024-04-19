	 
// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include"Components/BoxComponent.h"
#include"Components/WidgetComponent.h"
#include"Components/SphereComponent.h"
#include"ShooterCharacter.h"
#include"Camera/CameraComponent.h"

// Sets default values
AItem::AItem():
    ItemCount(0),
    ItemName(FString("Default")),
	ItemRarity(EItemRarity::EIR_Common),
	ItemState(EItemState::EIS_Pickup),

	//物品移动过渡变量
	ZCurveTime(0.7f),
	ItemInterpStartLocation(FVector(0.f)),
	CameraTargetLocation(FVector(0.f)),
	bInterping(false),
	ItemInterpX(0.f),
	ItemInterpY(0.f),
	interpInitialYawOffset(0.f)


{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);


	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(ItemMesh);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Visibility, 
		ECollisionResponse::ECR_Block);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(GetRootComponent());

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());


}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	//默认情况下隐藏拾取物品图标
	if (PickupWidget) 

	{
 	     PickupWidget->SetVisibility(false);
	}

	//根据物品的稀有程度设计星星数量
	SetActiveStars();

	//为拾取物品设置重叠球体区域
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	/**根据状态设置物品组件的属性*/
	SetItemProperties(ItemState);

}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter) 
		{
			ShooterCharacter->IncrementOverlappedItemCount(1);
		}

	}

}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(-1);
		}
		
	}
}

void AItem::SetActiveStars()
{
	//6个数组元素存5颗星星，不使用"0“位置的数组元素
	for (int32 i = 0; i <= 5; i++) 
	{
		ActiveStars.Add(false);
	}
	switch (ItemRarity)
	{
	case EItemRarity::EIR_Damaged:
		ActiveStars[1] = true;
		break;
	case EItemRarity::EIR_Common:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		break;
	case EItemRarity::EIR_Uncommon:
		ActiveStars[1] = true;
		ActiveStars[2] = true; 
		ActiveStars[3] = true;
		break;
	case EItemRarity::EIR_Rare:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		break;
	case EItemRarity::EIR_Legendary:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		ActiveStars[4] = true;
		ActiveStars[5] = true;
		break;
	}

}

void AItem::SetItemProperties(EItemState State)
{
	switch (State)
	{
	case EItemState::EIS_Pickup:
		//设置mesh的属性
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	 	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	    //设置球体区域的属性
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		//设置碰撞盒属性
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_Visibility,
			ECollisionResponse::ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EItemState::EIS_Equipped:
		PickupWidget->SetVisibility(false);
		//设置mesh的属性
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// 设置球体区域的属性
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// 设置碰撞盒属性
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        break;
	case EItemState::EIS_Falling:
		// 设置mesh的属性
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_WorldStatic, 
			ECollisionResponse::ECR_Block);
		// 设置球体区域的属性
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// 设置碰撞盒属性
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_EquipInterping:
		PickupWidget->SetVisibility(false);
		// 设置mesh的属性
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// 设置球体区域的属性
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// 设置碰撞盒属性
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

void AItem::FinishInterping()
{
	bInterping = false;
	if (Character) 
	{
		Character->GetPickupItem(this);
	}
	//将物品比例设置正常
	SetActorScale3D(FVector(1.f));

}

void AItem::ItemInterp(float DeltaTime)
{
	if (!bInterping) return;
	
	if (Character && ItemZCurve)
	{
		// 存储从过渡计时器开始后的已用时间
		const  float  ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
		// 获取对应于“已用时间”的曲线值
		const  float  CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);
		UE_LOG(LogTemp, Warning, TEXT("CurveValue: %f"), CurveValue);
		// 获取物品按曲线移动开始时初始位置
		FVector ItemLocation = ItemInterpStartLocation;
		// 获得拾取物品期间预期悬停于相机某个视角的位置
		const FVector CameraInterpLocation{ Character->GetCameraInterpLocation() };
		// 物品与相机Z上的向量差，X和Y被设置为0
		const FVector ItemToCamera{ FVector(0.f,0.f,(CameraInterpLocation - ItemLocation).Z) };
		// 用于与曲线值相乘的比例因子
		const float DeltaZ = ItemToCamera.Size();


		const FVector CurrentLocation{ GetActorLocation() };
		// 过渡时x坐标的值
		const float InterpXValue = FMath::FInterpTo(
			CurrentLocation.X,
			CameraInterpLocation.X,
			DeltaTime,
			30.0f);

		// 过渡时y坐标的值
		const float InterpYValue = FMath::FInterpTo(
			CurrentLocation.Y,
			CameraInterpLocation.Y,
			DeltaTime,
			30.0f);

		// 设置X和Y
		ItemLocation.X = InterpXValue;
		ItemLocation.Y = InterpYValue;

		// 将曲线值添加到初始位置的Z分量中（按照比例因子初始化）
		ItemLocation.Z += CurveValue * DeltaZ;
		SetActorLocation(ItemLocation, true, nullptr, ETeleportType::TeleportPhysics);


		// 此帧时相机的旋转
		const FRotator CameraRotation{ Character->GetFollowCamera()->GetComponentRotation() };
		//相机的旋转+初始时的yaw偏移
		FRotator ItemRotation{ 0.f , CameraRotation.Yaw + interpInitialYawOffset, 0.f };
		SetActorRotation(ItemRotation, ETeleportType::TeleportPhysics);


		if (ItemScaleCurve)
		{
			const float  ScaleCurveValue = ItemScaleCurve->GetFloatValue(ElapsedTime);
			SetActorScale3D(FVector(ScaleCurveValue, ScaleCurveValue, ScaleCurveValue));
		}
	}


}


// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/**处理物品处于装备过渡状态时的过渡*/
	ItemInterp(DeltaTime);
}


void AItem::SetItemState(EItemState State)
{
	ItemState = State;
	SetItemProperties(State);
}

void AItem::StartItemCurve(AShooterCharacter* Char)
{
	//存储角色的句柄
	Character = Char;

	//存储物品的起始位置
	ItemInterpStartLocation = GetActorLocation();
	bInterping = true;
	SetItemState(EItemState::EIS_EquipInterping);

	GetWorldTimerManager().SetTimer(
		ItemInterpTimer, 
		this, 
		&AItem::FinishInterping,
		ZCurveTime);
	

	//获得初始时相机的Yaw偏移
	const float CameraRotationYaw{ Character->GetFollowCamera()->GetComponentRotation().Yaw };
	//获得初始时物品的Yaw偏移
	const float ItemRotationYaw{ GetActorRotation().Yaw };
	//初始时相机和过渡状态物品的yaw方向上的偏移
	interpInitialYawOffset = ItemRotationYaw - CameraRotationYaw;

}
