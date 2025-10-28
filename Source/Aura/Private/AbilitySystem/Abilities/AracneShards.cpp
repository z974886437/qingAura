// 青楼


#include "AbilitySystem/Abilities/AracneShards.h"

FString UAracneShards::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);// 根据技能等级获取伤害数值
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)// 如果是等级 1，显示单发火球描述
	{
		return FString::Printf(TEXT(
			/* Title 标题*/
			"<Title>奥术碎片</>\n\n"

			// Level
			"<Small>等级: </><Level>%d</>\n"
			//ManaCost
			"<Small>法力消耗: </><ManaCost>%.1f</>\n"
			//Cooldown
			"<Small>冷却： </><Cooldown>%.1f</>\n\n"

			//Number of FireBolts
			"<Default>召唤 1 个奥术能量, 技能效果: </>"
			//Damage
			"<Damage>%d</><Default> 奥术伤害，击飞敌人 </>"),

			//Values
			Level,// 替换 %d 为等级
			ManaCost,
			Cooldown,
			ScaledDamage// 替换 %d 为伤害数值
			);
	}
	else// 如果是等级 > 1，显示多发火球描述
	{
		return FString::Printf(TEXT(
			/* Title 标题*/
			"<Title>奥术碎片</>\n\n"

			// Level
			"<Small>等级: </><Level>%d</>\n"
			//ManaCost
			"<Small>法力消耗: </><ManaCost>%.1f</>\n"
			//Cooldown
			"<Small>冷却： </><Cooldown>%.1f</>\n\n"

			//Number of FireBolts
			"<Default>召唤 %d 个奥术能量, 技能效果: </>"

			//Damage
			"<Damage>%d</><Default> 奥术伤害，击飞敌人 </>"),

			//Values
			Level,// 替换 %d 为等级
			ManaCost,
			Cooldown,
			FMath::Min(Level,MaxNumShards),
			ScaledDamage// 替换 %d 为伤害数值
			);
	}
}

FString UAracneShards::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);// 根据技能等级获取伤害数值
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
			"<Default>召唤 %d 个奥术能量, 技能效果: </>"

			//Damage
			"<Damage>%d</><Default> 奥术伤害，击飞敌人 </>"),

			//Values
			Level,// 替换 %d 为等级
			ManaCost,
			Cooldown,
			FMath::Min(Level,MaxNumShards),
			ScaledDamage// 替换 %d 为伤害数值
			);
}
