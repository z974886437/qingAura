// 青楼


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());//广播一个多播委托事件，并传递当前角色的生命值（Health）给所有监听者（比如 UI 血条）
	OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAuraAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	GetAuraPS()->OnXPChangedDelegate.AddUObject(this,&UOverlayWidgetController::OnXPChanged);// 把当前控制器的 OnXPChanged 绑定到经验值变化事件 → XP 一更新就能自动调用 UI 更新函数
	GetAuraPS()->OnLevelChangedDelegate.AddLambda(
		[this](int32 NewLevel)
		{
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel);	
		}
	);

	//将一个函数（HealthChanged）绑定到 GAS（Gameplay Ability System）中的属性变更事件委托上，属于 GAS 属性监听的标准写法
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);//当 GAS 系统检测到 Health 变化时，调用这个函数；
			}
		);
	
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);//当 GAS 系统检测到 MaxHealth 变化时，调用这个函数；
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnManaChanged.Broadcast(Data.NewValue);//当 GAS 系统检测到 mana 变化时，调用这个函数；
			}
		);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxManaChanged.Broadcast(Data.NewValue);//当 GAS 系统检测到 MaxMana 变化时，调用这个函数；
			}
		);

	if (GetAuraASC())//  检查并初始化 Aura ASC
	{
		GetAuraASC()->AbilityEquipped.AddUObject(this,&UOverlayWidgetController::OnAbilityEquipped);
		if (GetAuraASC()->bStartupAbilitiesGiven)// 如果 ASC 已经初始化过起始技能，则直接调用初始化逻辑
		{
			BroadcastAbilityInfo();
		}
		else
		{
			GetAuraASC()->AbilitiesGivenDelegate.AddUObject(this,&UOverlayWidgetController::BroadcastAbilityInfo);// 否则注册一个回调，等它初始化完成后再调用
		}
		
		//在注册一个对 Gameplay Tag 事件的监听函数（Lambda）
		GetAuraASC()->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				for (const FGameplayTag& Tag : AssetTags)
				{
					// For example, soy that Tag =  Message.HealthPotion 例如，大豆的标签（Tag）为“健康药水（HealthPotion）
					//* "Message.HealthPotion".MatchesTag("Message") will return True, "Message".MatchesTag("Message.HealthPotion") will return False
					//"Message.HealthPotion"。如果与标签"Message"匹配，将返回True；如果与标签"Message.HealthPotion"匹配，将返回False
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if (Tag.MatchesTag(MessageTag))
					{
						//Broadcast the tag to the Widget Controller 将标签广播至小部件控制器
						//将资产标签中的每个标签添加到屏幕上 添加调试消息
						// const FString Msg = FString::Printf(TEXT("GE Tag: %s"), *Tag.ToString());
						// GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue,Msg);
						const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable,Tag);// 根据标签从数据表里找到对应的 UI 数据行
						MessageWidgetRowDelegate.Broadcast(*Row);//消息部件委托进行广播
						
					}
				}
			}
		);
	}

}

// 当玩家经验值发生变化时触发 → 计算当前等级进度百分比并通知 UI 更新
void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	const ULevelUpInfo* LevelUpInfo = GetAuraPS()->LevelUpInfo;// 获取等级信息表 → 必须在蓝图中配置好，否则直接报错
	checkf(LevelUpInfo,TEXT("Unabled to find LevelUpInfo. Please fill out AuraPlayerState Blueprint"));

	const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);// 根据当前 XP 查找对应等级
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();// 获取最高等级，用来防止越界

	if (Level <= MaxLevel && Level > 0)// 确保等级合法（1 到 MaxLevel）
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;// 当前等级升级所需总 XP
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level - 1].LevelUpRequirement; // 上一级升级所需总 XP（比如等级 5 升级需要 500，总 XP 是 500；等级 4 升级需要 300，总 XP 是 300）

		const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;// 当前等级区间所需 XP（比如等级 4 → 5 需要 200）
		const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;// 玩家在当前等级区间已经获得的 XP（比如当前 XP=350，那就是 350-300=50）

		const float XPBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelRequirement); // 计算经验条百分比（0~1）

		OnXPPercentChangeDelegate.Broadcast(XPBarPercent);  // 通知 UI 更新经验条进度
	}
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status,const FGameplayTag& Slot, const FGameplayTag& PreviousSlot) const
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();// 获取全局的 Aura 游戏标签（单例）

	FAuraAbilityInfo LastSlotInfo;// 构造一个“上一个槽位”的占位信息
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;// 标记为“已解锁”状态（即该槽位现在空了）
	LastSlotInfo.InputTag = PreviousSlot;// 设置输入槽为之前的槽位
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;// 清空技能标签（代表没有技能）
	// Broadcast empty info if PreviousSlot is a valid slot. Only if equipping on already-Equipped spell如果 PreviousSlot 是有效插槽，则广播空信息。仅当装备已经装备的法术时
	AbilityInfoDelegate.Broadcast(LastSlotInfo); // 如果 PreviousSlot 有效，则广播空信息，让 UI 清掉旧槽的技能

	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag); // 查找新装备的技能信息
	Info.StatusTag = Status;// 更新状态为最新（已装备）
	Info.InputTag = Slot;// 更新绑定的槽位
	AbilityInfoDelegate.Broadcast(Info);// 广播新的技能信息，让 UI 显示新技能
	
}
