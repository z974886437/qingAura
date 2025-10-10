// 青楼


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// 返回火球术技能的描述字符串（根据等级显示不同效果）
FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);// 根据技能等级获取火焰伤害数值
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)// 如果是等级 1，显示单发火球描述
	{
		return FString::Printf(TEXT(
			/* Title 标题*/
			"<Title>火球术</>\n\n"

			// Level
			"<Small>等级: </><Level>%d</>\n"
			//ManaCost
			"<Small>法力消耗: </><ManaCost>%.1f</>\n"
			//Cooldown
			"<Small>冷却： </><Cooldown>%.1f</>\n\n"

			//Number of FireBolts
			"<Default>发射 1 个火球, 技能效果: </>"
			//Damage
			"<Damage>%d</><Default> 火焰伤害，有几率燃烧 </>"),

			//Values
			Level,// 替换 %d 为等级
			ManaCost,
			Cooldown,
			ScaledDamage// 替换 %d 为伤害数值
			);
	}
	else// 如果是等级 > 1，显示多发火球描述
	{
		return FString::Printf(TEXT(
			/* Title 标题*/
			"<Title>火球术</>\n\n"

			// Level
			"<Small>等级: </><Level>%d</>\n"
			//ManaCost
			"<Small>法力消耗: </><ManaCost>%.1f</>\n"
			//Cooldown
			"<Small>冷却： </><Cooldown>%.1f</>\n\n"

			//Number of FireBolts
			"<Default>发射 %d 个火球, 技能效果: </>"

			//Damage
			"<Damage>%d</><Default> 火焰伤害，有几率燃烧 </>"),

			//Values
			Level,// 替换 %d 为等级
			ManaCost,
			Cooldown,
			FMath::Min(Level,NumProjectiles),
			ScaledDamage// 替换 %d 为伤害数值
			);
	}
}

// 返回技能“下一级”的描述字符串（展示升级后技能效果）
FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);// 根据技能等级获取火焰伤害数值
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
			/* Title 标题*/
			"<Title>下一级</>\n\n"

			// Level
			"<Small>等级: </><Level>%d</>\n"
			//ManaCost
			"<Small>法力消耗: </><ManaCost>%.1f</>\n"
			//Cooldown
			"<Small>冷却： </><Cooldown>%.1f</>\n\n"

			//Number of FireBolts
			"<Default>发射 %d 个火球, 技能效果: </>"

			//Damage
			"<Damage>%d</><Default> 火焰伤害，有几率燃烧 </>"),

			//Values
			Level,// 替换 %d 为等级
			ManaCost,
			Cooldown,
			FMath::Min(Level,NumProjectiles),
			ScaledDamage// 替换 %d 为伤害数值
			);
	
}

// 函数：生成多个投射物
void UAuraFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag,bool bOverridePitch, float PitchOverride, AActor* HomingTarget)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();// 判断是否在服务器执行（Actor 生成必须在服务端，否则客户端不同步）
	if (!bIsServer) return;// 客户端直接返回，不生成投射物

	// 从战斗接口获取发射位置（比如角色的武器、手臂等插槽位置）
	const FVector SocketLocation =ICombatInterface::Execute_GetCombatSocketLocation(
		GetAvatarActorFromActorInfo(),
		SocketTag);// 从战斗接口获取发射插槽位置（解耦角色，统一从接口拿位置）
	
	// 计算从发射点到目标位置的旋转角度
	// 通过目标位置和发射点的相对位置来计算方向向量，然后将其转换为旋转角度
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	if (bOverridePitch) Rotation.Pitch = PitchOverride;// 如果启用 Pitch 覆盖，就强制把俯仰角设置为指定值 这样可以忽略目标的高度差，用固定角度发射投射物（例如箭矢抛物线）

	const FVector Forward = Rotation.Vector();// 获取发射物的前进方向向量（从旋转角度获取方向，通常用于表示物体朝向的方向）

	const int32 EffectiveNumProjectiles = FMath::Min(NumProjectiles,GetAbilityLevel());// 计算有效的投射物数量，并根据技能等级进行调整
	// 使用给定的旋转方向、扩散角度以及投射物数量计算出多个旋转角度
	TArray<FRotator> Rotations = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward,FVector::UpVector,ProjectileSpread,EffectiveNumProjectiles);
	
	for (const FRotator& Rot : Rotations)// 遍历计算出来的每个旋转角度，为每个角度生成一个投射物
	{
		FTransform SpawnTransform;// 用于描述生成位置、旋转、缩放的变换数据
		SpawnTransform.SetLocation(SocketLocation);// 设置生成位置（旋转稍后可设置）
		SpawnTransform.SetRotation(Rot.Quaternion());// 将计算好的朝向角（FRotator）转成四元数，设置到生成变换中
	
		
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(// 延迟生成一个 AAuraProjectile 类型的 Actor
			ProjectileClass,// 要生成的类（这里是你的投射物类，比如火球）
			SpawnTransform, // 生成的位置信息（FTransform 包含位置、旋转、缩放）
			GetOwningActorFromActorInfo(), // 拥有者（Outer），这里是技能的拥有者 Actor
			Cast<APawn>(GetOwningActorFromActorInfo()), // 生成时的 Instigator（通常是触发攻击的 Pawn）
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn// 冲突处理方式：总是生成，即使和其他物体重叠
			);
	
		Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();// 设置投射物的伤害效果参数

		if (HomingTarget && HomingTarget->Implements<UCombatInterface>())// 如果有跟踪目标，则设置投射物的跟踪目标组件
		{
			Projectile->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
		}
		else
		{
			// 如果没有跟踪目标，创建一个新的场景组件，将目标位置设置为投射物目标位置
			Projectile->HomingTargetSceneComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
			Projectile->HomingTargetSceneComponent->SetWorldLocation(ProjectileTargetLocation);
			Projectile->ProjectileMovement->HomingTargetComponent = Projectile->HomingTargetSceneComponent;
		}
		Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::FRandRange(HomingAccelerationMin,HomingAccelerationMax);// 设置跟踪目标的加速度范围，随机选择一个值
		Projectile->ProjectileMovement->bIsHomingProjectile = bLaunchHomingProjectiles;// 设置投射物为跟踪型投射物
	
		Projectile->FinishSpawning(SpawnTransform);// 完成生成，开始在世界中生效
	}
}
