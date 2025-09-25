// 青楼


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AuraGameplayTags.h"

// 返回火球术技能的描述字符串（根据等级显示不同效果）
FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 Damage = GetDamageByDamageType(Level,FAuraGameplayTags::Get().Damage_Fire);// 根据技能等级获取火焰伤害数值
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)// 如果是等级 1，显示单发火球描述
	{
		return FString::Printf(TEXT(
			/* Title 标题*/
			"<Title>火球术</>\n\n"

			// Level
			"<Small>等级: </><Level>%d</>\n"
			//ManaCost
			"<Small>法力消耗: </><ManaCost>%.1f</>\n"
			//Cooldown
			"<Small>冷却： </><Cooldown>%.1f</>\n\n"

			//Number of FireBolts
			"<Default>发射 1 个火球, 技能效果: </>"
			//Damage
			"<Damage>%d</><Default> 火焰伤害，有几率燃烧 </>"),

			//Values
			Level,// 替换 %d 为等级
			ManaCost,
			Cooldown,
			Damage// 替换 %d 为伤害数值
			);
	}
	else// 如果是等级 > 1，显示多发火球描述
	{
		return FString::Printf(TEXT(
			/* Title 标题*/
			"<Title>火球术</>\n\n"

			// Level
			"<Small>等级: </><Level>%d</>\n"
			//ManaCost
			"<Small>法力消耗: </><ManaCost>%.1f</>\n"
			//Cooldown
			"<Small>冷却： </><Cooldown>%.1f</>\n\n"

			//Number of FireBolts
			"<Default>发射 %d 个火球, 技能效果: </>"

			//Damage
			"<Damage>%d</><Default> 火焰伤害，有几率燃烧 </>"),

			//Values
			Level,// 替换 %d 为等级
			ManaCost,
			Cooldown,
			FMath::Min(Level,NumProjectiles),
			Damage// 替换 %d 为伤害数值
			);
	}
}

// 返回技能“下一级”的描述字符串（展示升级后技能效果）
FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	// 根据等级获取火焰伤害数值
	const int32 Damage = GetDamageByDamageType(Level,FAuraGameplayTags::Get().Damage_Fire);// 根据技能等级获取火焰伤害数值
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
			/* Title 标题*/
			"<Title>下一级</>\n\n"

			// Level
			"<Small>等级: </><Level>%d</>\n"
			//ManaCost
			"<Small>法力消耗: </><ManaCost>%.1f</>\n"
			//Cooldown
			"<Small>冷却： </><Cooldown>%.1f</>\n\n"

			//Number of FireBolts
			"<Default>发射 %d 个火球, 技能效果: </>"

			//Damage
			"<Damage>%d</><Default> 火焰伤害，有几率燃烧 </>"),

			//Values
			Level,// 替换 %d 为等级
			ManaCost,
			Cooldown,
			FMath::Min(Level,NumProjectiles),
			Damage// 替换 %d 为伤害数值
			);
	
}
