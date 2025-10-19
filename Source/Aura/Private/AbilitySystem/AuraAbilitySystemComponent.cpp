// 青楼


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Interaction/PlayerInterface.h"


void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&UAuraAbilitySystemComponent::ClientEffectApplied);//监听某个 GameplayEffect 被应用到自己（Self） 时触发的回调
}

// 作用：为角色批量添加预设技能，并给这些技能打上初始标签
// 原因：在角色初始化时需要绑定固定技能（比如普攻、冲刺），并标记它们的状态和输入映射
void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities) // 遍历传入的所有技能类
	{
		// 遍历所有预设的技能类（StartupAbilities 是一个 TArray<TSubclassOf<UGameplayAbility>>）
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,1);// 创建一个技能实例说明（Spec），默认等级为1
		
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability)) // 确认这个技能是自定义的 UAuraGameplayAbility（确保有我们扩展的字段）
		{
			// 将技能的 StartupInputTag 添加到这个技能实例的“动态标签列表”中
			// 动态标签可以在运行时控制技能行为，例如绑定输入、分组判断等
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);// 给技能额外打上一个“装备中”状态标签，方便统一管理
			// 将这个技能赋予角色（但不会自动激活）
			GiveAbility(AbilitySpec);
		}
	}
	// 将这个技能赋予角色，并且只激活一次（比如一次性技能）
	//GiveAbilityAndActivateOnce(AbilitySpec);
	bStartupAbilitiesGiven = true;// 记录“初始化技能已赋予”，避免重复执行
	AbilitiesGivenDelegate.Broadcast();// 广播一个事件，告诉外界“技能已经全部给完”
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{
		// 遍历所有预设的技能类（StartupAbilities 是一个 TArray<TSubclassOf<UGameplayAbility>>）
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;// 如果输入标签无效，直接返回（防御性编程）

	FScopedAbilityListLock ActiveScopeLoc(*this);// 加锁，防止在遍历技能列表时发生并发修改（如添加/移除技能）
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())// 遍历所有可激活的技能（这些技能是通过 GiveAbility 给角色的）
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))// 判断当前技能是否拥有完全匹配的输入标签（用于匹配玩家输入）
		{
			AbilitySpecInputPressed(AbilitySpec);// 标记该技能的“输入已按下”状态（比如用于判断是否持续按住）
			if (AbilitySpec.IsActive())// 如果技能激活
			{
				InvokeReplicatedEvent(
					EAbilityGenericReplicatedEvent::InputPressed,// 表示“输入按下”事件类型
					AbilitySpec.Handle, // 当前技能的唯一句柄，用于标识是哪一个技能
					AbilitySpec.ActivationInfo.GetActivationPredictionKey()// 预测键，用于客户端预测和服务器验证同步
					);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;// 如果输入标签无效，直接返回（防御性编程）

	FScopedAbilityListLock ActiveScopeLoc(*this);// 加锁，防止在遍历技能列表时发生并发修改（如添加/移除技能）
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())// 遍历所有可激活的技能（这些技能是通过 GiveAbility 给角色的）
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))// 判断当前技能是否拥有完全匹配的输入标签（用于匹配玩家输入）
		{
			AbilitySpecInputPressed(AbilitySpec);// 标记该技能的“输入已按下”状态（比如用于判断是否持续按住）
			if (!AbilitySpec.IsActive())// 如果技能还没有激活
			{
				TryActivateAbility(AbilitySpec.Handle);// 尝试激活这个技能（会触发 CanActivateAbility 检查等）
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;// 如果输入标签无效，直接返回（防御性编程）

	FScopedAbilityListLock ActiveScopeLoc(*this);// 加锁，防止在遍历技能列表时发生并发修改（如添加/移除技能）
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())// 遍历所有可激活的技能（这些技能是通过 GiveAbility 给角色的）
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag) && AbilitySpec.IsActive())// 判断当前技能是否拥有完全匹配的输入标签（用于匹配玩家输入）
		{
			AbilitySpecInputReleased(AbilitySpec);// 标记该技能的“输入已被释放”状态（比如用于判断是否持续按住）
			// 通知服务器和客户端同步这个“输入释放”事件
			// 通过网络复制系统触发 EAbilityGenericReplicatedEvent::InputReleased 事件
			// 参数：技能句柄 + 激活预测键（用于客户端预测系统）
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased,AbilitySpec.Handle,AbilitySpec.ActivationInfo.GetActivationPredictionKey());
		}
	}
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);// 🔒 给 AbilitySystemComponent 加一个临时锁，保证遍历 ActivatableAbilities 的过程中不会被并发修改
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())// 遍历 AbilitySystemComponent 中的所有可激活技能（存储在 ActivatableAbilities 容器里）
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))// 如果传进来的委托有效，就用当前 AbilitySpec 调用一次
		{
			UE_LOG(LogAura,Error,TEXT("Failed to execute delegate in %hs"),__FUNCTION__);
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)// 如果这个技能有效
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)// 遍历该技能自带的标签列表（AbilityTags 通常在技能蓝图里配置）
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))// 判断当前标签是否属于 "Abilities" 标签的子类
			{
				return Tag; // 找到后立即返回（第一个匹配的）
			}
		}
	}
	return FGameplayTag(); // 如果没找到，返回一个空的 GameplayTag
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)// 遍历技能实例（AbilitySpec）上动态附加的标签
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag")))) // 判断是否属于 InputTag 分类
		{
			return Tag;// 找到第一个匹配的输入标签就返回
		}
	}
	return FGameplayTag(); // 如果没有找到，返回空标签
}

// 作用：从 AbilitySpec 的标签中找到一个 "Abilities.Status" 相关的状态标签
// 原因：技能在被授予时可能动态附加状态标签，这里需要快速提取对应的状态（如眩晕、中毒）
FGameplayTag UAuraAbilitySystemComponent::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag StatusTag : AbilitySpec.DynamicAbilityTags)// 遍历这个技能实例的动态标签
	{
		if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status")))) // 如果这个标签属于 "Abilities.Status" 范畴（比如 Abilities.Status.Stun）
		{
			return StatusTag;// 找到后立即返回这个具体状态标签
		}
	}
	return FGameplayTag();// 如果没有找到，返回一个空的 GameplayTag
}

// 根据技能标签获取该技能的状态
FGameplayTag UAuraAbilitySystemComponent::GetStatusFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag)) // 尝试通过技能标签获取对应的技能规格（FGameplayAbilitySpec 保存技能实例信息）
	{
		return GetStatusFromSpec(*Spec);// 如果找到了对应的技能规格，则根据规格返回技能状态（比如已解锁、可用、冷却中等）
	}
	return FGameplayTag();// 如果找不到该技能的规格，返回一个空的 FGameplayTag（表示无状态）
}

// 根据技能标签获取该技能的输入标签（比如 Q/E/R 键位）
FGameplayTag UAuraAbilitySystemComponent::GetSlotFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag)) // 尝试通过技能标签获取对应的技能规格（FGameplayAbilitySpec 保存技能实例信息）
	{
		return GetInputTagFromSpec(*Spec);// 如果找到对应的技能规格，则返回它绑定的输入标签（即技能按键）
	}
	return FGameplayTag();// 如果找不到该技能的规格，返回一个空的 FGameplayTag（表示无状态）
}

// 功能：判断指定的技能槽位（Slot）当前是否为空（即没有被任何技能占用）
bool UAuraAbilitySystemComponent::SlotIsEmpty(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLoc(*this);// 加锁，防止在遍历技能列表时发生并发修改（如添加/移除技能）
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())// 遍历当前角色身上所有可激活技能（Activatable Abilities）
	{
		if (AbilityHasSlot(AbilitySpec,Slot))// 如果某个技能占用了这个槽位，则说明槽位不是空的
		{
			return false;
		}
	}
	return true;// 如果遍历完都没找到占用该槽位的技能，则该槽位为空
}

// 功能：判断某个技能（Ability）是否被分配到了指定的“技能槽位（Slot）”
bool UAuraAbilitySystemComponent::AbilityHasSlot(const FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
	// 检查该技能的动态标签（DynamicAbilityTags）中，是否包含指定的槽位标签（Slot）
	// 如果包含，说明该技能当前属于这个槽位
	return Spec.DynamicAbilityTags.HasTagExact(Slot);
}

// 判断一个技能（Ability）是否属于“某个输入槽位”（Slot），比如是否绑定在某个按键上
bool UAuraAbilitySystemComponent::AbilityHasAnySlot(const FGameplayAbilitySpec& Spec)
{
	// 检查该技能的动态标签中是否包含 “InputTag” 标签
	// FGameplayAbilitySpec::DynamicAbilityTags 是运行时可变的标签集合，用于标识技能的当前状态或绑定信息
	// HasTag(...) 用于判断是否包含某个标签
	// RequestGameplayTag(FName("InputTag")) 表示从全局标签表中请求（或创建）一个名为 "InputTag" 的 GameplayTag
	return Spec.DynamicAbilityTags.HasTag(FGameplayTag::RequestGameplayTag(FName("InputTag")));
}

// 功能：根据指定的槽位标签（Slot），找到占用该槽位的技能（AbilitySpec）
// 若该槽位上有技能，则返回其指针；若槽位为空，则返回 nullptr
FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecWithSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);// 加锁，防止在遍历技能列表时被其他线程修改（例如添加或移除技能）
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())// 遍历当前角色拥有的所有可激活技能（Activatable Abilities）
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(Slot))// 判断该技能的动态标签中是否包含这个槽位标签（Slot）
		{
			return &AbilitySpec;// 找到匹配的技能，返回其指针（引用返回会导致悬挂指针风险）
		}
	}
	return nullptr;// 没找到占用该槽位的技能，返回空指针
}

// 功能：判断传入的技能（Spec）是否是被动技能（Passive Ability）
// 返回 true 表示该技能为被动类型，false 表示非被动技能（例如主动释放技能）
bool UAuraAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& Spec) const
{
	// 从 AbilitySystemLibrary 获取当前角色的技能信息表（UAbilityInfo）
	// 这个数据表包含了游戏中所有技能的基础数据（如类型、图标、冷却、描述等）
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	// 根据技能实例（Spec）提取其对应的“技能标签”（AbilityTag）
	// 例如：Abilities.Fireball、Abilities.Passive.HealthRegen 等
	const FGameplayTag AbilityTag = GetAbilityTagFromSpec(Spec);
	const FAuraAbilityInfo& Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);// 在技能信息表中查找该标签对应的技能数据结构（FAuraAbilityInfo）
	const FGameplayTag AbilityType = Info.AbilityType;// 取出技能类型标签（AbilityType），例如 Abilities.Type.Active 或 Abilities.Type.Passive
	return AbilityType.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Type_Passive);// 判断该技能类型是否与“被动技能”标签完全匹配（精确匹配）
}

// 给一个技能分配指定的插槽（Slot 标签）
void UAuraAbilitySystemComponent::AssignSlotToAbility(FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
	ClearSlot(&Spec);// 清空该技能上已有的插槽标签，确保不会重复或冲突
	Spec.DynamicAbilityTags.AddTag(Slot);	// 在技能的动态标签中添加一个新的插槽标签（表示它被分配到了某个输入或位置）
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)// 根据技能标签查找对应的 AbilitySpec，返回指针，如果没找到返回 nullptr
{
	FScopedAbilityListLock ActiveScopeLoc(*this);// 加锁，防止在遍历技能列表时发生并发修改（如添加/移除技能）
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())// 遍历当前角色所有可激活的技能（AbilitySpec 是技能实例化后的数据）
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)// 遍历该技能的标签（AbilityTags 通常标识技能类别/用途）
		{
			if (Tag.MatchesTag(AbilityTag))// 判断技能是否匹配目标标签（MatchesTag 支持层级匹配，比如 Fire.Skill 匹配 Fire）
			{
				return &AbilitySpec;	// 找到就返回该技能的 AbilitySpec 指针
			}
		}
	}
	return nullptr;// 没找到就返回空指针
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)// 升级指定属性：检查是否有可用点数，有的话通知服务端执行升级
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())// 判断角色是否实现了 PlayerInterface（只有玩家才有属性点）
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)// 从接口获取当前可用的属性点数，只有 > 0 才能升级
		{
			ServerUpgradeAttribute(AttributeTag); // 调用服务端 RPC，真正执行属性升级逻辑（防止客户端作弊）
		}
	}
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)// 服务端执行属性升级：触发 GameplayEvent 并消耗 1 点属性点
{
	// 构造事件数据：指定事件标签（要升级的属性）和数值（升级幅度）
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(),AttributeTag,Payload);// 向 AvatarActor 发送 GameplayEvent，触发 GAS 中对应的 Ability 或 Effect

	if (GetAvatarActor()->Implements<UPlayerInterface>())// 如果是玩家角色，就从接口中减去 1 点属性点
	{
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(),-1);
	}
}

// 根据玩家等级更新技能状态：
// 如果等级达标但技能还没获得，就分发给玩家并标记为“可用”
void UAuraAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
	// 从 AbilitySystemLibrary 获取技能配置信息（包含所有技能的需求等级和标签）
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	for (const FAuraAbilityInfo& Info : AbilityInfo->AbilityInformation)// 遍历技能信息表，逐个检查
	{
		if (!Info.AbilityTag.IsValid()) continue;// 如果技能没有标签，跳过（无效数据）
		if (Level < Info.LevelRequirement) continue;// 如果当前等级不足以满足技能需求等级，跳过（玩家等级不够解锁）
		if (GetSpecFromAbilityTag(Info.AbilityTag)  == nullptr)// 如果玩家还没获得这个技能（根据标签在已有技能里查不到）
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability,1);// 创建一个技能实例（FGameplayAbilitySpec：存放技能等级、冷却、状态等）
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);// 给这个技能动态加一个“可用”状态标签（Eligible）
			GiveAbility(AbilitySpec);// 把技能分发给玩家（真正添加到 AbilitySystemComponent）
			MarkAbilitySpecDirty(AbilitySpec);// 标记这个技能的状态已更新（确保同步到客户端/持久化）
			ClientUpdateAbilityStatus(Info.AbilityTag,FAuraGameplayTags::Get().Abilities_Status_Eligible,1);// 通知客户端更新 UI，显示该技能状态为 Eligible
		}
	}
}

// 消耗一个技能点来升级或解锁技能（由服务端执行）
void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))// 根据技能标签获取技能规格（AbilitySpec），如果没找到就返回空
	{
		if (GetAvatarActor()->Implements<UPlayerInterface>())// 如果当前角色实现了玩家接口，就减少一个技能点
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(),-1);
		}
		
		const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();// 获取全局技能标签集合（方便后面做状态对比）
		
		FGameplayTag Status = GetStatusFromSpec(*AbilitySpec);// 获取该技能当前的状态标签（已解锁、已装备、可学习等）
		if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))// 如果技能目前是“可学习”状态，就把它改为“已解锁”
		{
			AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Eligible);// 移除“可学习”标签
			AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Unlocked);// 添加“已解锁”标签
			Status = GameplayTags.Abilities_Status_Unlocked;// 更新状态变量
		}
		// 如果技能已经“解锁”或“已装备”，则直接增加技能等级
		else if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || Status.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
		{
			AbilitySpec->Level += 1;
		}
		ClientUpdateAbilityStatus(AbilityTag,Status,AbilitySpec->Level);// 通知客户端更新技能状态和等级（保证 UI 同步）
		MarkAbilitySpecDirty(*AbilitySpec);// 标记这个技能规格被修改过，让 GAS 知道要同步更新
	}
}

// 服务器端执行的装备技能函数（客户端调用后在服务端执行）
void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag,const FGameplayTag& Slot)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))// 根据技能标签找到对应的技能规格（Spec）
	{
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get(); // 获取全局的 Aura 游戏标签（单例）
		const FGameplayTag& PrevSlot = GetInputTagFromSpec(*AbilitySpec); // 获取该技能之前绑定的槽位
		const FGameplayTag& Status = GetStatusFromSpec(*AbilitySpec);// 获取该技能的当前状态（解锁、装备、锁定等）

		// 判断技能状态是否允许装备（必须是已解锁或已装备）
		const bool bStatusValid = Status == GameplayTags.Abilities_Status_Equipped  || Status == GameplayTags.Abilities_Status_Unlocked;
		if (bStatusValid)// 如果技能状态合法，才能继续执行装备逻辑
		{
			//Handle activation/deactivation for passive abilities 处理被动能力的激活和停用

			if (!SlotIsEmpty(Slot))// There is an ability in this slot already. Deactivate and clear its slot,这个位置已经有一个能力了。停用并清除其插槽
			{
				FGameplayAbilitySpec* SpecWithSlot = GetSpecWithSlot(Slot);// 找出当前插槽对应的技能规格
				if (SpecWithSlot)
				{
					//is that ability the same as this ability? If so,we can return early 那个能力和这个能力一样吗？如果是这样，我们可以早点回来
					if (AbilityTag.MatchesTagExact(GetAbilityTagFromSpec(*SpecWithSlot)))// 如果这个插槽里的技能就是我们要装备的技能，则无需重复操作
					{
						ClientEquipAbility(AbilityTag,GameplayTags.Abilities_Status_Equipped,Slot,PrevSlot);// 通知客户端更新技能 UI 状态（技能已装备，记录新旧槽位）
						return;// 提前结束
					}

					if (IsPassiveAbility(*SpecWithSlot))// 如果旧技能是被动技能，则先停用它（广播停用事件）
					{
						DeactivatePassiveAbility.Broadcast(GetAbilityTagFromSpec(*SpecWithSlot));
					}

					ClearSlot(SpecWithSlot);// 清除旧技能的插槽绑定
				}
			}

			if (!AbilityHasAnySlot(*AbilitySpec))// Ability doesn't yet have a slot (It's not active) 该能力还没有插槽（未激活）
			{
				if (IsPassiveAbility(*AbilitySpec))// 若该技能是被动技能，则尝试立即激活
				{
					TryActivateAbility(AbilitySpec->Handle);//尝试立即激活
				}
			}
			AssignSlotToAbility(*AbilitySpec,Slot);// 正式将该技能分配到新的插槽上（添加动态标签）

			MarkAbilitySpecDirty(*AbilitySpec); // 标记该技能规格数据已修改（触发 GAS 内部同步）
		}
		ClientEquipAbility(AbilityTag,GameplayTags.Abilities_Status_Equipped,Slot,PrevSlot);// 无论是否已装备成功，都通知客户端进行 UI 更新（保证界面一致）
	}
}

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Status,const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	AbilityEquipped.Broadcast(AbilityTag,Status,Slot,PreviousSlot);
}

// 根据技能标签获取技能描述和下一级技能描述
// 返回值：true = 技能已解锁（有 AbilitySpec），false = 技能未解锁（显示锁定描述）
bool UAuraAbilitySystemComponent::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription,FString& OutNextLevelDescription)
{
	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))// 先根据技能标签找到对应的技能规格（AbilitySpec），如果没找到就是没解锁
	{
		if (UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec->Ability))// 如果 Ability 是 UAuraGameplayAbility 类型，就可以调用它的描述函数
		{
			OutDescription = AuraAbility->GetDescription(AbilitySpec->Level);// 获取当前等级的技能描述
			OutNextLevelDescription = AuraAbility->GetNextLevelDescription(AbilitySpec->Level + 1);// 获取下一等级的技能描述（等级+1）
			return true;// 找到了有效技能，返回 true
		}
	}
	// 如果没找到 AbilitySpec，说明技能还没解锁
	// 就去 AbilityInfo 数据表里查找技能的解锁需求
	const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	if (!AbilityTag.IsValid() || AbilityTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_None))
	{
		OutDescription = FString();
	}
	else
	{
		OutDescription = UAuraGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement);// 设置描述为锁定状态（例如：“需要等级5才能解锁”）
	}
	OutNextLevelDescription = FString();// 未解锁时没有下一等级描述
	return false;// 返回 false 表示技能未解锁
}

// 清空指定技能在快捷槽上的绑定（移除输入标签）
void UAuraAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* Spec)
{
	const FGameplayTag Slot = GetInputTagFromSpec(*Spec); // 获取该技能当前绑定的输入槽标签（比如 Q、E、R 等）
	Spec->DynamicAbilityTags.RemoveTag(Slot); // 从技能的动态标签中移除这个输入槽标签
}

// 清空指定槽位上绑定的所有技能
void UAuraAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);// 加锁，保证遍历技能列表时数据不会被并发修改
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())// 遍历角色所有可激活的技能
	{
		if (AbilityHasSlot(&Spec,Slot))// 如果该技能绑定在指定槽位上
		{
			ClearSlot(&Spec);// 清空这个技能的槽位绑定
		}
	}
}

// 判断某个技能是否绑定在指定的快捷槽（输入标签）
bool UAuraAbilitySystemComponent::AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot)
{
	for (FGameplayTag Tag : Spec->DynamicAbilityTags)// 遍历该技能的所有动态标签（可能包含槽位信息）
	{
		if (Tag.MatchesTagExact(Slot))// 如果某个标签和指定槽位完全匹配
		{
			return true; // 找到匹配的槽位，返回 true
		}
	}
	return false; // 遍历完都没找到，说明该技能没绑定这个槽位
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)// 如果初始技能还没被标记为“已赋予”
	{
		bStartupAbilitiesGiven = true;// 标记一下，避免重复广播
		AbilitiesGivenDelegate.Broadcast();// 广播一个委托，告诉外部：初始技能已经同步完成，可以做 UI 或初始化逻辑
	}
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag,const FGameplayTag& StatusTag,int32 AbilityLevel)
{
	AbilityStatusChanged.Broadcast(AbilityTag,StatusTag,AbilityLevel);
}


void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                     const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	//GEngine->AddOnScreenDebugMessage(1, 8.f, FColor::Blue, FString("Effect Applied!"));
	FGameplayTagContainer TagContainer;//用于存储多个标签的结构
	EffectSpec.GetAllAssetTags(TagContainer);//用于从 FGameplayEffectSpec 中获取所有资产标签（Asset Tags），并存储到 FGameplayTagContainer

	EffectAssetTags.Broadcast(TagContainer);// 标签广播机制
}


