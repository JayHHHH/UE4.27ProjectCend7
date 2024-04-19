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
	//ȷ�����ǿ�ָ��

	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}
	
	//ȷ��PawnOwner �Ƿ�ɹ�CastΪAShooterCharater
	if (ShooterCharacter)
	{
		bReloading = ShooterCharacter->GetCombatState() == ECombatState::ECS_Reloading;


		//��ý�ɫ�ٶ�
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		//��ˮƽ�ٶ���0��ֻ��ȡ�����ٶ�
		Velocity.Z = 0;
		Speed = Velocity.Size();

		//�жϽ�ɫ�Ƿ��ڿ���
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		//�Ƿ��ڼ���(ͨ�������ٶ������Ĵ�С�Ƿ�Ϊ0)
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}

		//��ȡ�����ɫ�ĳ�����ƶ����򣬲����������������֮���ƫ��ֵ���ڽ�ɫ�ƶ�ʱʹ�ø�ƫ��ֵ�����������ɫ���ƶ��������š�
		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();//��ȡ�����ɫ��ǰ��׼�ĳ���
		FRotator MovementRotation =UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());//��ȡ�����ɫ��ǰ���ٶ�����������X����ת90�ȣ��Ӷ��õ�һ��ƽ���ڵ������תֵ��
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation,AimRotation).Yaw; 

		//��ֹͣ�ƶ�ǰ��������״ֵ̬,�Ա�ֹͣ��������ȷ����.
		if (ShooterCharacter->GetVelocity().Size() > 0.f) {
			LastMovementOffsetYaw = MovementOffsetYaw;
		}
		bAiming = ShooterCharacter->GetAiming();

	}

	TurnInPlace();
}



//��ʼ��
void UShooterAnimInstance::NativeInitializeAnimation()
{

	//��ΪTryGetPawnOwner()ֻ�ܷ���һ��pawn������Ҫǿ��ת����Character
	 ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());

}
  
void UShooterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter == nullptr)return;

	Pitch = ShooterCharacter->GetBaseAimRotation().Pitch;

	//���ڴ�ӡս��Ԫ��
	CombatStateString = UEnum::GetValueAsString(ShooterCharacter->GetCombatState());
	 

	if (Speed > 0)
	{
       //�ƶ��в�ִ��ԭ��תȦ
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


		//�������һ������Χ [-180, 180]����������ȷ���������� Yaw ƫ����ʼ����һ������ķ�Χ�ڡ�
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - YawDelta);

		//��ö�������ʱ����Turning��ֵ,����ʱΪ1.0��������ʱΪ0.0
		const float Turning{ GetCurveValue(TEXT("Turning")) };
		if (Turning > 0) 
		{
			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
			const float DeltaRotation{ RotationCurve - RotationCurveLastFrame };

			//RootYawOffset > 0,��ת��RootYawOffset<0,��ת
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



