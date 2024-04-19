	 
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

	//��Ʒ�ƶ����ɱ���
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

	//Ĭ�����������ʰȡ��Ʒͼ��
	if (PickupWidget) 

	{
 	     PickupWidget->SetVisibility(false);
	}

	//������Ʒ��ϡ�г̶������������
	SetActiveStars();

	//Ϊʰȡ��Ʒ�����ص���������
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	/**����״̬������Ʒ���������*/
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
	//6������Ԫ�ش�5�����ǣ���ʹ��"0��λ�õ�����Ԫ��
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
		//����mesh������
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	 	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	    //�����������������
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		//������ײ������
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_Visibility,
			ECollisionResponse::ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EItemState::EIS_Equipped:
		PickupWidget->SetVisibility(false);
		//����mesh������
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// �����������������
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// ������ײ������
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        break;
	case EItemState::EIS_Falling:
		// ����mesh������
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_WorldStatic, 
			ECollisionResponse::ECR_Block);
		// �����������������
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// ������ײ������
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_EquipInterping:
		PickupWidget->SetVisibility(false);
		// ����mesh������
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// �����������������
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// ������ײ������
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
	//����Ʒ������������
	SetActorScale3D(FVector(1.f));

}

void AItem::ItemInterp(float DeltaTime)
{
	if (!bInterping) return;
	
	if (Character && ItemZCurve)
	{
		// �洢�ӹ��ɼ�ʱ����ʼ�������ʱ��
		const  float  ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
		// ��ȡ��Ӧ�ڡ�����ʱ�䡱������ֵ
		const  float  CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);
		UE_LOG(LogTemp, Warning, TEXT("CurveValue: %f"), CurveValue);
		// ��ȡ��Ʒ�������ƶ���ʼʱ��ʼλ��
		FVector ItemLocation = ItemInterpStartLocation;
		// ���ʰȡ��Ʒ�ڼ�Ԥ����ͣ�����ĳ���ӽǵ�λ��
		const FVector CameraInterpLocation{ Character->GetCameraInterpLocation() };
		// ��Ʒ�����Z�ϵ������X��Y������Ϊ0
		const FVector ItemToCamera{ FVector(0.f,0.f,(CameraInterpLocation - ItemLocation).Z) };
		// ����������ֵ��˵ı�������
		const float DeltaZ = ItemToCamera.Size();


		const FVector CurrentLocation{ GetActorLocation() };
		// ����ʱx�����ֵ
		const float InterpXValue = FMath::FInterpTo(
			CurrentLocation.X,
			CameraInterpLocation.X,
			DeltaTime,
			30.0f);

		// ����ʱy�����ֵ
		const float InterpYValue = FMath::FInterpTo(
			CurrentLocation.Y,
			CameraInterpLocation.Y,
			DeltaTime,
			30.0f);

		// ����X��Y
		ItemLocation.X = InterpXValue;
		ItemLocation.Y = InterpYValue;

		// ������ֵ��ӵ���ʼλ�õ�Z�����У����ձ������ӳ�ʼ����
		ItemLocation.Z += CurveValue * DeltaZ;
		SetActorLocation(ItemLocation, true, nullptr, ETeleportType::TeleportPhysics);


		// ��֡ʱ�������ת
		const FRotator CameraRotation{ Character->GetFollowCamera()->GetComponentRotation() };
		//�������ת+��ʼʱ��yawƫ��
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

	/**������Ʒ����װ������״̬ʱ�Ĺ���*/
	ItemInterp(DeltaTime);
}


void AItem::SetItemState(EItemState State)
{
	ItemState = State;
	SetItemProperties(State);
}

void AItem::StartItemCurve(AShooterCharacter* Char)
{
	//�洢��ɫ�ľ��
	Character = Char;

	//�洢��Ʒ����ʼλ��
	ItemInterpStartLocation = GetActorLocation();
	bInterping = true;
	SetItemState(EItemState::EIS_EquipInterping);

	GetWorldTimerManager().SetTimer(
		ItemInterpTimer, 
		this, 
		&AItem::FinishInterping,
		ZCurveTime);
	

	//��ó�ʼʱ�����Yawƫ��
	const float CameraRotationYaw{ Character->GetFollowCamera()->GetComponentRotation().Yaw };
	//��ó�ʼʱ��Ʒ��Yawƫ��
	const float ItemRotationYaw{ GetActorRotation().Yaw };
	//��ʼʱ����͹���״̬��Ʒ��yaw�����ϵ�ƫ��
	interpInitialYawOffset = ItemRotationYaw - CameraRotationYaw;

}
