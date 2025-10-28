// 青楼


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	// 创建一个待发送的 GameplayEffectSpec，用于描述即将施加的伤害效果
	// DamageEffectClass = 伤害效果蓝图/类，1.f = 技能等级（可调节伤害成长）
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass,1.f);
	const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());   // 根据当前技能等级，计算该伤害类型的实际数值（例如：火焰伤害 = 50）
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle,DamageType,ScaledDamage);// 把这个伤害值绑定到 Spec 上，通过 Tag 标记（SetByCaller），供执行时使用
	// 将准备好的伤害 Spec 应用到目标身上
	// 这里获取目标的 AbilitySystemComponent，然后执行应用
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(),
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
	
}

// 从技能类的默认值生成伤害效果参数，返回 FDamageEffectParams 结构体
FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor,
	FVector InRadialDamageOrigin, bool bOverrideKnockbackDirection, FVector KnockbackDirectionOverride,
	bool bOverrideDeathImpulse, FVector DeathImpulseDirectionOverride,bool bOverridePitch, float PitchOverride) const
{
	FDamageEffectParams Params;// 定义一个伤害效果参数结构体
	Params.WorldContextObject = GetAvatarActorFromActorInfo();// 设置上下文对象，这里是技能的施法者（AvatarActor）
	Params.DamageGameplayEffectClass = DamageEffectClass;// 设置伤害效果类，通常是一个 GameplayEffect 蓝图类
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();// 设置来源的能力系统组件（施法者的 ASC）
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);// 设置目标的能力系统组件（通过目标 Actor 获取）
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());// 设置基础伤害数值，根据技能等级从曲线/表格中取值
	Params.AbilityLevel = GetAbilityLevel();// 设置技能等级，用于计算效果强度
	Params.DamageType = DamageType;// 设置伤害类型（如火焰、冰霜等，通常是 GameplayTag 或枚举）
	Params.DebuffChance = DebuffChance;// 设置 Debuff 触发几率
	Params.DebuffDamage = DebuffDamage;// 设置 Debuff 每次触发的伤害
	Params.DebuffDuration = DebuffDuration;// 设置 Debuff 持续时间
	Params.DebuffFrequency = DebuffFrequency;// 设置 Debuff 触发频率（如每秒几次）
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;//设置死亡冲量级
	Params.KnockbackForceMagnitude = KnockbackForceMagnitude;// 设置击退力的大小
	Params.KnockbackChance = KnockbackChance;// 设置击退几率

	if (IsValid(TargetActor))// 如果目标有效，进行方向计算
	{
		FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();// 计算目标与施法者之间的旋转
		
		if (bOverridePitch)// 如果需要覆盖 Pitch 角度，则进行覆盖
		{
			Rotation.Pitch = PitchOverride;// 使用自定义的 Pitch 角度
		}
		
		const FVector ToTarget = Rotation.Vector();// 获取指向目标的方向向量
		
		if (!bOverrideKnockbackDirection) // 如果不覆盖击退方向，则计算击退力
		{
			Params.KnockbackForce = ToTarget * KnockbackForceMagnitude;// 设置击退力
		}
		
		if (!bOverrideDeathImpulse)// 如果不覆盖死亡冲量，则计算死亡冲量
		{
			Params.DeathImpulse = ToTarget * DeathImpulseMagnitude;// 设置死亡冲量
		}
	}
	
	if (bOverrideKnockbackDirection) // 如果需要覆盖击退方向
	{
		KnockbackDirectionOverride.Normalize();// 确保方向向量规范化
		Params.KnockbackForce = KnockbackDirectionOverride * KnockbackForceMagnitude;// 设置自定义击退力
		if (bOverridePitch)// 如果需要覆盖 Pitch 角度，重新计算击退力
		{
			FRotator KnockbackRotation = KnockbackDirectionOverride.Rotation();
			KnockbackRotation.Pitch = PitchOverride;// 设置自定义 Pitch 角度
			Params.KnockbackForce = KnockbackRotation.Vector() * KnockbackForceMagnitude;// 更新击退力
		}
	}
	
	if (bOverrideDeathImpulse) // 如果需要覆盖死亡冲量
	{
		DeathImpulseDirectionOverride.Normalize();// 确保方向向量规范化
		Params.DeathImpulse = DeathImpulseDirectionOverride * DeathImpulseMagnitude;
		if (bOverridePitch) // 如果需要覆盖 Pitch 角度，重新计算死亡冲量
		{
			FRotator DeathImpulseRotation = DeathImpulseDirectionOverride.Rotation();
			DeathImpulseRotation.Pitch = PitchOverride;// 设置自定义 Pitch 角度
			Params.DeathImpulse = DeathImpulseRotation.Vector() * DeathImpulseMagnitude;// 更新死亡冲量
		}
	}
	
	if (bIsRadialDamage)// 如果是辐射伤害（RadialDamage）
	{
		Params.bIsRadialDamage = bIsRadialDamage;// 设置是否是辐射伤害
		Params.RadialDamageOrigin = InRadialDamageOrigin;// 设置辐射伤害起始位置
		Params.RadialDamageInnerRadius = RadialDamageInnerRadius;// 设置辐射伤害内半径
		Params.RadialDamageOuterRadius = RadialDamageOuterRadius; // 设置辐射伤害外半径
	}
	return Params;// 返回最终填充好的参数
}

float UAuraDamageGameplayAbility::GetDamageAtLevel() const
{
	return Damage.GetValueAtLevel(GetAbilityLevel());
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
