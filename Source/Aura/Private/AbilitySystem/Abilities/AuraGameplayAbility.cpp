// 青楼


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystem/AuraAttributeSet.h"

// 根据技能等级生成一段技能描述字符串
FString UAuraGameplayAbility::GetDescription(int32 Level)
{
	// 使用 FString::Printf 格式化字符串，插入技能名字和等级
	// 注意：这里的 <Default>、<Level> 是富文本标记（用于 UI 样式），不是普通 HTML
	return FString::Printf(TEXT("<Default>%s,</><Level>%d</>"),// 格式模板，%s = 技能名，%d = 技能等级
		L"Default Ability Name - LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum ",// 技能名（写死的测试文本）
		Level);// 技能等级（由函数参数传入）
}

// 根据传入的等级，生成下一等级的技能描述
FString UAuraGameplayAbility::GetNextLevelDescription(int32 Level)
{
	// 使用 FString::Printf 来格式化描述文本
	// 格式里用到了富文本标签 <Default> 和 <Level>，UI 会根据这些标签显示不同样式
	return FString::Printf(TEXT("<Default>Next Level: </><Level>%d</> \n<Default>Causes much more damage </>"),// 格式字符串，%d 会被替换成技能等级
		Level); // 要显示的技能等级
}

FString UAuraGameplayAbility::GetLockedDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Spell Locked Until Level: %d</>"),Level);
}

// 获取技能的魔力消耗（根据技能等级计算）
float UAuraGameplayAbility::GetManaCost(float InLevel) const
{
	float ManaCost = 0.f;// 初始化魔力消耗为 0
	if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())// 获取技能的消耗效果（GameplayEffect），判断是否存在
	{
		for (FGameplayModifierInfo Mod : CostEffect->Modifiers)// 遍历消耗效果中的所有修饰器（Modifiers）
		{
			if (Mod.Attribute == UAuraAttributeSet::GetManaAttribute())// 如果修饰的属性是 Mana（蓝条消耗）
			{
				Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel,ManaCost);// 尝试根据技能等级取出固定数值（Static Magnitude）
				break;// 找到后就退出循环（防止重复赋值）
			}
		}
	}
	return ManaCost;// 返回最终计算出的魔力消耗
}

// 获取技能冷却时间（根据技能等级计算）
float UAuraGameplayAbility::GetCooldown(float InLevel) const
{
	float Cooldown = 0.f;// 初始化冷却时间为 0
	if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())// 获取技能的冷却效果（GameplayEffect），判断是否存在
	{
		// 从冷却效果的持续时间（DurationMagnitude）中尝试获取固定数值
		// 如果定义了按等级变化的曲线，会根据 InLevel 返回对应冷却时间
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel,Cooldown);
	}
	return Cooldown;// 返回最终冷却时间
}
