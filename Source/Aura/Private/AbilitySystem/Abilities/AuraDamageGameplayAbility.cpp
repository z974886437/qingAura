// 青楼


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	// 创建一个待发送的 GameplayEffectSpec，用于描述即将施加的伤害效果
	// DamageEffectClass = 伤害效果蓝图/类，1.f = 技能等级（可调节伤害成长）
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass,1.f);
	for (TTuple<FGameplayTag,FScalableFloat> Pair : DamageTypes)// 遍历伤害类型字典（键=伤害标签，值=可随等级缩放的伤害数值）
	{
		const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());   // 根据当前技能等级，计算该伤害类型的实际数值（例如：火焰伤害 = 50）
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle,Pair.Key,ScaledDamage);// 把这个伤害值绑定到 Spec 上，通过 Tag 标记（SetByCaller），供执行时使用
	}
	// 将准备好的伤害 Spec 应用到目标身上
	// 这里获取目标的 AbilitySystemComponent，然后执行应用
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(),
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
	
}
