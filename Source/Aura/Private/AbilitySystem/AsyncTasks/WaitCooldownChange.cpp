// 青楼


#include "AbilitySystem/AsyncTasks/WaitCooldownChange.h"
#include "AbilitySystemComponent.h"

UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent,const FGameplayTag& InCooldownTag)
{
	// 1. 创建异步任务对象（NewObject不会立刻被GC回收）
	UWaitCooldownChange* WaitCooldownChange = NewObject<UWaitCooldownChange>();// 创建一个新的 UWaitCooldownChange 对象（受 UE GC 管理，不会立刻销毁）
	WaitCooldownChange->ASC = AbilitySystemComponent;// 保存传入的 AbilitySystemComponent，后面要在它身上注册冷却监听
	WaitCooldownChange->CooldownTag = InCooldownTag;// 保存要监听的冷却标签（比如 "Cooldown.Fireball"）

	// 2. 检查参数有效性，如果没传有效组件/标签 → 立即结束任务并返回 nullptr
	if (!IsValid(AbilitySystemComponent) || !InCooldownTag.IsValid())
	{
		WaitCooldownChange->EndTask(); // 如果组件或标签无效 → 结束任务，释放内存
		return nullptr;// 返回空指针 → 蓝图/调用方拿到 nullptr，知道任务没成功创建
	}

	// To know when a cooldown has ended(Cooldown Tag has been removed)要知道冷却时间何时结束（冷却时间标签已被移除）
	// 3. 在 AbilitySystemComponent 上注册监听器
	//    当指定 CooldownTag 被“添加或移除”时，会调用 CooldownTagChanged()
	AbilitySystemComponent->RegisterGameplayTagEvent(
		InCooldownTag,// 要监听的 GameplayTag（例如 Cooldown.Fireball）
		EGameplayTagEventType::NewOrRemoved// 事件类型：当 Tag 新增或移除时触发
		).AddUObject(
			WaitCooldownChange,// 回调绑定到哪个对象
			&UWaitCooldownChange::CooldownTagChanged// 回调函数指针
			);

	//To know when a cooldown effect has been applied 知道何时应用了冷却效果
	// 给 ASC 绑定委托，当角色自己获得新的 GameplayEffect 时触发
	// 这里绑定到 UWaitCooldownChange::OnActiveEffectAdded，用于检测冷却效果是否被应用
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCooldownChange,&UWaitCooldownChange::OnActiveEffectAdded);
	
	return WaitCooldownChange;// 4. 返回这个任务对象，蓝图就可以拿到并绑定事件
}

void UWaitCooldownChange::EndTask()
{
	if (!IsValid(ASC)) return;// 如果 AbilitySystemComponent 无效，直接返回，避免空指针崩溃
	ASC->RegisterGameplayTagEvent(CooldownTag,EGameplayTagEventType::NewOrRemoved).RemoveAll(this);// 解绑之前注册的标签事件，防止重复回调或内存泄漏

	SetReadyToDestroy();// 标记异步任务对象可以安全销毁
	MarkAsGarbage();// 告诉 GC 回收对象，彻底释放资源
}

// 当监听的冷却标签计数变化时调用，判断冷却是否结束
// 如果 NewCount 为 0，说明冷却结束，触发蓝图事件通知外部
void UWaitCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
	if (NewCount == 0)// 冷却结束判断
	{
		CooldownEnd.Broadcast(0.f);// 广播事件给蓝图，通知冷却结束
	}
}

// 当角色自己获得新的 GameplayEffect 时调用，用于检测是否应用了指定冷却标签
void UWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC,const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	// 获取 GameplayEffect 自身的所有 Asset 标签
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);

	// 获取 GameplayEffect 授予的所有标签
	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags);

	if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))// 如果 Effect 自身或授予的标签包含我们关心的 CooldownTag
	{
		// 构建查询，匹配所有拥有该冷却标签的活跃效果
		FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
		TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery); // 获取所有匹配效果的剩余时间
		if (TimesRemaining.Num() > 0)// 如果存在至少一个匹配效果
		{
			float TimeRemaining = TimesRemaining[0]; // 默认取第一个剩余时间
			// 找到最长剩余时间
			for (int32 i = 0 ; i < TimesRemaining.Num();i++)// 遍历所有匹配效果的剩余时间，找出最长的冷却时间
			{
				if (TimesRemaining[i] > TimeRemaining)// 如果当前剩余时间比已记录的最长时间大，则更新最长时间	
				{
					TimeRemaining = TimesRemaining[i];
				}
			}
			
			CooldownStart.Broadcast(TimeRemaining); // 广播事件给蓝图，通知冷却开始及剩余时间
		}
	}
}
