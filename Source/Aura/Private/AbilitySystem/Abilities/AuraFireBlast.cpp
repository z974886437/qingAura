// 青楼


#include "AbilitySystem/Abilities/AuraFireBlast.h"

FString UAuraFireBlast::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);// 根据技能等级获取火焰伤害数值
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
			/* Title 标题*/
			"<Title>火焰冲击波</>\n\n"

			// Level
			"<Small>等级: </><Level>%d</>\n"
			//ManaCost
			"<Small>法力消耗: </><ManaCost>%.1f</>\n"
			//Cooldown
			"<Small>冷却： </><Cooldown>%.1f</>\n\n"

			//Number of FireBolts
			"<Default>发动 %d </>"
			"<Default>向各个方向发射火球，每个火球都会回来</>"/*fire balls in all directions, each coming back and*/
			"<Default>返回时爆炸，导致 </>"/*exploding upon return, causing */

			
			//Damage
			"<Damage>%d</><Default> 火焰伤害，有几率燃烧 </>"),

			//Values
			Level,// 替换 %d 为等级
			ManaCost,
			Cooldown,
			NumFireBalls,
			ScaledDamage// 替换 %d 为伤害数值
			);
}

FString UAuraFireBlast::GetNextLevelDescription(int32 Level)
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
			"<Default>发动 %d </>"
			"<Default>向各个方向发射火球，每个火球都会回来</>"/*fire balls in all directions, each coming back and*/
			"<Default>返回时爆炸，导致 </>"/*exploding upon return, causing */

			
			//Damage
			"<Damage>%d</><Default> 火焰伤害，有几率燃烧 </>"),

			//Values
			Level,// 替换 %d 为等级
			ManaCost,
			Cooldown,
			NumFireBalls,
			ScaledDamage// 替换 %d 为伤害数值
			);
}
