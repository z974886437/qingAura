// 青楼


#include "AbilitySystem/Abilities/AuraPassiveAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UAuraPassiveAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	// 调用父类的 ActivateAbility，保持 GameplayAbility 的标准初始化流程
	// 这样父类中注册的触发条件、冷却处理等逻辑依然会执行
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 从 Avatar（通常是角色）身上获取 AbilitySystemComponent，并尝试转换为自定义的 UAuraAbilitySystemComponent
	// 这样就能访问 Aura 系统扩展的功能（例如自定义事件或委托）
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo())))
	{
		// 把本被动技能（this）的 ReceiveDeactivate 函数绑定到 AuraASC 的 “DeactivatePassiveAbility” 委托上
		// 意思是：当某处触发该委托（比如系统要求关闭某个被动技能）时，会自动调用 ReceiveDeactivate
		AuraASC->DeactivatePassiveAbility.AddUObject(this,&UAuraPassiveAbility::ReceiveDeactivate);
	}
}

void UAuraPassiveAbility::ReceiveDeactivate(const FGameplayTag& AbilityTag)
{
	// 检查传入的 AbilityTag 是否与当前技能的标签完全匹配
	// 用 HasTagExact 是为了防止“模糊匹配”，确保只关闭指定的被动技能
	if (AbilityTags.HasTagExact(AbilityTag))
	{
		// 如果匹配，则立即结束该技能
		// true,true → 表示“正常结束且强制立即中止”，不会等待蒙太奇或特效自然完成
		EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,true);
	}
}
