// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "ShooterAnimInstance.h"
#include "GameFramework/Character.h"
#include "AmmoType.h"
#include "ShooterCharacter.generated.h"



UENUM(BlueprintType)
enum class ECombatState :uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_FireTimerInProgress UMETA(DisplayName = "FireTimerInProgress"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),

	ECS_MAX UMETA(DisplayName = "DefaultMAX")

};


UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{

	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**ǰ���ƶ�����*/
	void MoveForward(float Value);

    /*�����ƶ�����*/	
	void MoveRight(float Value);

	/**�����ӽ��ƶ�*/
	void TurnAtRate(float Rate);

	/*�����ӽ��ƶ�*/

	void LookUpAtRate(float Rate);

	/**
	* �������x���ƶ���ת������
	*����ΪValue ����ֵ����������ƶ�
	*/
	void Turn(float Value);

	/**
	* �������y���ƶ���ת������
	*����ΪValue ����ֵ����������ƶ�
	*/
	void LookUp(float Value);

    /*һ������*/
	void FireWeapon();

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

	/** ������׼��ʱ�任��׼*/
	void AimingButtonPressed();
	void AimingButtonReleased();

	void CameraInterpZoom(float DeltaTime);
	
	/**����������*/
	void SetLookRates();

	void CalculateCrosshairSpread(float DeltaTime);

	void StartCrosshairBulletFire();

	void FireButtonPressed();
	void FireButtonReleased();

    UFUNCTION()
	void FinishCrosshairBulletFire();

	void StartFireTimer();

	UFUNCTION()
	void AutoFireReset();

	//׼���µ����߼�⣬����ʾ��Ŀͼ��
	bool TraceUnderCrosshairs(FHitResult& OutHitResult,FVector& OutHitLocation);

	/**���ص���������0ʱ������Ʒ�������*/
	void TraceForItems();

	/**����Ĭ��������װ����*/
	class AWeapon* SpawnDefaultWeapon();

	/**�������������ŵ���������*/
	void EquipWeapon( AWeapon* WeaponToEquip);

	/**���������������������嵽����*/
	void DropWeapon();
	

	void SelectButtonPressed();
	void SelectButtonReleased();

	/**������ǰ��������������׷�ٵ���������*/
	void SwapWeapon(AWeapon* WeaponToSwap);
		
	/** ���ӵ�������ʼ���ӵ�map*/
	void InitializeAmmoMap();

	/**Check to make sure our weapon has ammo*/
	bool WeaponHasAmmo();

	/**���������ܺ���*/
	void PlayFireSound();
	void SendBullet();
	void PlayGunfireMontage();

	/**��RΪ������*/
	void ReloadButtonPressed();

	/**������*/
	void ReloadWeapon();

	/**�ж�����Ƿ���Я���������͵��ӵ�*/
	bool CarryingAmmo();

	/**����ͼ�е���ץȡ���ж����е�notify*/
	UFUNCTION(BlueprintCallable)
	void GrabClip();

	/**����ͼ�е����ͷŻ��������е�notify*/
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	
	/**meta = (AllowPrivateAccess = "true")��Ϊ����˽�в��ֱ�¶����ͼ�����Ƿ���*/
	/**��������ˣ����ɱۣ����������λ�ڽ�ɫ���沢�ҷ�ֹ��ײ*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/**Camera that follows the character*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/**�ֱ�����ת���ٶȣ��ԡ�/��Ϊ��λ��*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category=Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	/**�ֱ�����ת��*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	/**δ��׼ʱ����ת�� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipTurnRate;

	/**δ��׼ʱ����ת�� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipLookUpRate;

	/**��׼ʱ����ת�� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingTurnRate;

	/**��׼ʱ����ת�� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingLookUpRate;

	/**����׼ʱ��������ƶ���������Ӱ������*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"),meta =(ClampMin ="0.0",ClanmpMax ="1.0",UIMin ="0.0",UIMax="1.0"))
	float MouseHipTurnRate;

	/**����׼ʱ��������ƶ���������Ӱ������*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClanmpMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookUpRate;

	/**��׼ʱ��������ƶ���������Ӱ������*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClanmpMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingTurnRate;

	/**��׼ʱ��������ƶ���������Ӱ������*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClanmpMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingLookUpRate;

	/**���ǹ����ʾ*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	/**ǹ����������ϵͳ*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	/*�������̫��**/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	/**�ӵ���ײ����������*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	/**�ӵ�����켣*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	/*��׼**/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	/**Ĭ�������Ұ*/
	float CameraDefaultFOV;

	/**�Ҽ���׼�Ŵ�ʱ�������Ұ*/
	float CameraZoomedFOV;

	/**�����ǰ��Ұ*/
	float CameraCurrentFOV;

	/**�ӽ������ٶ�*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;
	 
	/**����ʮ��׼��ɢ������*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category=Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;

	/**ʮ��׼���������ƶ��ٶȱ仯ʱ��ɢ������ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;

	/**ʮ��׼���������ڿ���ʱ��ɢ������ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;

	/**ʮ��׼������׼ʱ��ɢ������ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor;

	/**ʮ��׼�������ʱ��ɢ������ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;

	/**��ǹ��ļ���ʱ�䣬���ڿ���*/
	float ShootTimeDuration;
	bool bFiringBullet;
	FTimerHandle CrosshairShootTimer;


	/**�Ƿ���������*/
	bool bFireButtonPressed;

	/**Ϊ����ǹ��Ϊ�ٵȴ���ʱ��*/
	bool bShouldFire;

	/**�Զ����������*/
	float AutomaticFireRate;

	/** ��ÿ�ο�ǹ֮�����ü�ʱ��*/
	FTimerHandle AutoFireTimer;

	/**��Ҫ���ÿһ֡����ĿʱΪ��*/
	bool bShouldTraceForItems;

	/** �ص���Ŀ������*/
	int8 OverlappedItemCount;

	/**���������׼����Ʒʱ���һ֡ʱ�Ķ�������ת�����ߺ��ø���Ʒ����Ϣͼ����ʧ*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Items, meta =(AllowPrivateAccess="true"))
	class AItem* TraceHitItemLastFrame;


	/**��ǰװ��������*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Combat, meta = (AllowPrivateAccess = "true"))
	 AWeapon* EquippedWeapon;


	/**����ͼ������Ĭ��������*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category=Combat,meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	/**����������TraceForItems����׷�����е���Ʒ������Ϊ�գ�*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItem;

	/**Ŀ�ĵ��������ˮƽ����ľ���*/
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float  CameraInterpDistance;

	/**Ŀ�ĵ����������ֱ���ϵľ���*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CameraInterpElevation;
	
	/** ��map���洢��ͬ���ӵ�����*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;

	/**9mm�ӵ����ǹ ��ʼ���е��ӵ�����*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 Starting9mmAmmo;

	/**ͻ����ǹ��ʼ���е��ӵ�����*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 StartingARAmmo;

	/**ս��״̬,ֻ����Unoccupied״̬�¿����������װ��*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category =Combat, meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

	/**����������̫��*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	/**�ڻ����ڼ��һ��ץȡ����ʱ�ĵ��еı任��ϢTransform*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FTransform ClipTransform;

	/**�����ڼ丽���������ֲ��ĳ������*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandSceneComponent;

	

public:
	/**Returns CameraBoom subobject(�Ӷ���),���Ϊ�����������Խ�ʡ�ռ�*/
	FORCEINLINE USpringArmComponent* GetCameraBoom()const { return CameraBoom ; }
	
	/**Returns FollowCamera subobject(�Ӷ���)*/
	FORCEINLINE UCameraComponent* GetFollowCamera()const { return FollowCamera ; }

	FORCEINLINE bool GetAiming() const { return bAiming; }

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	FORCEINLINE int8  GetOverlappedItemCount() const { return OverlappedItemCount; }

	//�����ص���Ŀ������������bShouldTraceForItems��״̬
	void IncrementOverlappedItemCount(int8 Amount);

	FVector GetCameraInterpLocation();

	void GetPickupItem(AItem* Item);

	FORCEINLINE ECombatState  GetCombatState() const { return CombatState; }
	
	

};
