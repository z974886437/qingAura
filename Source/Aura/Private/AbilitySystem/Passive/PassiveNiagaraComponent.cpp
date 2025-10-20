// 青楼


#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	bAutoActivate = false;
}

// 当游戏开始或组件启用时自动调用
void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();// 调用父类 BeginPlay，确保父类初始化逻辑被执行

	// 尝试直接从本组件的拥有者（Owner）获取 Aura 能力系统组件（ASC）
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		// 如果成功获取到 AuraASC，则把本组件的 OnPassiveActivate 函数绑定到它的 ActivatePassiveEffect 事件上
		AuraASC->ActivatePassiveEffect.AddUObject(this,&UPassiveNiagaraComponent::OnPassiveActivate);
	}
	else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))	// 如果当前拥有者还没有 ASC（通常在敌人或角色初始化早期会出现）
	{
		// 那么就注册一个监听，当拥有者稍后注册 ASC 时会触发此委托
		CombatInterface->GetOnASCRegisteredDelegate().AddLambda([this](UAbilitySystemComponent* ASC)
		{
			// 当 ASC 注册完成后，再次尝试获取 AuraAbilitySystemComponent
			if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
			{
				AuraASC->ActivatePassiveEffect.AddUObject(this,&UPassiveNiagaraComponent::OnPassiveActivate);// 然后同样把 OnPassiveActivate 函数绑定到被动效果激活事件上
			}
		});
	}
}

// 当某个被动技能被激活或关闭时触发（由 ASC 的 ActivatePassiveEffect 委托调用）
void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
	if (AbilityTag.MatchesTagExact(PassiveSpellTag))// 判断传入的标签是否与本组件绑定的被动技能标签完全匹配（确保是属于自己的被动效果）
	{
		if (bActivate && !IsActive())// 如果被动技能被激活，且当前 Niagara 组件还未启动
		{
			Activate();// 启动 Niagara 特效组件（例如持续发光、特效环绕等）
		}
		else// 否则（可能是被动关闭、或重复触发时）
		{
			Deactivate();
		}
	}
}
