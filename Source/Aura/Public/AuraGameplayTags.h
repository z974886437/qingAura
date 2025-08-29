// 青楼

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 *  AruaGameplayTags
 *
 *  Singleton containing native Gameplay Tags  包含原生游戏标签的包含原生游戏标签的
 */

struct FAuraGameplayTags
{
public:

	static const FAuraGameplayTags& Get() {return GameplayTags;}
	static void InitializeNativeGameplayTags();//初始化本机游戏标签

	FGameplayTag Attributes_Primary_Strength;//力量
	FGameplayTag Attributes_Primary_Intelligence;//智力
	FGameplayTag Attributes_Primary_Resilience;//韧性
	FGameplayTag Attributes_Primary_Vigor;//活力
	
	FGameplayTag Attributes_Secondary_Armor;//盔甲
	FGameplayTag Attributes_Secondary_ArmorPenetration;//护甲穿透
	FGameplayTag Attributes_Secondary_BlockChance;//格挡概率
	FGameplayTag Attributes_Secondary_CriticalHitChance;//暴击概率
	FGameplayTag Attributes_Secondary_CriticalHitDamage;//暴击伤害
	FGameplayTag Attributes_Secondary_CriticalHitResistance;//暴击抗性
	FGameplayTag Attributes_Secondary_HealthRegeneration;//生命恢复
	FGameplayTag Attributes_Secondary_ManaRegeneration;//法力恢复
	FGameplayTag Attributes_Secondary_MaxHealth;//最大生命值
	FGameplayTag Attributes_Secondary_MaxMana;//最大法力值

	FGameplayTag Attributes_Resistance_Fire;//抗性_火
	FGameplayTag Attributes_Resistance_Lightning;//抗性_闪电
	FGameplayTag Attributes_Resistance_Arcane;//抗性_奥术
	FGameplayTag Attributes_Resistance_Physical;//抗性_物理

	
	FGameplayTag InputTag_LMB;//鼠标左键
	FGameplayTag InputTag_RMB;//鼠标右键
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;

	FGameplayTag Damage;//伤害
	FGameplayTag Damage_Fire;//伤害_火
	FGameplayTag Damage_Lightning;//伤害_闪电
	FGameplayTag Damage_Arcane;//伤害_奥术
	FGameplayTag Damage_Physical;//物理伤害

	FGameplayTag Abilities_Attack;//技能_攻击

	FGameplayTag Montage_Attack_Weapon;//蒙太奇_攻击_武器
	FGameplayTag Montage_Attack_RightHand;//蒙太奇_攻击_右手
	FGameplayTag Montage_Attack_LeftHand;//蒙太奇_攻击_左手

	TMap<FGameplayTag,FGameplayTag> DamageTypesToResistance;//抵抗伤害类型

	FGameplayTag Effects_HitReact;//击中反应
private:
	static FAuraGameplayTags GameplayTags;//游戏标签

	
};
