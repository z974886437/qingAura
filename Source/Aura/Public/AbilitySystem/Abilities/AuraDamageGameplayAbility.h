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

	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor = nullptr) const;//从 classdefaults 创建伤害效果参数

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

	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const;//从数组中获取随机标记的蒙太奇
};
