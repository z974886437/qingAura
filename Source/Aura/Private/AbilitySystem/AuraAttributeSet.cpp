// 青楼


#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Player/AuraPlayerController.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"

UAuraAttributeSet::UAuraAttributeSet()
{

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
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance,GetCriticalHitResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage,GetCriticalHitDamageAttribute);
	
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

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
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

void UAuraAttributeSet::PostGameplayEffectExecute(const  FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Props;//效果属性
	SetEffectProperties(Data,Props);//设置效果属性

	// 检查 TargetCharacter 是否实现了 UCombatInterface 接口
	// 如果实现了 UCombatInterface 接口，调用 Execute_IsDead 检查目标角色是否死亡
	// 如果角色已死亡，直接返回，不再执行后续代码
	if (Props.TargetCharacter->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(Props.TargetCharacter)) return;
	
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())//判断本次被执行的属性修改是否是“血量属性（Health）”
	{
		SetHealth(FMath::Clamp(GetHealth(),0.f,GetMaxHealth()));//当前血量限制在 0 到最大生命值之间，然后设置为该值
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(),0.f,GetMaxMana()));//当前血量限制在 0 到最大生命值之间，然后设置为该值
	}
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())// 判断当前变化的属性是不是 IncomingDamage（即角色受到的伤害数值）
	{
		HandleIncomingDamage(Props);
	}
	if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		HandleIncomingXP(Props);
	}
}

void UAuraAttributeSet::HandleIncomingDamage(const FEffectProperties& Props)
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
				FVector Impulse = UAuraAbilitySystemLibrary::GetDeathImpulse(Props.EffectContextHandle);// 获取致命冲击力（角色死亡时的冲击效果）
				CombatInterface->Die(UAuraAbilitySystemLibrary::GetDeathImpulse(Props.EffectContextHandle));// 调用战斗接口的 Die 函数，处理死亡逻辑
			}
			SendXPEvent(Props);// 触发经验值事件，可能是奖励经验等相关逻辑
		}
		else
		{
			FGameplayTagContainer TagContainer;// 定义一个 GameplayTag 容器，用来装要触发的技能标签
			TagContainer.AddTag(FAuraGameplayTags::Get().Effects_HitReact);// 往容器里添加一个“受击反应”标签（Effects.HitReact）
			Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);// 让目标的 AbilitySystemComponent（ASC）尝试根据这个标签激活对应的技能

			const FVector& KnockbackForce = UAuraAbilitySystemLibrary::GetKnockbackForce(Props.EffectContextHandle);
			if (!KnockbackForce.IsNearlyZero(1.f))
			{
				Props.TargetCharacter->LaunchCharacter(KnockbackForce,true,true);
			}
		}
			
		const bool bBlock = UAuraAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);// 1. 从上下文读取是否格挡
		const bool bCriticalHit = UAuraAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);// 2. 从上下文读取是否暴击
		ShowFloatingText(Props,LocalIncomingDamage,bBlock,bCriticalHit);//显示浮动文本
		if (UAuraAbilitySystemLibrary::IsSuccessfulDebuff(Props.EffectContextHandle))
		{
			Debuff(Props);
		}
	}
}

void UAuraAttributeSet::Debuff(const FEffectProperties& Props)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();// 获取全局的 Aura 游戏标签，可能用于标识特定的伤害类型和效果
	FGameplayEffectContextHandle EffectContext = Props.SourceASC->MakeEffectContext();// 创建一个新的效果上下文
	EffectContext.AddSourceObject(Props.SourceAvatarActor);// 将源角色对象添加到效果上下文中

	// 获取伤害类型、减益伤害、减益持续时间和频率
	const FGameplayTag DamageType = UAuraAbilitySystemLibrary::GetDamageType(Props.EffectContextHandle);
	const float DebuffDamage = UAuraAbilitySystemLibrary::GetDebuffDamage(Props.EffectContextHandle);
	const float DebuffDuration = UAuraAbilitySystemLibrary::GetDebuffDuration(Props.EffectContextHandle);
	const float DebuffFrequency = UAuraAbilitySystemLibrary::GetDebuffFrequency(Props.EffectContextHandle);

	FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"),*DamageType.ToString());// 使用伤害类型来创建减益效果的名称
	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(),FName(DebuffName));// 创建一个新的游戏效果对象

	Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;// 设置效果的持续时间策略

	// 设置效果的周期和持续时间
	Effect->Period = DebuffFrequency;
	Effect->DurationMagnitude = FScalableFloat(DebuffDuration);

	//Effect->InheritableOwnedTagsContainer.AddTag(GameplayTags.DamageTypesToDebuff[DamageType]);// 添加与减益效果相关的标签
	FInheritedTagContainer TagContainer = FInheritedTagContainer();// 创建一个空的 FInheritedTagContainer 对象，用于存储标签信息
	UTargetTagsGameplayEffectComponent& Component = Effect->FindOrAddComponent<UTargetTagsGameplayEffectComponent>();// 获取或添加一个 UTargetTagsGameplayEffectComponent 组件
	TagContainer.Added.AddTag(GameplayTags.DamageTypesToDebuff[DamageType]);// 将 DamageType 对应的标签添加到 TagContainer 的 "Added" 标签集中
	Component.SetAndApplyTargetTagChanges(TagContainer);// 将 TagContainer 中的标签信息应用到目标标签组件

	// 设置效果的堆叠方式
	Effect->StackingType = EGameplayEffectStackingType::AggregateBySource;
	Effect->StackLimitCount = 1;
	
	// 在效果的修饰符数组中添加一个新的修饰符
	const int32 Index = Effect->Modifiers.Num();
	Effect->Modifiers.Add(FGameplayModifierInfo());
	FGameplayModifierInfo& ModifierInfo = Effect->Modifiers[Index];

	// 配置修饰符的值和作用类型
	ModifierInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.Attribute = UAuraAttributeSet::GetIncomingDamageAttribute();

	// 创建一个效果规范并设置上下文
	if (FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect,EffectContext,1.f))
	{
		FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(MutableSpec->GetContext().Get());// 强制转换为 Aura 特定的效果上下文

		// 将伤害类型设置到上下文中
		TSharedPtr<FGameplayTag> DebuffDamageType = MakeShareable(new FGameplayTag(DamageType));
		AuraContext->SetDamageType(DebuffDamageType);

		Props.TargetASC->ApplyGameplayEffectSpecToSelf(*MutableSpec);// 应用效果到目标的 Ability System Component
	}
}

void UAuraAttributeSet::HandleIncomingXP(const FEffectProperties& Props)
{
	const float LocalIncomingXP = GetIncomingXP();// 取出临时存放的经验值
	SetIncomingXP(0.f);// 把 IncomingXP 清零，避免经验值重复计算
	//UE_LOG(LogAura,Log,TEXT("Incoming XP:%f"),LocalIncomingXP);

	//TODO:See if we should level up 判断是否应该升级

	//Source Character is the owner, since GA_ListenForEvents applies GE_EventBasedEffect,adding to InComingXP
	//源字符是所有者，因为GA_ListenForEvents应用GE_EventBasedEffect，添加到 InComingXP
	if (Props.SourceCharacter->Implements<UPlayerInterface>() && Props.SourceCharacter->Implements<UCombatInterface>())// 确认角色实现了 PlayerInterface 才能调用接口
	{
		const int32 CurrentLevel = ICombatInterface::Execute_GetPlayerLevel(Props.SourceCharacter);// 获取角色当前等级（通过 CombatInterface）
		const int32 CurrentXP = IPlayerInterface::Execute_GetXP(Props.SourceCharacter);// 获取角色当前经验值（通过 PlayerInterface）
			
		const int32 NewLevel = IPlayerInterface::Execute_FindLevelForXP(Props.SourceCharacter,CurrentXP + LocalIncomingXP);// 计算加上这次获得的经验后的新等级
		const int32 NumLevelUps = NewLevel - CurrentLevel;// 计算实际升级了多少级（新等级 - 旧等级）
		if (NumLevelUps > 0)// 如果确实升级了（等级差 > 0）
		{
			// TODO: 目前只取当前等级的奖励，通常应该按每一级循环发放奖励
			const int32 AttributePointsReward = IPlayerInterface::Execute_GetAttributePointsReward(Props.SourceCharacter,CurrentLevel);
			const int32 SpellPointsReward = IPlayerInterface::Execute_GetSpellPointsReward(Props.SourceCharacter,CurrentLevel);

			IPlayerInterface::Execute_AddToPlayerLevel(Props.SourceCharacter,NumLevelUps);// 给角色增加等级
			IPlayerInterface::Execute_AddToAttributePoints(Props.SourceCharacter,AttributePointsReward);// 增加属性点奖励
			IPlayerInterface::Execute_AddToSpellPoints(Props.SourceCharacter,SpellPointsReward);// 增加技能点奖励

			// 升级时满血满蓝
			bTopOffHealth = true;
			bTopOffMana = true;
				
			IPlayerInterface::Execute_LevelUp(Props.SourceCharacter);// 调用角色的升级事件（比如播放特效、UI提示）
		}
			
		IPlayerInterface::Execute_AddToXP(Props.SourceCharacter,LocalIncomingXP);// 调用接口，把经验加到 SourceCharacter（通常是玩家自己）身上
	}
}



void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)// 当属性值发生改变时会调用这个回调函数
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);// 先调用父类逻辑，保证基类处理正常

	if (Attribute == GetMaxHealthAttribute() && bTopOffHealth)// 如果 MaxHealth 被修改，并且允许 TopOff（把血量补满）
	{
		SetHealth(GetMaxHealth());// 把当前血量直接设置为最大血量
		bTopOffHealth = false;
	}
	if (Attribute == GetMaxManaAttribute() && bTopOffMana)// 如果 MaxMana 被修改，并且允许 TopOff（把蓝量补满）
	{
		SetMana(GetMaxMana());// 把当前法力值直接设置为最大法力
		bTopOffMana = false;
	}
}

void UAuraAttributeSet::SendXPEvent(const FEffectProperties& Props)
{
	if (Props.TargetCharacter->Implements<UCombatInterface>())// 判断目标是否实现了战斗接口（CombatInterface），确保能获取等级和职业
	{
		const int32 TargetLevel = ICombatInterface::Execute_GetPlayerLevel(Props.TargetCharacter);// 获取目标角色等级
		const ECharacterClass TargetClass = ICombatInterface::Execute_GetCharacterClass(Props.TargetCharacter);// 获取目标角色职业（比如战士、法师）
		const int32 XPReward = UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(Props.TargetCharacter,TargetClass,TargetLevel);// 根据目标职业和等级，计算应该奖励多少经验值

		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();// 获取全局 GameplayTags 单例，用于标记事件类型
		FGameplayEventData Payload;// 构造 GameplayEvent 的数据载体（Payload）
		Payload.EventTag = GameplayTags.Attributes_Meta_IncomingXP;// 事件标签 → "获得经验"
		Payload.EventMagnitude = XPReward; // 事件强度 → 经验数值
		// 发送 GameplayEvent 给源角色（通常是击杀者），让它能获得经验奖励
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter,GameplayTags.Attributes_Meta_IncomingXP,Payload);
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
	//UE_LOG(LogTemp, Warning, TEXT("OnRep_Health Triggered! New=%f"), GetHealth());
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


