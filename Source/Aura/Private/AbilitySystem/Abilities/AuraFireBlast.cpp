// 青楼


#include "AbilitySystem/Abilities/AuraFireBlast.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraFireBall.h"

FString UAuraFireBlast::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);// 根据技能等级获取火焰伤害数值
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
			/* Title 标题*/
			"<Title>火焰冲击波</>\n\n"

			// Level
			"<Small>等级: </><Level>%d</>\n"
			//ManaCost
			"<Small>法力消耗: </><ManaCost>%.1f</>\n"
			//Cooldown
			"<Small>冷却： </><Cooldown>%.1f</>\n\n"

			//Number of FireBolts
			"<Default>发动 %d </>"
			"<Default>向各个方向发射火球，每个火球都会回来</>"/*fire balls in all directions, each coming back and*/
			"<Default>返回时爆炸，导致 </>"/*exploding upon return, causing */

			
			//Damage
			"<Damage>%d</><Default> 火焰伤害，有几率燃烧 </>"),

			//Values
			Level,// 替换 %d 为等级
			ManaCost,
			Cooldown,
			NumFireBalls,
			ScaledDamage// 替换 %d 为伤害数值
			);
}

FString UAuraFireBlast::GetNextLevelDescription(int32 Level)
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
			"<Default>发动 %d </>"
			"<Default>向各个方向发射火球，每个火球都会回来</>"/*fire balls in all directions, each coming back and*/
			"<Default>返回时爆炸，导致 </>"/*exploding upon return, causing */

			
			//Damage
			"<Damage>%d</><Default> 火焰伤害，有几率燃烧 </>"),

			//Values
			Level,// 替换 %d 为等级
			ManaCost,
			Cooldown,
			NumFireBalls,
			ScaledDamage// 替换 %d 为伤害数值
			);
}

// 生成多个火球并返回包含它们的数组
TArray<AAuraFireBall*> UAuraFireBlast::SpawnFireBalls()
{
	TArray<AAuraFireBall*> FireBalls;// 定义一个数组，用于存储生成的火球
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector(); // 获取施法者的前方方向向量
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();// 获取施法者的位置
	
	// 使用自定义的库函数计算一组均匀分布的旋转值（围绕施法者的前方方向旋转）
	// 旋转会生成 `NumFireBalls` 个火球
	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward,FVector::UpVector,360.f,NumFireBalls);

	for (const FRotator& Rotator : Rotators) // 遍历每一个计算得到的旋转角度
	{
		FTransform SpawnTransform;// 创建一个变换，用于设置火球的生成位置和旋转
		SpawnTransform.SetLocation(Location);// 设置位置
		SpawnTransform.SetRotation(Rotator.Quaternion());// 设置旋转
		
		// 延迟生成火球（用于支持稍后设置一些属性）
		AAuraFireBall* FireBall = GetWorld()->SpawnActorDeferred<AAuraFireBall>(
			FireBallClass,// 火球的类（通常是一个蓝图）
			SpawnTransform,// 生成位置和旋转
			GetOwningActorFromActorInfo(),// 获取施法者
			CurrentActorInfo->PlayerController->GetPawn(),// 获取玩家控制的角色
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn // 无论碰撞如何都生成
			);

		FireBall->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();// 设置火球的伤害效果参数
		FireBall->ReturnToActor = GetAvatarActorFromActorInfo();// 设置返回的目标角色（施法者）

		FireBall->ExplosionDamageParams = MakeDamageEffectParamsFromClassDefaults();
		FireBall->SetOwner(GetAvatarActorFromActorInfo());

		FireBalls.Add(FireBall);// 将生成的火球添加到火球数组中

		FireBall->FinishSpawning(SpawnTransform);// 完成火球的生成，并使其在世界中生效
	}
	
	return FireBalls;// 返回生成的火球数组
}
