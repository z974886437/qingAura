// 青楼


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "Aura/Public/AuraGameplayTags.h"

//#include "Kismet/KismetSystemLibrary.h"



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
		Rotation.Pitch = PitchOverride;// 这样可以忽略目标的高度差，用固定角度发射投射物（例如箭矢抛物线）
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
	
	Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();// 设置投射物的伤害效果参数
	
	Projectile->FinishSpawning(SpawnTransform);// 完成生成，开始在世界中生效
}
