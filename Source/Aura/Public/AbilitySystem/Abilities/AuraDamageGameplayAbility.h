// 青楼

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);//造成损害

	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(
		AActor* TargetActor = nullptr,
		FVector InRadialDamageOrigin = FVector::ZeroVector,//径向损伤起源
		bool bOverrideKnockbackDirection = false,//b 覆盖击退方向
		FVector KnockbackDirectionOverride = FVector::ZeroVector,//击退方向覆盖
		bool bOverrideDeathImpulse = false,//b 克服死亡冲动
		FVector DeathImpulseDirectionOverride = FVector::ZeroVector,//死亡冲动方向超控
		bool bOverridePitch = false,//b 覆盖音高
		float PitchOverride = 0.f//俯仰倍率
		) const;//从 classdefaults 创建伤害效果参数

	UFUNCTION(BlueprintPure)
	float GetDamageAtLevel() const;//在关卡时受到伤害

protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;//伤害效果类

	UPROPERTY(EditDefaultsOnly,Category = "Damage")
	FGameplayTag DamageType;//伤害类型

	UPROPERTY(EditDefaultsOnly,Category = "Damage")
	FScalableFloat Damage;//伤害

	UPROPERTY(EditDefaultsOnly,Category = "Damage")
	float DebuffChance = 20.f;//Debuff更新

	UPROPERTY(EditDefaultsOnly,Category = "Damage")
	float DebuffDamage = 5.f;//Debuff伤害

	UPROPERTY(EditDefaultsOnly,Category = "Damage")
	float DebuffFrequency = 1.f;//Debuff频率

	UPROPERTY(EditDefaultsOnly,Category = "Damage")
	float DebuffDuration = 5.f;//Debuff期间

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DeathImpulseMagnitude = 1000.f;//死亡冲量级

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float KnockbackForceMagnitude = 1000.f;//击退力大小

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float KnockbackChance = 0.f;//击退几率

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	bool bIsRadialDamage = false;//是径向损伤

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Damage")
	float RadialDamageInnerRadius = 0.f;//径向损伤内半径

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Damage")
	float RadialDamageOuterRadius = 0.f;//径向损伤外半径

	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const;//从数组中获取随机标记的蒙太奇
};
