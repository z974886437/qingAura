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

FTaggedMontage UAuraDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const
{
	if (TaggedMontages.Num() > 0)// 如果数组中至少有一个元素
	{
		const int32 Selection = FMath::RandRange(0,TaggedMontages.Num() - 1);// 从 0 到 (数组长度-1) 之间随机生成一个整数索引
		return TaggedMontages[Selection];// 返回该索引对应的蒙太奇
	}

	return FTaggedMontage();// 如果数组是空的，直接返回一个默认构造的 FTaggedMontage（即空对象）
}

// 根据传入的伤害类型（DamageType）和技能等级（InLevel）返回伤害数值
float UAuraDamageGameplayAbility::GetDamageByDamageType(float InLevel, const FGameplayTag& DamageType)
{
	// 校验：DamageTypes 是否包含该伤害类型
	// 如果没有，会触发 checkf 崩溃并输出错误信息（方便调试）
	checkf(DamageTypes.Contains(DamageType),TEXT("GameplayAbility [%s] does not contain DamageType [%s]"),
		*GetNameSafe(this),// 技能名字（安全获取，即使对象是 nullptr 也不会崩）
		*DamageType.ToString() // 伤害类型的字符串形式
		);
	return DamageTypes[DamageType].GetValueAtLevel(InLevel);// 从 DamageTypes 映射表中取出对应的数值，并根据等级 InLevel 计算实际伤害
}
