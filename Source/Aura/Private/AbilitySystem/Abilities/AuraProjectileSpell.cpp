// 青楼


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "Aura/Public/AuraGameplayTags.h"

//#include "Kismet/KismetSystemLibrary.h"

// 返回火球术技能的描述字符串（根据等级显示不同效果）
FString UAuraProjectileSpell::GetDescription(int32 Level)
{
	const int32 Damage = DamageTypes[FAuraGameplayTags::Get().Damage_Fire].GetValueAtLevel(Level);// 根据技能等级获取火焰伤害数值
	if (Level == 1)// 如果是等级 1，显示单发火球描述
	{
		// // 使用 FString::Printf 格式化技能描述（1 颗火球）
		// return FString::Printf(
		// 	TEXT("<Title>FIRE BOLT</>\n\n"
		// 	"<Default>Launches a bolt of fire, exploding On impact and dealing: </>"
		// 	"<Damage>%d</><Default> fire damage with a chance to burn </>\n\n"
		// 	"<Small>Level: </><Level>%d</>"),
		// 	Damage,// 替换 %d 为伤害数值
		// 	Level// 替换 %d 为等级
		// 	);
		// 使用 FString::Printf 格式化技能描述（1 颗火球）
		return FString::Printf(
			TEXT("<Title>火球术</>\n\n"
			"<Default>发射 1 个火球, 技能效果: </>"
			"<Damage>%d</><Default> 火焰伤害，有几率燃烧 </>\n\n"
			"<Small>等级: </><Level>%d</>"),
			Damage,// 替换 %d 为伤害数值
			Level// 替换 %d 为等级
			);
	}
	else// 如果是等级 > 1，显示多发火球描述
	{
		// // 使用 FString::Printf 格式化技能描述（多颗火球，数量受 NumProjectiles 限制）
		// return FString::Printf(
		// 	TEXT("<Title>FIRE BOLT</>\n\n"
		// 		"<Default>Launches %d bolts of fire, exploding On impact and dealing: </>"
		// 		" <Damage>%d</><Default> fire damage with a chance to burn </>\n\n"
		// 		"<Small>Level: </><Level>%d</>"),
		// 		FMath::Min(Level,NumProjectiles),
		// 		Damage,
		// 		Level
		// 		);

		// 使用 FString::Printf 格式化技能描述（多颗火球，数量受 NumProjectiles 限制）
		return FString::Printf(
			TEXT("<Title>火球术</>\n\n"
				"<Default>发射 %d 个火球, 技能效果: </>"
				" <Damage>%d</><Default> 火焰伤害，有几率燃烧 </>\n\n"
				"<Small>等级: </><Level>%d</>"),
				FMath::Min(Level,NumProjectiles),
				Damage,
				Level
				);
	}
}

// 返回技能“下一级”的描述字符串（展示升级后技能效果）
FString UAuraProjectileSpell::GetNextLevelDescription(int32 Level)
{
	// 根据等级获取火焰伤害数值
	const int32 Damage = DamageTypes[FAuraGameplayTags::Get().Damage_Fire].GetValueAtLevel(Level);
	//return FString::Printf(TEXT("<Title>Next Level</>\n\n<Default>Launches %d bolts of fire, exploding On impact and dealing: </> <Damage>%d</><Default> fire damage with a chance to burn </>\n\n<Small>Level: </><Level>%d</>"),FMath::Min(Level,NumProjectiles),Damage,Level);
	// 使用 FString::Printf 格式化字符串，拼接技能说明
	return FString::Printf(TEXT(// 技能描述模板（包含标题、技能效果、等级信息，带有自定义标签方便 UI 渲染）
		"<Title>下一级</>\n\n"// 标题（显示“下一级”）
		"<Default>发射 %d 个火球, 技能效果: </>"// 说明火球数量（受 NumProjectiles 限制）
		" <Damage>%d</><Default> 火焰伤害，有几率燃烧 </>\n\n"// 显示伤害数值和附加效果
		"<Small>等级: </>" // 小字提示
		"<Level>%d</>"),// 显示技能等级	
		FMath::Min(Level,NumProjectiles),// 火球数量：取等级和最大数量的最小值
		Damage,// 火焰伤害数值
		Level);// 技能等级
}

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


	//UKismetSystemLibrary::PrintString(this,FString("ActivateAbility (C++)"),true,true,FLinearColor::Yellow,3);//界面显示C++调试
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation,const FGameplayTag& SocketTag,bool bOverridePitch,float PitchOverride)
{
	
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();// 判断是否在服务器执行（Actor 生成必须在服务端，否则客户端不同步）
	if (!bIsServer) return;// 客户端直接返回，不生成投射物
	
	const FVector SocketLocation =ICombatInterface::Execute_GetCombatSocketLocation(
		GetAvatarActorFromActorInfo(),
		SocketTag);// 从战斗接口获取发射插槽位置（解耦角色，统一从接口拿位置）
	
	// 计算从发射点指向目标点的旋转角度（让投射物面向目标方向）
	// (目标位置 - 发射位置) 得到方向向量，再用 Rotation() 转成 FRotator
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	if (bOverridePitch)// 如果启用 Pitch 覆盖，就强制把俯仰角设置为指定值 这样可以忽略目标的高度差，用固定角度发射投射物（例如箭矢抛物线）
	{
		Rotation.Pitch = PitchOverride;
	}
		
	FTransform SpawnTransform;// 用于描述生成位置、旋转、缩放的变换数据
	SpawnTransform.SetLocation(SocketLocation);// 设置生成位置（旋转稍后可设置）
	SpawnTransform.SetRotation(Rotation.Quaternion());// 将计算好的朝向角（FRotator）转成四元数，设置到生成变换中
	
		
	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(// 延迟生成一个 AAuraProjectile 类型的 Actor
		ProjectileClass,// 要生成的类（这里是你的投射物类，比如火球）
		SpawnTransform, // 生成的位置信息（FTransform 包含位置、旋转、缩放）
		GetOwningActorFromActorInfo(), // 拥有者（Outer），这里是技能的拥有者 Actor
		Cast<APawn>(GetOwningActorFromActorInfo()), // 生成时的 Instigator（通常是触发攻击的 Pawn）
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn// 冲突处理方式：总是生成，即使和其他物体重叠
		);

	//  Give the Projectile a Gameplay Effect Spec for causing Damage 为射弹提供造成伤害的游戏效果规格
	// 从当前技能 ActorInfo 中获取 Avatar（通常是角色 Pawn/Character），再取得它的 ASC
	const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();// 创建一个 GameplayEffectContextHandle，用来存放技能施放时的上下文信息
	EffectContextHandle.SetAbility(this);// 把当前技能自身绑定到 Context，便于后续追溯来源
	EffectContextHandle.AddSourceObject(Projectile);// 把投射物 Projectile 作为 SourceObject 加入上下文，用来标识效果的来源对象	
	TArray<TWeakObjectPtr<AActor>> Actors;// 添加相关的 Actor（此处是 Projectile），TWeakObjectPtr 确保不会导致强引用内存泄漏
	Actors.Add(Projectile);
	EffectContextHandle.AddActors(Actors);
	FHitResult HitResult;// 构造一次命中结果，用来记录投射物命中的位置
	HitResult.Location = ProjectileTargetLocation;
	EffectContextHandle.AddHitResult(HitResult);// 把命中结果加入 Context，方便伤害计算、表现（例如在命中位置生成特效）
	
	// 创建一个指定类型的 GameplayEffectSpec，用于之后应用到目标
	const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass,GetAbilityLevel(),EffectContextHandle);

	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();// 先获取全局的 GameplayTags（项目里定义的单例，存放所有标签）

	for (auto& Pair : DamageTypes)
	{
		const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());// 根据技能等级，从 Damage（曲线或数值表）中获取对应的伤害数值
		// 给即将应用的 GameplayEffectSpec（SpecHandle）添加一个 “SetByCaller” 类型的数值
		// 参数含义： SpecHandle → 目标 GameplayEffectSpec  Pair.Key   → 当前伤害类型对应的 GameplayTag（如 Damage.Fire / Damage.Ice） ScaledDamage → 本次实际计算出来的伤害值
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,Pair.Key,ScaledDamage);// 2. 把伤害值写进 GameplayEffectSpec 的 SetByCaller
	}
	
	Projectile->DamageEffectSpecHandle = SpecHandle;// 把带有伤害数值的 SpecHandle 交给 Projectile（投射物），
	
	Projectile->FinishSpawning(SpawnTransform);// 完成生成，开始在世界中生效
}
