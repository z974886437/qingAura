
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

	FGameplayTag Attributes_Meta_IncomingXP;//元_传入XP
	
	FGameplayTag InputTag_LMB;//鼠标左键
	FGameplayTag InputTag_RMB;//鼠标右键
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;
	FGameplayTag InputTag_Passive_1;//鼠标左键
	FGameplayTag InputTag_Passive_2;//鼠标右键

	FGameplayTag Damage;//伤害
	FGameplayTag Damage_Fire;//伤害_火
	FGameplayTag Damage_Lightning;//伤害_闪电
	FGameplayTag Damage_Arcane;//伤害_奥术
	FGameplayTag Damage_Physical;//物理伤害

	FGameplayTag Attributes_Resistance_Fire;//抗性_火
	FGameplayTag Attributes_Resistance_Lightning;//抗性_闪电
	FGameplayTag Attributes_Resistance_Arcane;//抗性_奥术
	FGameplayTag Attributes_Resistance_Physical;//抗性_物理

	FGameplayTag Debuff_Burn;//减益燃烧
	FGameplayTag Debuff_Stun;//减益眩晕
	FGameplayTag Debuff_Arcane;//减益奥术
	FGameplayTag Debuff_Physical;//物理减益

	FGameplayTag Debuff_Chance;//减益更新
	FGameplayTag Debuff_Damage;//减益伤害
	FGameplayTag Debuff_Frequency;//减益频率
	FGameplayTag Debuff_Duration;//减益期间
	
	FGameplayTag Abilities_None;//技能_无

	FGameplayTag Abilities_Attack;//技能_攻击
	FGameplayTag Abilities_Summon;//技能_召唤

	FGameplayTag Abilities_HitReact;//技能_击中反应

	FGameplayTag Abilities_Status_Locked;//技能_状态_锁定
	FGameplayTag Abilities_Status_Eligible;//技能_状态_资格
	FGameplayTag Abilities_Status_Unlocked;//技能_状态_锁定
	FGameplayTag Abilities_Status_Equipped;//技能_状态_装备

	FGameplayTag Abilities_Type_Offensive;//能力_类型_攻击
	FGameplayTag Abilities_Type_Passive;//能力_类型_被动
	FGameplayTag Abilities_Type_None;//能力_类型_无
	
	FGameplayTag Abilities_Fire_FireBolt;//技能_火_火弩箭
	FGameplayTag Abilities_Lightning_Electrocute;//技能_闪电_触电

	FGameplayTag Abilities_Passive_HaloOfProtection;//能力_被动_保护光环
	FGameplayTag Abilities_Passive_LifeSiphon;//能力_被动_生命虹吸
	FGameplayTag Abilities_Passive_ManaSiphon;//能力_被动_法力虹吸

	FGameplayTag Cooldown_Fire_FireBolt;//冷却_火_火弩箭
	
	FGameplayTag CombatSocket_Weapon;//战斗插座_武器
	FGameplayTag CombatSocket_RightHand;//战斗插座_右手
	FGameplayTag CombatSocket_LeftHand;//战斗插座_左手
	FGameplayTag CombatSocket_Tail;//战斗插座_尾部

	FGameplayTag Montage_Attack_1;//蒙太奇_攻击1
	FGameplayTag Montage_Attack_2;//蒙太奇_攻击2
	FGameplayTag Montage_Attack_3;//蒙太奇_攻击3
	FGameplayTag Montage_Attack_4;//蒙太奇_攻击4

	TMap<FGameplayTag,FGameplayTag> DamageTypesToResistance;//抵抗伤害类型
	TMap<FGameplayTag,FGameplayTag> DamageTypesToDebuff;//减益伤害类型

	FGameplayTag Effects_HitReact;//击中反应

	FGameplayTag Player_Block_InputPressed;//玩家_阻止_输入按下
	FGameplayTag Player_Block_InputHeld;//玩家_阻止_输入保持
	FGameplayTag Player_Block_InputReleased;//玩家_阻止_输入释放
	FGameplayTag Player_Block_CursorTrace;//玩家_阻止_光标跟踪
	
private:
	static FAuraGameplayTags GameplayTags;//游戏标签

	
};
