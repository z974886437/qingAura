// 青楼


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	SpellPointsChanged.Broadcast(GetAuraPS()->GetSpellPoints());
	
}

// 绑定回调函数到依赖（这里是绑定技能状态变化的回调）
// 当技能状态发生变化时，通知 UI 更新
void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	// 监听 AuraAbilitySystemComponent 的技能状态变化事件
	GetAuraASC()->AbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTag,const FGameplayTag& StatusTag,int32 NewLevel)
	{
		if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))
		{
			SelectedAbility.Status = StatusTag;// 设置当前选中的技能信息
			bool bEnableSpendPoints = false; // 是否启用“消耗技能点”按钮
			bool bEnableEquip = false; // 是否启用“装备技能”按钮
			ShouldEnableButtons(StatusTag,CurrentSpellPoints,bEnableSpendPoints,bEnableEquip);// 根据状态和点数计算按钮可用性
			// 定义两个字符串变量，用来接收技能描述和下一级技能描述
			FString Description;
			FString NextLevelDescription;
			GetAuraASC()->GetDescriptionsByAbilityTag(AbilityTag,Description,NextLevelDescription);// 调用 ASC 的函数，根据技能标签获取描述（已解锁就返回真实描述，未解锁返回锁定提示）
			SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints,bEnableEquip,Description,NextLevelDescription);// 广播事件，通知 UI 更新按钮可用性
		}
		
		if (AbilityInfo)// 如果技能信息表存在
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);// 根据技能标签查找对应的技能信息
			Info.StatusTag = StatusTag;// 更新技能的当前状态标签（比如 Locked / Eligible / Unlocked）
			AbilityInfoDelegate.Broadcast(Info);// 广播给 UI（比如技能树、技能菜单），让界面显示最新状态
		}
	});

	GetAuraASC()->AbilityEquipped.AddUObject(this,&USpellMenuWidgetController::OnAbilityEquipped);

	GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda([this](int32 SpellPoints)
	{
		SpellPointsChanged.Broadcast(SpellPoints);// 广播事件 → 通知 UI 显示最新的点数
		CurrentSpellPoints = SpellPoints;// 缓存当前技能点数

		bool bEnableSpendPoints = false; // 是否启用“消耗技能点”按钮
		bool bEnableEquip = false; // 是否启用“装备技能”按钮
		ShouldEnableButtons(SelectedAbility.Status,CurrentSpellPoints,bEnableSpendPoints,bEnableEquip);// 根据状态和点数计算按钮可用性
		// 定义两个字符串变量，用来接收技能描述和下一级技能描述
		FString Description;
		FString NextLevelDescription;
		GetAuraASC()->GetDescriptionsByAbilityTag(SelectedAbility.Ability,Description,NextLevelDescription);// 调用 ASC 的函数，根据技能标签获取描述（已解锁就返回真实描述，未解锁返回锁定提示）
		SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints,bEnableEquip,Description,NextLevelDescription);// 广播事件，通知 UI 更新按钮可用性
	});
}

// 当某个技能球（SpellGlobe）被选中时调用
// 根据技能标签判断其状态，并决定“消耗点数按钮”和“装备按钮”是否启用
void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	if (bWaitingForEquipSelection)
	{
		const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType;// 获取当前选中技能对应的类型（例如攻击、防御等）
		StopWaitingForEquipDelegate.Broadcast(SelectedAbilityType); // 广播取消等待装备的事件，传递被取消技能的类型
		bWaitingForEquipSelection = false;
	}
	
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();// 获取全局技能标签单例
	const int32 SpellPoints = GetAuraPS()->GetSpellPoints();// 获取当前可用技能点
	FGameplayTag AbilityStatus; // 用于存储该技能的状态（Locked/Unlocked/Equipped/Eligible）

	// 检查技能标签和技能规格
	const bool bTagValid = AbilityTag.IsValid();// 标签是否有效
	const bool bTagNone = AbilityTag.MatchesTag(GameplayTags.Abilities_None);// 是否是“无技能”占位符
	const FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);// 根据标签找技能规格
	const bool bSpecValid = AbilitySpec != nullptr;  // 判断是否成功获取到技能规格
	
	if (!bTagValid || bTagNone || !bSpecValid) // 如果无效 → 设置为“锁定状态”；否则从 AbilitySpec 获取真实状态
	{
		AbilityStatus = GameplayTags.Abilities_Status_Locked; // 状态强制设为“锁定”
	}
	else
	{
		AbilityStatus = GetAuraASC()->GetStatusFromSpec(*AbilitySpec);// 否则从技能规格里获取该技能的真实状态
	}
	
	// 设置当前选中的技能信息
	SelectedAbility.Ability = AbilityTag;
	SelectedAbility.Status = AbilityStatus;
	// 调用逻辑函数，决定按钮是否启用
	bool bEnableSpendPoints = false; // 是否启用“消耗技能点”按钮
	bool bEnableEquip = false; // 是否启用“装备技能”按钮
	ShouldEnableButtons(AbilityStatus,SpellPoints,bEnableSpendPoints,bEnableEquip);// 根据状态和点数计算按钮可用性
	// 定义两个字符串变量，用来接收技能描述和下一级技能描述
	FString Description;
	FString NextLevelDescription;
	GetAuraASC()->GetDescriptionsByAbilityTag(AbilityTag,Description,NextLevelDescription);// 调用 ASC 的函数，根据技能标签获取描述（已解锁就返回真实描述，未解锁返回锁定提示）
	SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints,bEnableEquip,Description,NextLevelDescription);// 广播事件，通知 UI 更新按钮可用性
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	if (GetAuraASC())
	{
		GetAuraASC()->ServerSpendSpellPoint(SelectedAbility.Ability);
	}
}

// 技能菜单控件控制器中取消选中技能的逻辑
void USpellMenuWidgetController::GlobeDeselect()
{
	if (bWaitingForEquipSelection)
	{
		const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;// 获取当前选中技能对应的类型（例如攻击、防御等）
		StopWaitingForEquipDelegate.Broadcast(SelectedAbilityType); // 广播取消等待装备的事件，传递被取消技能的类型
		bWaitingForEquipSelection = false;
	}
	
	SelectedAbility.Ability = FAuraGameplayTags::Get().Abilities_None;// 将当前选中技能设置为“无技能”状态
	SelectedAbility.Status = FAuraGameplayTags::Get().Abilities_Status_Locked;// 将当前选中技能状态重置为“锁定”状态（不可用）

	// 广播技能球取消选中事件，传递空字符串，用于通知 UI 更新按钮状态（禁用升级和装备按钮）
	SpellGlobeSelectedDelegate.Broadcast(false,false,FString(),FString());
}

// 当玩家点击“装备”按钮时执行的逻辑
void USpellMenuWidgetController::EquipButtonPressed()
{
	// 根据当前选中技能的标签，找到它对应的技能类型（例如攻击、防御、辅助）
	const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;

	WaitForEquipDelegate.Broadcast(AbilityType);// 广播“等待装备”事件，通知 UI 或逻辑层进入技能装备流程
	bWaitingForEquipSelection = true;// 标记当前状态为“正在等待玩家选择装备槽位”

	const FGameplayTag SelectedStatus = GetAuraASC()->GetStatusFromAbilityTag(SelectedAbility.Ability);// 获取当前选中技能的状态（比如已解锁、已装备、冷却中等）
	if (SelectedStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))// 如果技能状态是“已装备”
	{
		SelectedSlot = GetAuraASC()->GetInputTagFromAbilityTag(SelectedAbility.Ability);// 获取该技能已经装备的输入标签（比如 Q/E/R 键），并保存到 SelectedSlot
	}
}

// 当玩家点击某个法术槽时触发，决定是否能把当前选中的技能装备到该槽
void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
	if (!bWaitingForEquipSelection) return; // 如果当前没有在等待选择技能（比如没点击“装备”按钮），直接返回
	//Check Selected ability against the Slot's Ability type.根据插槽的技能类型检查 选定技能
	//(don't equip on offensive spell in a passive slot and vice versa) 不要在被动槽中装备进攻法术，反之亦然
	const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType; // 获取当前选中技能的类型（例如：攻击型、被动型等）

	// 如果槽位的类型和选中技能的类型不匹配，则返回（阻止错误装备）
	// 举例：被动技能槽不能放攻击法术，攻击槽也不能放被动技能
	if (!SelectedAbilityType.MatchesTagExact(AbilityType)) return;

	GetAuraASC()->ServerEquipAbility(SelectedAbility.Ability,SlotTag);
}

// 当技能成功装备到槽位时，更新 UI 并广播相关信息
void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status,const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	bWaitingForEquipSelection = false;// 装备流程结束，取消等待状态

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

	StopWaitingForEquipDelegate.Broadcast(AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType);// 广播“停止等待装备”的事件，通知 UI 停止高亮/提示
	SpellGlobeReassignedDelegate.Broadcast(AbilityTag);
	GlobeDeselect();// 技能菜单控件控制器中取消选中技能的逻辑
}

// 判断技能按钮是否应该启用（消耗技能点按钮 & 装备按钮）
// 参数：AbilityStatus 表示技能当前状态，SpellPoints 表示可用技能点数量
// 返回：通过引用参数设置两个 bool 值
void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints,bool& bShouldEnableSpellPointsButton, bool& bShouldEnableEquipButton)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();// 获取全局定义的技能标签单例

	// 默认禁用两个按钮
	bShouldEnableSpellPointsButton = false; // 默认关闭“消耗技能点按钮”
	bShouldEnableEquipButton = false;// 默认关闭“装备按钮”
	if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped)) // 情况 1：如果技能状态是“已装备”
	{
		bShouldEnableEquipButton = true;   // 已装备的技能，装备按钮保持可用
		if (SpellPoints > 0) // 如果玩家还有技能点
		{
			bShouldEnableSpellPointsButton = true;// 点数按钮也可用（可以升级）
		}
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))// 情况 2：如果技能状态是“符合条件但未装备”
	{
		bShouldEnableEquipButton = false; // 符合条件但未装备 → 装备按钮暂不可用
		if (SpellPoints > 0)// 如果玩家还有技能点
		{
			bShouldEnableSpellPointsButton = true;// 点数按钮可用（可以解锁/升级）
		}
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))// 情况 3：如果技能状态是“已解锁（可装备）”
	{
		bShouldEnableEquipButton = true;// 已解锁技能 → 装备按钮可用
		if (SpellPoints > 0)// 如果玩家还有技能点
		{
			bShouldEnableSpellPointsButton = true;// 点数按钮也可用（继续升级）
		}
	}
}
