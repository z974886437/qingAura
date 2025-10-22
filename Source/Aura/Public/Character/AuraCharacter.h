// 青楼

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"


class UCameraComponent;
class UNiagaraComponent;
class USpringArmComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase,public IPlayerInterface
{
	GENERATED_BODY()
public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController) override;//重写虚函数 PossessedBy()，它是 Unreal Engine（UE）中处理角色被控制器接管（Possess）时的关键函数
	virtual void OnRep_PlayerState() override;

	/* Player Interface */
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetXP_Implementation() const override;
	virtual int32 FindLevelForXP_Implementation(int32 InXP) const override;
	virtual int32 GetAttributePointsReward_Implementation(int32 Level) const override;//获得属性点奖励
	virtual int32 GetSpellPointsReward_Implementation(int32 Level) const override;//获得法术点奖励
	virtual void AddToPlayerLevel_Implementation(int32 InPlayerLevel) override;
	virtual void AddToAttributePoints_Implementation(int32 InAttributePoints) override;//添加到属性点
	virtual void AddToSpellPoints_Implementation(int32 InSpellPoints) override;//添加到法术点
	virtual int32 GetAttributePoints_Implementation() const override;//获取属性点
	virtual int32 GetSpellPoints_Implementation() const override;//获得法术点
	virtual void ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial = nullptr) override;//展示魔法阵
	virtual void HideMagicCircle_Implementation() override;//隐藏魔法阵
	
	
	/* end Player Interface */
	
	/* Combat Interface */
	virtual int32 GetPlayerLevel_Implementation() override;
	
	/* end Combat Interface */

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

	virtual void OnRep_Stunned() override;//代表震惊
	virtual void OnRep_Burned() override;//代表燃烧

private:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> TopDownCameraComponent;//自上而下的相机组件

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;//相机热潮
	
	virtual void InitAbilityActorInfo() override;//初始化能力Actor信息

	UFUNCTION(NetMulticast,Reliable)
	void MulticastLevelUpParticles() const;//多播升级粒子
};
