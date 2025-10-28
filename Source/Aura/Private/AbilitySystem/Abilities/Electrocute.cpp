// 青楼


#include "AbilitySystem/Abilities/Electrocute.h"

FString UElectrocute::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);// 根据技能等级获取伤害数值
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)// 如果是等级 1，显示单发火球描述
	{
		return FString::Printf(TEXT(
			/* Title 标题*/
			"<Title>闪电链</>\n\n"

			// Level
			"<Small>等级: </><Level>%d</>\n"
			//ManaCost
			"<Small>法力消耗: </><ManaCost>%.1f</>\n"
			//Cooldown
			"<Small>冷却： </><Cooldown>%.1f</>\n\n"

			//Number of FireBolts
			"<Default>发射 1 个闪电光束, 技能效果: </>"
			//Damage
			"<Damage>%d</><Default> 闪电伤害，有几率眩晕 </>"),

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
			"<Title>闪电链</>\n\n"

			// Level
			"<Small>等级: </><Level>%d</>\n"
			//ManaCost
			"<Small>法力消耗: </><ManaCost>%.1f</>\n"
			//Cooldown
			"<Small>冷却： </><Cooldown>%.1f</>\n\n"

			//Number of FireBolts
			"<Default>发射 %d 个闪电光束, 技能效果: </>"

			//Damage
			"<Damage>%d</><Default> 闪电伤害，有几率眩晕 </>"),

			//Values
			Level,// 替换 %d 为等级
			ManaCost,
			Cooldown,
			FMath::Min(Level,MaxNumShockTargets - 1),
			ScaledDamage// 替换 %d 为伤害数值
			);
	}
}

FString UElectrocute::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);// 根据技能等级获取火焰伤害数值
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
			"<Default>发射 %d 个闪电光束, 技能效果: </>"

			//Damage
			"<Damage>%d</><Default> 闪电伤害，有几率眩晕 </>"),

			//Values
			Level,// 替换 %d 为等级
			ManaCost,
			Cooldown,
			FMath::Min(Level,MaxNumShockTargets),
			ScaledDamage// 替换 %d 为伤害数值
			);
}
