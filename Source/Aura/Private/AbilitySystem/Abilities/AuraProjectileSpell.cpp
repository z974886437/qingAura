// 青楼


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

//#include "Kismet/KismetSystemLibrary.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


	//UKismetSystemLibrary::PrintString(this,FString("ActivateAbility (C++)"),true,true,FLinearColor::Yellow,3);//界面显示C++调试
}

void UAuraProjectileSpell::SpawnProjectile()
{
	
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();//判断当前逻辑是否在服务器端执行
	if (!bIsServer) return;

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (CombatInterface)
	{
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
		
		FTransform SpawnTransform;//生成变化
		SpawnTransform.SetLocation(SocketLocation);
		//Set the Projectile Rotation 设置弹丸旋转
		
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(// 延迟生成一个 AAuraProjectile 类型的 Actor
			ProjectileClass,// 要生成的类（这里是你的投射物类，比如火球）
			SpawnTransform, // 生成的位置信息（FTransform 包含位置、旋转、缩放）
			GetOwningActorFromActorInfo(), // 拥有者（Outer），这里是技能的拥有者 Actor
			Cast<APawn>(GetOwningActorFromActorInfo()), // 生成时的 Instigator（通常是触发攻击的 Pawn）
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn// 冲突处理方式：总是生成，即使和其他物体重叠
			);

		// Give the Projectile a Gameplay Effect Spec for causing Damage 为射弹提供造成伤害的游戏效果规格
		
		
		Projectile->FinishSpawning(SpawnTransform);
	}
}
