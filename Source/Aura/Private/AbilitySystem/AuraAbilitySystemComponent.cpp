// 青楼


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"


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

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		// 遍历所有预设的技能类（StartupAbilities 是一个 TArray<TSubclassOf<UGameplayAbility>>）
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,1);
		
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			// 将技能的 StartupInputTag 添加到这个技能实例的“动态标签列表”中
			// 动态标签可以在运行时控制技能行为，例如绑定输入、分组判断等
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			// 将这个技能赋予角色（但不会自动激活）
			GiveAbility(AbilitySpec);
		}
		// 将这个技能赋予角色，并且只激活一次（比如一次性技能）
		//GiveAbilityAndActivateOnce(AbilitySpec);
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

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	//GEngine->AddOnScreenDebugMessage(1, 8.f, FColor::Blue, FString("Effect Applied!"));
	FGameplayTagContainer TagContainer;//用于存储多个标签的结构
	EffectSpec.GetAllAssetTags(TagContainer);//用于从 FGameplayEffectSpec 中获取所有资产标签（Asset Tags），并存储到 FGameplayTagContainer

	EffectAssetTags.Broadcast(TagContainer);// 标签广播机制
}


