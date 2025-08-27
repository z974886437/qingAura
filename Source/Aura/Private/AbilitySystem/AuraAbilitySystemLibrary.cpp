// 青楼


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WidgetController//AuraWidgetController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

// 获取用于控制主 UI 叠加层（Overlay）的 Widget 控制器（Controller）对象
UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	// 尝试从世界上下文对象获取索引为 0 的玩家控制器（一般是本地玩家）
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject,0))
	{
		// 将 HUD 强转为自定义的 AAuraHUD，确保我们有能力访问 Aura 专属的 UI 系统
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();// 获取玩家状态（PlayerState），通常保存属性和能力组件
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();// 从 PlayerState 获取 GAS 的核心组件 AbilitySystemComponent
			UAttributeSet* AS = PS->GetAttributeSet();// 获取属性集 AttributeSet，包含血量、法力等自定义属性
			const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);// 构造控件控制器所需的参数结构体（封装了 UI 所需要的数据来源）
			return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);// 返回主 UI 控制器实例（用于驱动血条、蓝条等 HUD 显示）
		}
	}
	return nullptr;// 如果以上任何一步失败，返回空指针
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	// 尝试从世界上下文对象获取索引为 0 的玩家控制器（一般是本地玩家）
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject,0))
	{
		// 将 HUD 强转为自定义的 AAuraHUD，确保我们有能力访问 Aura 专属的 UI 系统
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();// 获取玩家状态（PlayerState），通常保存属性和能力组件
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();// 从 PlayerState 获取 GAS 的核心组件 AbilitySystemComponent
			UAttributeSet* AS = PS->GetAttributeSet();// 获取属性集 AttributeSet，包含血量、法力等自定义属性
			const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);// 构造控件控制器所需的参数结构体（封装了 UI 所需要的数据来源）
			return AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams);// 返回属性菜单小部件控制器
		}
	}
	return nullptr;// 如果以上任何一步失败，返回空指针
	
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject,ECharacterClass CharacterClass, float Level,UAbilitySystemComponent* ASC)
{
	// // 从当前世界上下文获取 GameMode，并强转成我们自定义的 AAuraGameModeBase
	// AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	// if (AuraGameMode == nullptr) return;// 如果没拿到，直接返回，避免崩溃

	AActor* AvatarActor = ASC->GetAvatarActor();// 获取当前 ASC（AbilitySystemComponent）的 AvatarActor（也就是角色本体）
	
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);// 从 GameMode 中拿到角色职业信息（CharacterClassInfo 数据表/配置类）
	FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);// 根据传入的职业枚举，获取该职业的默认属性配置（ClassDefaultInfo）

	// --- 应用 PrimaryAttributes（主要属性，例如 力量、智力、敏捷）
	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();// 创建上下文，记录来源
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);// 添加来源 Actor（用于溯源，谁造成的效果）
	// 生成一个属性效果规格（Spec）
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes,Level,PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());// 应用到自己

	// --- 应用 SecondaryAttributes（次要属性，例如 暴击率、格挡率、生命恢复速度）
	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes,Level,SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	// --- 应用 VitalAttributes（关键属性，例如 当前生命值、魔法值、体力）
	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes,Level,VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC,ECharacterClass CharacterClass)
{
	// // 从当前世界上下文获取 GameMode，并强转成我们自定义的 AAuraGameModeBase
	// AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	// if (AuraGameMode == nullptr) return;// 如果没拿到，直接返回，避免崩溃

	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);// 从 GameMode 中拿到角色职业信息（CharacterClassInfo 数据表/配置类）
	if (CharacterClassInfo == nullptr) return;// 如果没取到配置，直接返回，避免空指针
	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)// 遍历角色职业配置里存放的“通用技能”数组（这些技能适用于所有该类角色）
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,1);// 创建一个技能规格(FGameplayAbilitySpec)，这里指定技能类和技能等级（固定为1级）
		ASC->GiveAbility(AbilitySpec);// 把技能规格交给 AbilitySystemComponent（ASC），让角色真正拥有这个技能
	}
	const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);// 获取该职业的默认配置（不同职业有不同的初始技能）
	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.StartupAbilities)// 遍历“初始技能”数组（这些技能通常随角色等级变化）
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(ASC->GetAvatarActor()))// 如果角色实现了 ICombatInterface，就能获取玩家等级
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,CombatInterface->GetPlayerLevel()); // 创建技能规格：技能类 + 角色当前等级
			ASC->GiveAbility(AbilitySpec);// 授予技能给 ASC
		}
	}
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	// 从世界上下文对象获取当前游戏模式，并尝试转换成我们自定义的 AAuraGameModeBase
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return nullptr;// 如果转换失败（非我们自定义 GameMode），直接返回 nullptr
	return AuraGameMode->CharacterClassInfo;// 返回自定义 GameMode 中存储的 CharacterClassInfo（角色职业信息类）
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	// 通过句柄获取指针，并强制转换为自定义的 FAuraGameplayEffectContext
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsBlockedHit(); // 调用自定义方法，返回本次命中是否被阻挡
	}
	return false; // 如果转换失败或 Context 为空，则默认未阻挡
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	// 通过句柄获取指针，并强制转换为自定义的 FAuraGameplayEffectContext
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsCriticalHit(); // 调用自定义方法，返回本次命中是否被阻挡
	}
	return false; // 如果转换失败或 Context 为空，则默认未阻挡
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	// 从通用的 Context 中拿到我们自定义的 Aura 版本（FAuraGameplayEffectContext）
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsBlockedHit(bInIsBlockedHit); // 设置格挡状态
	}
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsCriticalHit)
{
	// 从通用的 Context 中拿到我们自定义的 Aura 版本（FAuraGameplayEffectContext）
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsCriticalHit(bInIsCriticalHit); // 设置暴击状态
	}
}
