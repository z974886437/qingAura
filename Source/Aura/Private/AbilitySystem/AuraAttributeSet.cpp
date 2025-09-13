// 青楼


#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerController.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	//InitHealth(10.f);//启动健康，初始化健康为100
	//InitMana(10.f);//初始化法力为50
	//InitMaxHealth(100.f);//
	//InitMaxMana(50.f);//初始化最大健康为100

	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();// 获取 Aura 项目中定义的所有 GameplayTag 单例引用（用于查找标签）
	/* Primary Attributes */
	//FAttributeSignature StrengthDelegate;// 创建一个用于获取“力量”属性的委托
	//StrengthDelegate.BindStatic(GetStrengthAttribute);// 将静态函数 GetStrengthAttribute 绑定到委托上
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength,GetStrengthAttribute);// 将“力量”标签与该委托添加到 TagsToAttributes 映射中
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence,GetIntelligenceAttribute);// 将“力量”标签与该委托添加到 TagsToAttributes 映射中
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resilience,GetResilienceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor,GetVigorAttribute);
	
	/* Secondary Attributes */
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Armor,GetArmorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ArmorPenetration,GetArmorPenetrationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_BlockChance,GetBlockChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitChance,GetCriticalHitChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage,GetCriticalHitDamageAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance,GetCriticalHitResistanceAttribute);
	
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_HealthRegeneration,GetHealthRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ManaRegeneration,GetManaRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxHealth,GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxMana,GetMaxManaAttribute);

	/* Resistance Attributes */
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Arcane,GetArcaneResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Fire,GetFireResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Lightning,GetLightningResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Physical,GetPhysicalResistanceAttribute);
	
	
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//引擎中一个更高级的网络属性复制注册宏，它：注册某个属性为网络同步变量，并设置同步条件 + 通知策略。
	// Primary Attributes 主要属性
	
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Strength,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Intelligence,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Resilience,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Vigor,COND_None,REPNOTIFY_Always);
	
	//Secondary Attributes 次要属性
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Armor,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,ArmorPenetration,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,BlockChance,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,CriticalHitChance,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,CriticalHitDamage,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,CriticalHitResistance,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,HealthRegeneration,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,ManaRegeneration,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,MaxHealth,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,MaxMana,COND_None,REPNOTIFY_Always);

	//Resistance Attribute 抗性属性
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,FireResistance,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,LightningResistance,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,ArcaneResistance,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,PhysicalResistance,COND_None,REPNOTIFY_Always);
	
	// Vital Attribute		重要属性
	
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Health,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Mana,COND_None,REPNOTIFY_Always);

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
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();//源能力系统组件
	
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

	FEffectProperties Props;//效果属性
	SetEffectProperties(Data,Props);//设置效果属性

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())//判断本次被执行的属性修改是否是“血量属性（Health）”
	{
		SetHealth(FMath::Clamp(GetHealth(),0.f,GetMaxHealth()));//当前血量限制在 0 到最大生命值之间，然后设置为该值
		UE_LOG(LogTemp,Warning,TEXT("Changed Health on %s,Health:%f"),*Props.TargetAvatarActor->GetName(),GetHealth());
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(),0.f,GetMaxMana()));//当前血量限制在 0 到最大生命值之间，然后设置为该值
	}
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())// 判断当前变化的属性是不是 IncomingDamage（即角色受到的伤害数值）
	{
		const float LocalIncomingDamage = GetIncomingDamage();// 取出当前累计的伤害值（之前可能被多次叠加）
		SetIncomingDamage(0.f);  // 把 IncomingDamage 重置为 0（避免重复结算）
		if (LocalIncomingDamage > 0.f)// 如果本次伤害值大于 0，才处理
		{
			const float NewHealth = GetHealth() - LocalIncomingDamage;// 计算新的生命值 = 旧生命值 - 伤害
			SetHealth(FMath::Clamp(NewHealth,0.f,GetMaxHealth()));// 用 Clamp 限制血量范围 [0, MaxHealth]，防止出现负数或超过最大值

			const bool bFatal = NewHealth <= 0.f;// 判断角色是否死亡（血量小于等于 0）
			if (bFatal)// 如果伤害是致命的（比如血量 <= 0）
			{
				ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor); // 尝试把目标角色转换成实现了 ICombatInterface 的对象
				if (CombatInterface)// 如果转换成功（目标确实实现了战斗接口）
				{
					CombatInterface->Die();// 调用接口里的 Die() 函数 → 触发目标的死亡逻辑
				}
			}
			else
			{
				FGameplayTagContainer TagContainer;// 定义一个 GameplayTag 容器，用来装要触发的技能标签
				TagContainer.AddTag(FAuraGameplayTags::Get().Effects_HitReact);// 往容器里添加一个“受击反应”标签（Effects.HitReact）
				Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);// 让目标的 AbilitySystemComponent（ASC）尝试根据这个标签激活对应的技能
			}
			
			const bool bBlock = UAuraAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);// 1. 从上下文读取是否格挡
			const bool bCriticalHit = UAuraAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);// 2. 从上下文读取是否暴击
			ShowFloatingText(Props,LocalIncomingDamage,bBlock,bCriticalHit);//显示浮动文本
		}
	}
}

void UAuraAttributeSet::ShowFloatingText(const FEffectProperties& Props, float Damage,bool bBlockedHit,bool bCriticalHit) const
{
	if (Props.SourceCharacter != Props.TargetCharacter)// 确认攻击者不是自己（避免自己打自己还飘伤害数字）
	{
		// 从攻击者角色(Props.SourceCharacter)获取控制它的 PlayerController，并强制转成我们自定义的 AuraPlayerController
		if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(Props.SourceCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage,Props.TargetCharacter,bBlockedHit,bCriticalHit);// 调用自定义函数，在目标角色头顶显示伤害数值（LocalIncomingDamage）
			return;
		}
		if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(Props.TargetCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage,Props.TargetCharacter,bBlockedHit,bCriticalHit);// 调用自定义函数，在目标角色头顶显示伤害数值（LocalIncomingDamage）
			
		}
	}
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Health,OldHealth);//用于实现 生命属性的网络同步 + 通知响应
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Mana,OldMana);//用于实现 法力属性的网络同步 + 通知响应
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Strength,OldStrength);//用于实现 力量属性的网络同步 + 通知响应
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Intelligence,OldIntelligence);//用于实现 智力属性的网络同步 + 通知响应
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Resilience,OldResilience);//用于实现 韧性属性的网络同步 + 通知响应
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Vigor,OldVigor);//用于实现 活力属性的网络同步 + 通知响应
}

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Armor,OldArmor);//用于实现 盔甲属性的网络同步 + 通知响应
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,ArmorPenetration,OldArmorPenetration);//用于实现 穿甲属性的网络同步 + 通知响应
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,BlockChance,OldBlockChance);//用于实现 格挡几率属性的网络同步 + 通知响应
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,CriticalHitChance,OldCriticalHitChance);//用于实现 暴击几率属性的网络同步 + 通知响应
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,CriticalHitDamage,OldCriticalHitDamage);//用于实现 暴击伤害属性的网络同步 + 通知响应
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,CriticalHitResistance,OldCriticalHitResistance);//用于实现 暴击抗性属性的网络同步 + 通知响应
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,HealthRegeneration,OldHealthRegeneration);//用于实现 生命恢复属性的网络同步 + 通知响应
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,ManaRegeneration,OldManaRegeneration);//用于实现 法力恢复属性的网络同步 + 通知响应
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,MaxHealth,OldMaxHealth);//用于实现 最大生命属性的网络同步 + 通知响应
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,MaxMana,OldMaxMana);//用于实现 最大法力属性的网络同步 + 通知响应
}

void UAuraAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,FireResistance,OldFireResistance);//用于实现 最大法力属性的网络同步 + 通知响应
}

void UAuraAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,LightningResistance,OldLightningResistance);//用于实现 最大法力属性的网络同步 + 通知响应
}

void UAuraAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,ArcaneResistance,OldArcaneResistance);//用于实现 最大法力属性的网络同步 + 通知响应
}

void UAuraAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,PhysicalResistance,OldPhysicalResistance);//用于实现 最大法力属性的网络同步 + 通知响应
}
