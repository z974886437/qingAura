// 青楼


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Aura/AuraLogChannels.h"
#include "Interaction/PlayerInterface.h"


void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&UAuraAbilitySystemComponent::ClientEffectApplied);//监听某个 GameplayEffect 被应用到自己（Self） 时触发的回调

	// FAuraGameplayTags::Get().Attributes_Primary_Intelligence;
	// const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	// GEngine->AddOnScreenDebugMessage(
	// 	-1,
	// 	10.f,
	// 	FColor::Orange,
	// 	FString::Printf(TEXT("Tag:%s"),*GameplayTags.Attributes_Secondary_Armor.ToString())
	// 	);
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

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;// 如果输入标签无效，直接返回（防御性编程）

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

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())// 遍历所有可激活的技能（这些技能是通过 GiveAbility 给角色的）
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))// 判断当前技能是否拥有完全匹配的输入标签（用于匹配玩家输入）
		{
			AbilitySpecInputReleased(AbilitySpec);// 标记该技能的“输入已被释放”状态（比如用于判断是否持续按住）
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

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)// 如果初始技能还没被标记为“已赋予”
	{
		bStartupAbilitiesGiven = true;// 标记一下，避免重复广播
		AbilitiesGivenDelegate.Broadcast();// 广播一个委托，告诉外部：初始技能已经同步完成，可以做 UI 或初始化逻辑
	}
}


void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                     const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	//GEngine->AddOnScreenDebugMessage(1, 8.f, FColor::Blue, FString("Effect Applied!"));
	FGameplayTagContainer TagContainer;//用于存储多个标签的结构
	EffectSpec.GetAllAssetTags(TagContainer);//用于从 FGameplayEffectSpec 中获取所有资产标签（Asset Tags），并存储到 FGameplayTagContainer

	EffectAssetTags.Broadcast(TagContainer);// 标签广播机制
}


