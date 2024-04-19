// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UShooterAnimInstance();

	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DealtaTime);

	virtual void NativeInitializeAnimation() override;

protected:
	/** ����ԭ��ת��ı��� */
	void  TurnInPlace();




private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* ShooterCharacter;

	/**��ɫ�ƶ��ٶ�*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category=Movement, meta = (AllowPrivateAccess = "true"))
    float Speed;


	/*�жϽ�ɫ�Ƿ��ڿ�����**/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	/*�жϽ�ɫ�Ƿ������ƶ�**/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	/* ��׼λ�ú��ƶ������ƫ���������ڷ������ƶ��Ȳ������仯 **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category =Movement, meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw; 

	/*����ֹͣ�ƶ�ǰ��ƫ������ȷ��ֹͣ��������������ֹͣ��ƫ���ٶ�����Ϊ�㣬ƫ������ʧ���޷�������Ӧ���Һ����ֹͣ����*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	/**��ǰ֡ʱCharacter��Yaw*/
	float CharacterYaw;

	/**��һ֡Character��Yaw*/
	float CharacterYawLastFrame;

	/**������׼��ת��Yaw��������׼ƫ�� */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Turn In Place",meta=(AllowPrivateAccess = "true"))
	float RootYawOffset;

	/**��ǰ֡����ת����ֵ*/
	float RotationCurve;

	/**��һ֡����ת����ֵ*/
	float RotationCurveLastFrame;

	/**������׼��ת��Pitch��������׼ƫ�� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float Pitch;

	/**����ʱΪ�棬��ֹ����ʱ����׼ƫ�� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	bool bReloading;

	/**��ӡս��״̬*/
	FString CombatStateString;

	

};
