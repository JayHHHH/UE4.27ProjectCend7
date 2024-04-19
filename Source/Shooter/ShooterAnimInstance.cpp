// Fill out your copyright notice in the Description page of Project Settings.
#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/KismetMathLibrary.h"

UShooterAnimInstance::UShooterAnimInstance() :
	Speed(0.f),
	bIsInAir(false),
	bIsAccelerating(false),
	MovementOffsetYaw(0.f),
	LastMovementOffsetYaw(0.f),
	bAiming(false),
	CharacterYaw(0.f),
	CharacterYawLastFrame(0.f),
	RootYawOffset(0.f),
	Pitch(0.f),
	bReloading(false)
	
{
	
}
void UShooterAnimInstance::UpdateAnimationProperties(float DealtaTime)
{
	//确保不是空指针

	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}
	
	//确认PawnOwner 是否成功Cast为AShooterCharater
	if (ShooterCharacter)
	{
		bReloading = ShooterCharacter->GetCombatState() == ECombatState::ECS_Reloading;


		//获得角色速度
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		//把水平速度置0，只获取横向速度
		Velocity.Z = 0;
		Speed = Velocity.Size();

		//判断角色是否在空中
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		//是否在加速(通过看加速度向量的大小是否为0)
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}

		//获取射击角色的朝向和移动方向，并计算出这两个方向之间的偏差值，在角色移动时使用该偏差值来调整射击角色的移动动画播放。
		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();//获取射击角色当前瞄准的朝向
		FRotator MovementRotation =UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());//获取射击角色当前的速度向量将其沿X轴旋转90度，从而得到一个平行于地面的旋转值。
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation,AimRotation).Yaw; 

		//在停止移动前保留最后的状态值,以便停止动画的正确播放.
		if (ShooterCharacter->GetVelocity().Size() > 0.f) {
			LastMovementOffsetYaw = MovementOffsetYaw;
		}
		bAiming = ShooterCharacter->GetAiming();

	}

	TurnInPlace();
}



//初始化
void UShooterAnimInstance::NativeInitializeAnimation()
{

	//因为TryGetPawnOwner()只能返回一个pawn所以需要强制转换成Character
	 ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());

}
  
void UShooterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter == nullptr)return;

	Pitch = ShooterCharacter->GetBaseAimRotation().Pitch;

	//用于打印战斗元素
	CombatStateString = UEnum::GetValueAsString(ShooterCharacter->GetCombatState());
	 

	if (Speed > 0)
	{
       //移动中不执行原地转圈
		RootYawOffset = 0.f;
		CharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		CharacterYawLastFrame = CharacterYaw;
		RotationCurveLastFrame = 0.f;
		RotationCurve = 0.f;
	}
	else
	{
		CharacterYawLastFrame = CharacterYaw;
		CharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		const float YawDelta{ CharacterYaw - CharacterYawLastFrame };


		//将结果归一化到范围 [-180, 180]。这样可以确保根骨骼的 Yaw 偏移量始终在一个合理的范围内。
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - YawDelta);

		//获得动画播放时曲线Turning的值,播放时为1.0，不播放时为0.0
		const float Turning{ GetCurveValue(TEXT("Turning")) };
		if (Turning > 0) 
		{
			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
			const float DeltaRotation{ RotationCurve - RotationCurveLastFrame };

			//RootYawOffset > 0,左转，RootYawOffset<0,右转
			RootYawOffset > 0 ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;
		
			const float ABSRootYawOffset{FMath::Abs(RootYawOffset)};
			if (ABSRootYawOffset > 90.f)
			{
				const float YawExcess{ ABSRootYawOffset - 90.f };
				RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}
		}
		if (GEngine) GEngine->AddOnScreenDebugMessage(1, -1, FColor::Cyan, FString::Printf(TEXT("RootYawOffset: %f"), RootYawOffset));
		if (GEngine) GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Combat State: %s"), *CombatStateString));
	}


}



