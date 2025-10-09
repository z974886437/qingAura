// 青楼


#include "AbilitySystem/Abilities/AuraFireBolt.h"

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

void UAuraFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag,bool bOverridePitch, float PitchOverride, AActor* HomingTarget)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();// 判断是否在服务器执行（Actor 生成必须在服务端，否则客户端不同步）
	if (!bIsServer) return;// 客户端直接返回，不生成投射物
	
	const FVector SocketLocation =ICombatInterface::Execute_GetCombatSocketLocation(
		GetAvatarActorFromActorInfo(),
		SocketTag);// 从战斗接口获取发射插槽位置（解耦角色，统一从接口拿位置）
	
	// 计算从发射点指向目标点的旋转角度（让投射物面向目标方向）
	// (目标位置 - 发射位置) 得到方向向量，再用 Rotation() 转成 FRotator
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	if (bOverridePitch) Rotation.Pitch = PitchOverride;// 如果启用 Pitch 覆盖，就强制把俯仰角设置为指定值 这样可以忽略目标的高度差，用固定角度发射投射物（例如箭矢抛物线）

	const FVector Forward = Rotation.Vector();// 获取发射物的前进方向向量（从旋转角度获取方向，通常用于表示物体朝向的方向）
	
	// 根据旋转角度，沿 Y 轴（即世界坐标中的 Up 方向）旋转指定角度，计算出偏移位置。
	// 这里的 `-ProjectileSpread / 2.f` 表示将投射物扩散角度的一半应用于左侧（负值表示顺时针旋转）。
	// 结果是发射物在一定范围内产生的扩散效果的一部分，通常用于模拟扇形散射效果。
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-ProjectileSpread / 2.f,FVector::UpVector);
	const FVector RightOfSpread = Forward.RotateAngleAxis(ProjectileSpread / 2.f,FVector::UpVector);
	
	//NumProjectiles = FMath::Min(MaxNumProjectiles,GetAbilityLevel());
	if (NumProjectiles > 1)// 如果有多个投射物
	{
		const float DeltaSpread = ProjectileSpread / (NumProjectiles - 1);// 计算每个投射物之间的扩散角度
		for (int32 i = 0; i < NumProjectiles;i++)// 遍历每个投射物
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i,FVector::UpVector);// 根据扩散角度旋转，确定每个投射物的发射方向
			const FVector Start = SocketLocation + FVector(0,0,5);// 设置投射物的起始位置，稍微抬高一点以防与地面重叠
			//绘制Debuff箭头
			UKismetSystemLibrary::DrawDebugArrow(
				GetAvatarActorFromActorInfo(),// 获取施法者的 Actor，这里是绘制箭头的上下文（通常是发射技能的角色）
				Start,// 箭头的起点（发射点位置）
				Start + Direction * 75.f,// 箭头的终点，通过计算发射方向并将其放大100倍来确定箭头的方向
				1,// 箭头的线宽（5代表较粗的箭头）
				FLinearColor::Red,// 箭头的颜色，这里设置为绿色
				120,// 箭头持续的时间（单位为秒，120秒，代表长时间可见）
				1// 箭头的生命周期（时间越短，箭头越快消失）
				);
		}
	}
	else
	{
		//Single projectile 单枚射弹
		const FVector Start = SocketLocation + FVector(0,0,5);// 设置投射物的起始位置，稍微抬高一点以防与地面重叠
		UKismetSystemLibrary::DrawDebugArrow(
				GetAvatarActorFromActorInfo(),// 获取施法者的 Actor，这里是绘制箭头的上下文（通常是发射技能的角色）
				Start,// 箭头的起点（发射点位置）
				Start + Forward * 75.f,// 箭头的终点，通过计算发射方向并将其放大100倍来确定箭头的方向
				1,// 箭头的线宽（5代表较粗的箭头）
				FLinearColor::Red,// 箭头的颜色，这里设置为绿色
				120,// 箭头持续的时间（单位为秒，120秒，代表长时间可见）
				1// 箭头的生命周期（时间越短，箭头越快消失）
				);
	}

	//绘制Debuff箭头
	UKismetSystemLibrary::DrawDebugArrow(
		GetAvatarActorFromActorInfo(),// 获取施法者的 Actor，这里是绘制箭头的上下文（通常是发射技能的角色）
		SocketLocation,// 箭头的起点（发射点位置）
		SocketLocation + Forward * 100.f,// 箭头的终点，通过计算发射方向并将其放大100倍来确定箭头的方向
		1,// 箭头的线宽（5代表较粗的箭头）
		FLinearColor::White,// 箭头的颜色，这里设置为绿色
		120,// 箭头持续的时间（单位为秒，120秒，代表长时间可见）
		2// 箭头的生命周期（时间越短，箭头越快消失）
		);

	//绘制Debuff箭头
	UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(),SocketLocation,SocketLocation + LeftOfSpread * 100.f,1,FLinearColor::Gray,120,1);
	UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(),SocketLocation,SocketLocation + RightOfSpread * 100.f,1,FLinearColor::Gray,120,1);
}
