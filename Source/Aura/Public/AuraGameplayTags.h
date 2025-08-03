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

	
protected:
	
private:
	static FAuraGameplayTags GameplayTags;//游戏标签

	
};
