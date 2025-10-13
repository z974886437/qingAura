// 青楼


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

bool UAuraAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject,FWidgetControllerParams& OutWCParams,AAuraHUD*& OutAuraHUD)
{
	// 尝试从世界上下文对象获取索引为 0 的玩家控制器（一般是本地玩家）
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject,0))
	{
		OutAuraHUD = Cast<AAuraHUD>(PC->GetHUD());// 获取当前 HUD，并转型为自定义的 AAuraHUD
		if (OutAuraHUD)// 如果 HUD 有效，继续提取 GAS 相关对象
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();// 玩家状态，存放 GAS 相关信息
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();// AbilitySystemComponent：GAS 核心组件，管理技能和效果
			UAttributeSet* AS = PS->GetAttributeSet();// AttributeSet：存放血量、蓝量、攻击力等属性

			// 把这些依赖打包进 WidgetControllerParams
			OutWCParams.AttributeSet = AS;
			OutWCParams.AbilitySystemComponent = ASC;
			OutWCParams.PlayerState = PS;
			OutWCParams.PlayerController = PC;
			return true;// 成功构建参数
		}
	}
	return false; // 获取失败，HUD 或 PC 不存在
}

// 获取用于控制主 UI 叠加层（Overlay）的 Widget 控制器（Controller）对象
UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams; // 声明一个 WidgetController 参数结构体（用于初始化控制器）
	AAuraHUD* AuraHUD = nullptr;// 准备一个 HUD 指针，用来接收 AuraHUD
	if (MakeWidgetControllerParams(WorldContextObject,WCParams,AuraHUD))// 调用上一个工具函数，尝试获取 WCParams 和 AuraHUD
	{
		return AuraHUD->GetOverlayWidgetController(WCParams);// 如果成功，就从 HUD 里获取 OverlayWidgetController
	}
	return nullptr;// 如果失败（找不到 PlayerController/HUD/PlayerState/ASC/AS），返回空指针
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams; // 声明一个 WidgetController 参数结构体（用于初始化控制器）
	AAuraHUD* AuraHUD = nullptr;// 准备一个 HUD 指针，用来接收 AuraHUD
	if (MakeWidgetControllerParams(WorldContextObject,WCParams,AuraHUD))// 调用上一个工具函数，尝试获取 WCParams 和 AuraHUD
	{
		return AuraHUD->GetAttributeMenuWidgetController(WCParams);// 如果成功，就从 HUD 里获取 OverlayWidgetController
	}
	return nullptr;// 如果失败（找不到 PlayerController/HUD/PlayerState/ASC/AS），返回空指针
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams; // 声明一个 WidgetController 参数结构体（用于初始化控制器）
	AAuraHUD* AuraHUD = nullptr;// 准备一个 HUD 指针，用来接收 AuraHUD
	if (MakeWidgetControllerParams(WorldContextObject,WCParams,AuraHUD))// 调用上一个工具函数，尝试获取 WCParams 和 AuraHUD
	{
		return AuraHUD->GetSpellMenuWidgetController(WCParams);// 如果成功，就从 HUD 里获取 OverlayWidgetController
	}
	return nullptr;// 如果失败（找不到 PlayerController/HUD/PlayerState/ASC/AS），返回空指针
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
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>())// 如果角色实现了 ICombatInterface，就能获取玩家等级
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor())); // 创建技能规格：技能类 + 角色当前等级
			ASC->GiveAbility(AbilitySpec);// 授予技能给 ASC
		}
	}
}

int32 UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject,ECharacterClass CharacterClass, int32 CharacterLevel)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);// 从 GameMode 中拿到角色职业信息（CharacterClassInfo 数据表/配置类）
	if (CharacterClassInfo == nullptr) return 0;// 如果没取到配置，直接返回，

	const FCharacterClassDefaultInfo& Info = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);// 根据角色枚举 CharacterClass 获取该职业的默认信息（血量、蓝量、攻击力等基础数值）
	const float XPReward = Info.XPReward.GetValueAtLevel(CharacterLevel);

	return static_cast<int32>(XPReward);
	
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	// 从世界上下文对象获取当前游戏模式，并尝试转换成我们自定义的 AAuraGameModeBase
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return nullptr;// 如果转换失败（非我们自定义 GameMode），直接返回 nullptr
	return AuraGameMode->CharacterClassInfo;// 返回自定义 GameMode 中存储的 CharacterClassInfo（角色职业信息类）
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	// 从世界上下文对象获取当前游戏模式，并尝试转换成我们自定义的 AAuraGameModeBase
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return nullptr;// 如果转换失败（非我们自定义 GameMode），直接返回 nullptr
	return AuraGameMode->AbilityInfo;// 返回自定义 GameMode 中存储的 CharacterClassInfo（角色职业信息类）
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

bool UAuraAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	// 通过句柄获取指针，并强制转换为自定义的 FAuraGameplayEffectContext
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsSuccessfulDebuff(); // 调用自定义方法，返回本次命中是否被阻挡
	}
	return false; // 如果转换失败或 Context 为空，则默认未阻挡
}

float UAuraAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	// 通过句柄获取指针，并强制转换为自定义的 FAuraGameplayEffectContext
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDebuffDamage(); // 调用自定义方法，返回本次命中是否被阻挡
	}
	return 0.f; // 如果转换失败或 Context 为空，则默认未阻挡
}

float UAuraAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	// 通过句柄获取指针，并强制转换为自定义的 FAuraGameplayEffectContext
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDebuffDuration(); // 调用自定义方法，返回本次命中是否被阻挡
	}
	return 0.f; // 如果转换失败或 Context 为空，则默认未阻挡
}

float UAuraAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	// 通过句柄获取指针，并强制转换为自定义的 FAuraGameplayEffectContext
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDebuffFrequency(); // 调用自定义方法，返回本次命中是否被阻挡
	}
	return 0.f; // 如果转换失败或 Context 为空，则默认未阻挡
}

// 获取伤害类型的函数，根据传入的 GameplayEffectContextHandle 获取相应的 DamageType
FGameplayTag UAuraAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	// 通过句柄获取指针，并强制转换为自定义的 FAuraGameplayEffectContext
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (AuraEffectContext->GetDamageType().IsValid())// 如果获取到的 DamageType 是有效的（IsValid()）
		{
			return *AuraEffectContext->GetDamageType();// 返回有效的 DamageType 对应的 FGameplayTag
		}
	}
	return FGameplayTag();// 如果转换失败或者 DamageType 无效，则返回一个默认的空 FGameplayTag（表示未定义的伤害类型）
}

FVector UAuraAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	// 尝试将 EffectContextHandle 转换为自定义的 FAuraGameplayEffectContext 类型
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDeathImpulse(); // 如果转换成功，获取死亡冲击力（Impulse）
	}
	return FVector::ZeroVector;// 如果转换失败，返回零向量（表示没有有效的冲击力）
}

FVector UAuraAbilitySystemLibrary::GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	// 尝试将 EffectContextHandle 转换为自定义的 FAuraGameplayEffectContext 类型
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetKnockbackForce(); // 如果转换成功，获取死亡冲击力（Impulse）
	}
	return FVector::ZeroVector;// 如果转换失败，返回零向量（表示没有有效的冲击力）
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

void UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInSuccessfulDebuff)
{
	// 从通用的 Context 中拿到我们自定义的 Aura 版本（FAuraGameplayEffectContext）
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsSuccessfulDebuff(bInSuccessfulDebuff); // 设置成功减益
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, float InDamage)
{
	// 从通用的 Context 中拿到我们自定义的 Aura 版本（FAuraGameplayEffectContext）
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffDamage(InDamage); // 设置成功减益
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, float InDuration)
{
	// 从通用的 Context 中拿到我们自定义的 Aura 版本（FAuraGameplayEffectContext）
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffDuration(InDuration); // 设置成功减益
	}
}

void UAuraAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle, float InFrequency)
{
	// 从通用的 Context 中拿到我们自定义的 Aura 版本（FAuraGameplayEffectContext）
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffFrequency(InFrequency); // 设置成功减益
	}
}

// 设置伤害类型，将伤害类型（InDamageType）应用到效果上下文（EffectContextHandle）中
void UAuraAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle,const FGameplayTag& InDamageType)
{
	// 从通用的 EffectContextHandle 中获取自定义的 Aura 版本（FAuraGameplayEffectContext）
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		// 使用传入的 FGameplayTag 参数 InDamageType 创建一个 TSharedPtr<FGameplayTag> 智能指针
		const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		// 将创建的 DamageType 设置到 AuraEffectContext 中，应用新的伤害类型
		AuraEffectContext->SetDamageType(DamageType); // 设置成功减益
	}
}

void UAuraAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InImpulse)
{
	// 尝试将 EffectContextHandle 转换为自定义的 FAuraGameplayEffectContext 类型
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDeathImpulse(InImpulse); // 如果转换成功，设置死亡冲击力（Impulse）
	}
}

void UAuraAbilitySystemLibrary::SetKnockbackForce(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InForce)
{
	// 尝试将 EffectContextHandle 转换为自定义的 FAuraGameplayEffectContext 类型
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetKnockbackForce(InForce); // 如果转换成功，设置死亡冲击力（Impulse）
	}
}

//在半径内获取现场玩家
void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject,// 世界上下文，一般用来获取 UWorld
	TArray<AActor*>& OutOverlappingActors, // 输出参数：存放范围内的玩家
	const TArray<AActor*>& ActorsToIgnore, // 输入参数：忽略的 Actor，比如自己
	float Radius, // 搜索半径
	const FVector& SphereOrigin// 搜索的球体中心点
	)
{
	FCollisionQueryParams SphereParams;// 创建球体查询参数对象
	SphereParams.AddIgnoredActors(ActorsToIgnore);// 忽略指定的 Actor，避免检测到自己或无关对象

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))// 获取当前世界对象，保证查询在正确的世界中执行
	{
		TArray<FOverlapResult> Overlaps;// 用于存储球体检测结果（可能重叠的对象）
		// 在 SphereOrigin 位置做一个半径为 Radius 的球体检测
		// 查询所有动态物体（角色、敌人、NPC等），结果存到 Overlaps
		World->OverlapMultiByObjectType(Overlaps,// 输出：重叠结果数组
			SphereOrigin, // 球心位置
			FQuat::Identity,// 没有旋转，单位四元数
			// 设置查询参数为“所有动态对象”，包含 Pawn、动态物体、物理体、载具、可破坏物体等，不会检测静态地形
			FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
			FCollisionShape::MakeSphere(Radius), // 生成一个半径为 Radius 的球体形状
			SphereParams// 查询参数（包含忽略的 Actor）
			);
		for (FOverlapResult& Overlap : Overlaps)// 遍历所有重叠到的对象
		{
			//const bool ImplementsCombatInterface = Overlap.GetActor()->Implements<UCombatInterface>();// 判断这个 Actor 是否实现了 ICombatInterface 接口
			//const bool IsAlive = !ICombatInterface::Execute_IsDead(Overlap.GetActor());// 调用接口函数 IsDead()，检查是否还活着
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))// 如果实现了接口，并且还活着
			{
				// 获取 Avatar（一般是 Pawn 或 Character），添加到输出数组，避免重复
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
			}
		}
	}
}

// 功能：从给定的 Actor 数组中，找出距离指定位置（Origin）最近的若干目标（最多 MaxTargets 个）
void UAuraAbilitySystemLibrary::GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors,TArray<AActor*>& OutClosestTargets, const FVector& Origin)
{
	if (Actors.Num() <= MaxTargets)// 如果目标数量少于或等于需要的最大数量，则直接全部返回
	{
		OutClosestTargets = Actors;// 直接复制
		return;// 提前结束，无需计算距离
	}

	TArray<AActor*> ActorsToCheck = Actors;// 拷贝一份可修改的待筛选列表
	int32 NumTargetsFound = 0;// 记录已找到的目标数量

	while (NumTargetsFound < MaxTargets)// 循环直到找到足够数量的目标
	{
		if (ActorsToCheck.Num() == 0) break;
		double ClosestDistance = TNumericLimits<double>::Max();// 初始化当前最近距离为“无限大”
		AActor* ClosestActor;// 用于记录当前最近的 Actor 指针
		
		for (AActor* PotentialTarget : ActorsToCheck)// 遍历所有待检查的 Actor
		{
			const double Distance = (PotentialTarget->GetActorLocation() - Origin).Length();// 计算当前 Actor 与指定位置之间的欧几里得距离
			
			if (Distance < ClosestDistance)// 如果该 Actor 距离比当前记录的最近距离更近，则更新记录
			{
				ClosestDistance = Distance;
				ClosestActor = PotentialTarget;
			}
		}
		ActorsToCheck.Remove(ClosestActor);// 从待检查列表中移除刚找到的最近目标
		OutClosestTargets.AddUnique(ClosestActor);// 将该最近目标添加到输出列表（AddUnique防止重复）
		++NumTargetsFound;// 计数器 +1
	}
}

bool UAuraAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool bBothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));// 判断两个Actor是否都是玩家
	const bool bBothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));// 判断两个Actor是否都是敌人
	const bool bFriends = bBothArePlayers || bBothAreEnemies;// 如果两个都是玩家，或者两个都是敌人，就认为他们是“朋友”
	return !bFriends;// 返回是否是敌人（即不是朋友）
}

// 应用伤害效果，根据传入的参数构建并施加 GameplayEffect，返回上下文句柄
FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();// 获取全局的 Aura GameplayTags，用于引用统一的标签
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();// 获取施法者的 AvatarActor（技能来源实体）
	
	FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();// 创建一个效果上下文，用来存放施法者、命中信息等
	EffectContextHandle.AddSourceObject(SourceAvatarActor);// 把施法者对象加入上下文，便于后续追溯伤害来源
	SetDeathImpulse(EffectContextHandle, DamageEffectParams.DeathImpulse);// 设置死亡冲击力（Death Impulse）到效果上下文
	SetKnockbackForce(EffectContextHandle,DamageEffectParams.KnockbackForce);// 设置击退力（KnockbackForce）到效果上下文
	
	// 构建一个 GameplayEffect 规格（Spec），包含效果类、技能等级和上下文
	const FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(
		DamageEffectParams.DamageGameplayEffectClass,     // 使用的伤害效果类
		DamageEffectParams.AbilityLevel, // 技能等级
		EffectContextHandle // 上下文
		);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,DamageEffectParams.DamageType,DamageEffectParams.BaseDamage);// 设置伤害数值，绑定到具体的伤害类型（如火焰、冰霜）
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,GameplayTags.Debuff_Chance,DamageEffectParams.DebuffChance);// 设置 Debuff 触发概率
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,GameplayTags.Debuff_Damage,DamageEffectParams.DebuffDamage);// 设置 Debuff 每次伤害
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,GameplayTags.Debuff_Duration,DamageEffectParams.DebuffDuration);// 设置 Debuff 持续时间
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,GameplayTags.Debuff_Frequency,DamageEffectParams.DebuffFrequency);// 设置 Debuff 触发频率
	
	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);// 将效果规格应用到目标的能力系统组件上（即目标受到效果影响）
	return EffectContextHandle;// 返回效果上下文句柄
}

TArray<FRotator> UAuraAbilitySystemLibrary::EvenlySpacedRotators(const FVector& Forward, const FVector& Axis,float Spread,int32 NumRotators)
{
	TArray<FRotator> Rotators;// 定义一个数组来存储最终的旋转角度（FRotator）
	
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f,Axis); // 根据扩散角度（Spread）和给定的旋转轴（Axis）计算投射物的起始方向
	if (NumRotators > 1) // 如果投射物数量大于 1，则计算每个投射物之间的旋转角度
	{
		const float DeltaSpread = Spread / (NumRotators - 1);// 计算每个投射物之间的扩散角度
		for (int32 i = 0; i < NumRotators;i++)// 遍历每个投射物
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i,FVector::UpVector);// 根据扩散角度旋转，确定每个投射物的发射方向
			Rotators.Add(Direction.Rotation());// 将方向向量转换为旋转角度并加入到 Rotators 数组中
		}
	}
	else
	{
		Rotators.Add(Forward.Rotation()); // 如果只有一个投射物，直接返回前进方向的旋转角度
	}
	return Rotators;// 返回所有计算得到的旋转角度
}

// 函数：计算指定扩散角度和数量的均匀分布向量
TArray<FVector> UAuraAbilitySystemLibrary::EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis,float Spread,int32 NumVectors)
{
	TArray<FVector> Vectors;// 存储计算得到的均匀分布的向量
	
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f,Axis);// 计算扩散角度的左侧起始方向（相对给定的 Forward 方向旋转一定角度）
	if (NumVectors > 1)// 如果投射物数量大于1，则进行均匀分布
	{
		const float DeltaSpread = Spread / (NumVectors - 1);// 计算每个投射物之间的扩散角度
		for (int32 i = 0; i < NumVectors;i++)// 遍历每个投射物
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i,FVector::UpVector);// 根据扩散角度旋转，确定每个投射物的发射方向
			Vectors.Add(Direction);// 将计算出的方向添加到向量列表中
		}
	}
	else
	{
		Vectors.Add(Forward);// 如果只有一个投射物，直接返回原始的 Forward 方向
	}
	return Vectors;// 返回计算得到的均匀分布向量列表
}


