// 青楼


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
}

// 绑定回调函数到依赖（这里是绑定技能状态变化的回调）
// 当技能状态发生变化时，通知 UI 更新
void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	// 监听 AuraAbilitySystemComponent 的技能状态变化事件
	GetAuraASC()->AbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTag,const FGameplayTag& StatusTag)
	{
		if (AbilityInfo)// 如果技能信息表存在
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);// 根据技能标签查找对应的技能信息
			Info.StatusTag = StatusTag;// 更新技能的当前状态标签（比如 Locked / Eligible / Unlocked）
			AbilityInfoDelegate.Broadcast(Info);// 广播给 UI（比如技能树、技能菜单），让界面显示最新状态
		}
	});
}
