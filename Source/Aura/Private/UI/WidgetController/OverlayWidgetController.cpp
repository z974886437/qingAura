// 青楼


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());//广播一个多播委托事件，并传递当前角色的生命值（Health）给所有监听者（比如 UI 血条）
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());

	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	//将一个函数（HealthChanged）绑定到 GAS（Gameplay Ability System）中的属性变更事件委托上，属于 GAS 属性监听的标准写法
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);//当 GAS 系统检测到 Health 变化时，调用这个函数；
			}
		);
	
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);//当 GAS 系统检测到 MaxHealth 变化时，调用这个函数；
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnManaChanged.Broadcast(Data.NewValue);//当 GAS 系统检测到 mana 变化时，调用这个函数；
			}
		);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxManaChanged.Broadcast(Data.NewValue);//当 GAS 系统检测到 MaxMana 变化时，调用这个函数；
			}
		);

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))//  检查并初始化 Aura ASC
	{
		if (AuraASC->bStartupAbilitiesGiven)// 如果 ASC 已经初始化过起始技能，则直接调用初始化逻辑
		{
			OnInitializeStartupAbilities(AuraASC);
		}
		else
		{
			AuraASC->AbilitiesGivenDelegate.AddUObject(this,&UOverlayWidgetController::OnInitializeStartupAbilities);// 否则注册一个回调，等它初始化完成后再调用
		}
		
		//在注册一个对 Gameplay Tag 事件的监听函数（Lambda）
		AuraASC->EffectAssetTags.AddLambda(
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

void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraAbilitySystemComponent)
{
	//TODO Get information about all given abilities,look up their Ability Info,and broadcast it to widgets.获取有关所有给定能力的信息，查找他们的能力信息，并将其广播到小部件。
	if (!AuraAbilitySystemComponent->bStartupAbilitiesGiven) return;// 如果初始技能还没分发，就直接返回（防止重复初始化）

	FForEachAbility BroadcastDelegate;// 定义一个委托，等下遍历所有技能时会执行
	// 给委托绑定一个 Lambda
	BroadcastDelegate.BindLambda([this,AuraAbilitySystemComponent](const FGameplayAbilitySpec& AbilitySpec)
	{
		//TODO need a way to figure out the ability tag for a given ability spec.需要一种方法来找出给定技能规格的能力标签。
		// 1. 根据技能实例 (AbilitySpec) 拿到技能标签 (Abilities.xxx)
		FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AuraAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = AuraAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);// 2. 再拿到该技能对应的输入标签 (InputTag.Q / InputTag.LMB)
		AbilityInfoDelegate.Broadcast(Info); // 3. 把完整的技能信息广播出去，让 UI 刷新（比如技能栏显示图标/冷却）
	});
	AuraAbilitySystemComponent->ForEachAbility(BroadcastDelegate);// 遍历所有技能，逐个执行上面绑定的 Lambda
	
	
}
