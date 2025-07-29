// 青楼


#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	InitHealth(50.f);//启动健康，初始化健康为100
	InitMaxHealth(100.f);//初始化最大健康为100
	InitMana(10.f);//初始化法力为50
	InitMaxMana(50.f);//初始化最大法力为50
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//引擎中一个更高级的网络属性复制注册宏，它：注册某个属性为网络同步变量，并设置同步条件 + 通知策略。
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Health,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,MaxHealth,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Mana,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,MaxMana,COND_None,REPNOTIFY_Always);
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f,GetMaxHealth());//将 NewValue 限制在 0.f 和 GetMaxHealth() 之间，防止它超出这个范围
	}
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f,GetMaxMana());//将 NewValue 限制在 0.f 和 GetMaxMana() 之间，防止它超出这个范围
	}
}

void UAuraAttributeSet::SetEffectProperties(const struct FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
		// if (Data.EvaluatedData.Attribute == GetHealthAttribute())//判断是哪种属性被修改 
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Health from GetHealth():%f"),GetHealth());
	// 	UE_LOG(LogTemp, Warning, TEXT("Magnitude:%f"),Data.EvaluatedData.Magnitude);
	// }
	//获取本次 GameplayEffect 的上下文信息（FGameplayEffectContextHandle），包括 谁施放了这个效果、命中谁、是否暴击、命中位置、投射物等信息
	// Source = couset of the effect,Target = target of the effect (owner of this AS)
	// 来源 = 效果的原因，目标等于目标的效果（该属性的所有者设置正确，目标是收到影响的事物）
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	//获取这次 GameplayEffect 的原始施法者（Instigator）所拥有的 ASC
	Props.SourceASC= Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();//源能力系统组件
	//ASC 为空或已被销毁 && GAS 未初始化完 Actor Info && Avatar 可能已被销毁或未设置
	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		//在 Gameplay Ability System（GAS） 中，用于获取 施法者（源）在世界中的具体角色实例（Actor）
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();// 获取施法者的 AvatarActor 和 PlayerController
		//如果是 AI（没有 PlayerController），尝试从 AvatarActor 中的 Pawn 获取控制器
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		if (Props.SourceController)//成功获取控制器后，再取其控制的 Pawn（就是施法者本体），通常是 Character 类型
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();//从 GameplayEffect 的回调数据中获取 被影响目标（Target）Actor
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();//从当前 GameplayEffect 的目标（被影响者）中，获取控制该目标的 控制器（AController）
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Props;
	SetEffectProperties(Data,Props);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())//判断本次被执行的属性修改是否是“血量属性（Health）”
	{
		SetHealth(FMath::Clamp(GetHealth(),0.f,GetMaxHealth()));//当前血量限制在 0 到最大生命值之间，然后设置为该值
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(),0.f,GetMaxMana()));//当前血量限制在 0 到最大生命值之间，然后设置为该值
	}
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Health,OldHealth);//用于实现 属性的网络同步 + 通知响应
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,MaxHealth,OldMaxHealth);//用于实现 属性的网络同步 + 通知响应
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Mana,OldMana);//用于实现 属性的网络同步 + 通知响应
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,MaxMana,OldMaxMana);//用于实现 属性的网络同步 + 通知响应
}


