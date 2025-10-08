// 青楼


#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UDebuffNiagaraComponent::UDebuffNiagaraComponent()
{
	bAutoActivate = false; // 设置该组件不在开始时自动激活
}

void UDebuffNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();// 调用父类的 BeginPlay

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());// 尝试获取拥有者的 CombatInterface 接口

	// 获取拥有者的 AbilitySystemComponent（ASC），并注册一个 GameplayTag 事件
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		// 注册 DebuffTag 事件，当标签的添加或移除时触发回调函数
		ASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
	}
	else if (CombatInterface)// 如果没有 ASC，且拥有 CombatInterface 接口
	{
		// 注册委托：当 ASC 被注册时，再注册 DebuffTag 事件
		CombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(this, [this](UAbilitySystemComponent* InASC)
		{
			InASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
		});
	}
}

void UDebuffNiagaraComponent::DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0) // 如果 debuff 的数量大于 0，表示 debuff 已经激活
	{
		Activate();// 激活该组件
	}
	else
	{
		Deactivate();// 否则禁用该组件
	}
}
