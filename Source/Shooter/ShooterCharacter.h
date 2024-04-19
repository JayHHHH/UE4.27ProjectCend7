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

	/**前后移动输入*/
	void MoveForward(float Value);

    /*左右移动输入*/	
	void MoveRight(float Value);

	/**左右视角移动*/
	void TurnAtRate(float Rate);

	/*上下视角移动*/

	void LookUpAtRate(float Rate);

	/**
	* 基于鼠标x轴移动旋转控制器
	*参数为Value 输入值来自于鼠标移动
	*/
	void Turn(float Value);

	/**
	* 基于鼠标y轴移动旋转控制器
	*参数为Value 输入值来自于鼠标移动
	*/
	void LookUp(float Value);

    /*一键开火*/
	void FireWeapon();

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

	/** 按下瞄准键时变换瞄准*/
	void AimingButtonPressed();
	void AimingButtonReleased();

	void CameraInterpZoom(float DeltaTime);
	
	/**灵敏度设置*/
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

	//准星下的射线检测，以显示项目图标
	bool TraceUnderCrosshairs(FHitResult& OutHitResult,FVector& OutHitLocation);

	/**当重叠数量大于0时进行物品触碰检测*/
	void TraceForItems();

	/**生成默认武器并装备他*/
	class AWeapon* SpawnDefaultWeapon();

	/**拿起武器并附着到网格体上*/
	void EquipWeapon( AWeapon* WeaponToEquip);

	/**丢掉武器并让他自由落体到地面*/
	void DropWeapon();
	

	void SelectButtonPressed();
	void SelectButtonReleased();

	/**丢掉当前武器并换上射线追踪到的新武器*/
	void SwapWeapon(AWeapon* WeaponToSwap);
		
	/** 用子弹数量初始化子弹map*/
	void InitializeAmmoMap();

	/**Check to make sure our weapon has ammo*/
	bool WeaponHasAmmo();

	/**武器开火功能函数*/
	void PlayFireSound();
	void SendBullet();
	void PlayGunfireMontage();

	/**绑定R为换弹键*/
	void ReloadButtonPressed();

	/**处理换弹*/
	void ReloadWeapon();

	/**判断玩家是否有携带武器类型的子弹*/
	bool CarryingAmmo();

	/**从蓝图中调用抓取弹夹动画中的notify*/
	UFUNCTION(BlueprintCallable)
	void GrabClip();

	/**从蓝图中调用释放换弹动画中的notify*/
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	
	/**meta = (AllowPrivateAccess = "true")是为了让私有部分暴露给蓝图供我们访问*/
	/**摄像机吊杆（弹簧臂）将摄像机定位在角色后面并且防止碰撞*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/**Camera that follows the character*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/**手柄左右转向速度，以°/秒为单位。*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category=Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	/**手柄上下转速*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	/**未瞄准时左右转速 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipTurnRate;

	/**未瞄准时上下转速 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipLookUpRate;

	/**瞄准时左右转速 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingTurnRate;

	/**瞄准时上下转速 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingLookUpRate;

	/**不瞄准时鼠标左右移动的灵敏度影响因子*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"),meta =(ClampMin ="0.0",ClanmpMax ="1.0",UIMin ="0.0",UIMax="1.0"))
	float MouseHipTurnRate;

	/**不瞄准时鼠标上下移动的灵敏度影响因子*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClanmpMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookUpRate;

	/**瞄准时鼠标左右移动的灵敏度影响因子*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClanmpMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingTurnRate;

	/**瞄准时鼠标上下移动的灵敏度影响因子*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClanmpMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingLookUpRate;

	/**随机枪声提示*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	/**枪口闪光粒子系统*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	/*开火的蒙太奇**/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	/**子弹碰撞产生的粒子*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	/**子弹烟雾轨迹*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	/*瞄准**/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	/**默认相机视野*/
	float CameraDefaultFOV;

	/**右键瞄准放大时的相机视野*/
	float CameraZoomedFOV;

	/**相机当前视野*/
	float CameraCurrentFOV;

	/**视角缩放速度*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;
	 
	/**决定十字准星散布因子*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category=Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;

	/**十字准星在人物移动速度变化时的散布因子 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;

	/**十字准星在人物在空中时的散布因子 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;

	/**十字准星在瞄准时的散布因子 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor;

	/**十字准星在射击时的散布因子 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;

	/**开枪后的极短时间，用于控制*/
	float ShootTimeDuration;
	bool bFiringBullet;
	FTimerHandle CrosshairShootTimer;


	/**是否按下鼠标左键*/
	bool bFireButtonPressed;

	/**为真则开枪，为假等待计时器*/
	bool bShouldFire;

	/**自动开火的速率*/
	float AutomaticFireRate;

	/** 在每次开枪之间设置计时器*/
	FTimerHandle AutoFireTimer;

	/**需要检测每一帧的项目时为真*/
	bool bShouldTraceForItems;

	/** 重叠项目的数量*/
	int8 OverlappedItemCount;

	/**存放视线瞄准到物品时最后一帧时的对象，用于转移视线后让该物品的信息图标消失*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Items, meta =(AllowPrivateAccess="true"))
	class AItem* TraceHitItemLastFrame;


	/**当前装备的武器*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Combat, meta = (AllowPrivateAccess = "true"))
	 AWeapon* EquippedWeapon;


	/**在蓝图中设置默认武器类*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category=Combat,meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	/**现在正在由TraceForItems方法追踪命中的物品（可以为空）*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItem;

	/**目的地与摄像机水平向外的距离*/
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float  CameraInterpDistance;

	/**目的地与摄像机垂直向上的距离*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CameraInterpElevation;
	
	/** 用map来存储不同的子弹类型*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;

	/**9mm子弹冲锋枪 初始弹夹的子弹数量*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 Starting9mmAmmo;

	/**突击步枪初始弹夹的子弹数量*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 StartingARAmmo;

	/**战斗状态,只能在Unoccupied状态下开火或者重新装弹*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category =Combat, meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

	/**换弹动画蒙太奇*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	/**在换弹期间第一次抓取弹夹时的弹夹的变换信息Transform*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FTransform ClipTransform;

	/**换弹期间附着在人物手部的场景组件*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandSceneComponent;

	

public:
	/**Returns CameraBoom subobject(子对象),标记为内联函数可以节省空间*/
	FORCEINLINE USpringArmComponent* GetCameraBoom()const { return CameraBoom ; }
	
	/**Returns FollowCamera subobject(子对象)*/
	FORCEINLINE UCameraComponent* GetFollowCamera()const { return FollowCamera ; }

	FORCEINLINE bool GetAiming() const { return bAiming; }

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	FORCEINLINE int8  GetOverlappedItemCount() const { return OverlappedItemCount; }

	//计算重叠项目数量，并更新bShouldTraceForItems的状态
	void IncrementOverlappedItemCount(int8 Amount);

	FVector GetCameraInterpLocation();

	void GetPickupItem(AItem* Item);

	FORCEINLINE ECombatState  GetCombatState() const { return CombatState; }
	
	

};
