// 青楼


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

FString UAuraBeamSpell::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);// 根据技能等级获取火焰伤害数值
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)// 如果是等级 1，显示单发火球描述
	{
		return FString::Printf(TEXT(
			/* Title 标题*/
			"<Title>闪电链</>\n\n"

			// Level
			"<Small>等级: </><Level>%d</>\n"
			//ManaCost
			"<Small>法力消耗: </><ManaCost>%.1f</>\n"
			//Cooldown
			"<Small>冷却： </><Cooldown>%.1f</>\n\n"

			//Number of FireBolts
			"<Default>发射 1 个闪电链, 技能效果: </>"
			//Damage
			"<Damage>%d</><Default> 闪电伤害，有几率眩晕 </>"),

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
			"<Title>闪电链</>\n\n"

			// Level
			"<Small>等级: </><Level>%d</>\n"
			//ManaCost
			"<Small>法力消耗: </><ManaCost>%.1f</>\n"
			//Cooldown
			"<Small>冷却： </><Cooldown>%.1f</>\n\n"

			//Number of FireBolts
			"<Default>发射 %d 个闪电链, 技能效果: </>"

			//Damage
			"<Damage>%d</><Default> 闪电伤害，有几率眩晕 </>"),

			//Values
			Level,// 替换 %d 为等级
			ManaCost,
			Cooldown,
			FMath::Min(Level,MaxNumShockTargets),
			ScaledDamage// 替换 %d 为伤害数值
			);
	}
}

FString UAuraBeamSpell::GetNextLevelDescription(int32 Level)
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
			"<Default>发射 %d 个闪电链, 技能效果: </>"

			//Damage
			"<Damage>%d</><Default> 闪电伤害，有几率眩晕 </>"),

			//Values
			Level,// 替换 %d 为等级
			ManaCost,
			Cooldown,
			FMath::Min(Level,MaxNumShockTargets),
			ScaledDamage// 替换 %d 为伤害数值
			);
}

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)// 判断鼠标检测结果是否命中了可阻挡的对象（例如地面、角色、障碍物）
	{
		MouseHitLocation = HitResult.ImpactPoint;// 保存鼠标命中的世界坐标（用于生成光束的终点位置）
		MouseHitActor = HitResult.GetActor();// 保存被鼠标命中的 Actor（可能是敌人、NPC 或环境物体）
	}
	else
	{
		// 如果鼠标没有命中任何可用对象（空中或地面不可点击区域）
		// 立即取消当前技能
		CancelAbility(
			CurrentSpecHandle,// 当前技能实例句柄
			CurrentActorInfo,// 技能所属角色信息
			CurrentActivationInfo,// 技能激活状态信息
			true// bReplicateCancel：是否广播取消给服务器/客户端
			);
	}
}

void UAuraBeamSpell::StoreOwnerVariables()
{
	// 判断当前技能是否有有效的激活信息（CurrentActorInfo）
	// CurrentActorInfo 是 GAS 提供的结构体，包含了技能施放者的各种引用信息（Pawn、Controller、ASC等）
	if (CurrentActorInfo)
	{
		// 从 CurrentActorInfo 中取出 PlayerController（是一个 TWeakObjectPtr）
		// 使用 Get() 获取其原始指针并保存到 OwnerPlayerController
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	check(OwnerCharacter);// 检查 OwnerCharacter 是否有效，否则在调试模式下会触发断言，防止空指针错误
	if (OwnerCharacter->Implements<UCombatInterface>())// 判断该角色是否实现了战斗接口（确保可以调用 GetWeapon 等接口函数）
	{
		if (USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharacter))// 从战斗接口获取角色当前装备的武器 SkeletalMeshComponent
		{
			TArray<AActor*> ActorsToIgnore;// 创建一个忽略列表，防止检测到自己
			ActorsToIgnore.Add(OwnerCharacter); // 把施法者加入忽略对象（避免光束打到自己）
			
			FHitResult HitResult;// 保存射线检测结果（包括命中的 Actor、位置、法线等）
			
			const FVector SocketLocation = Weapon->GetSocketLocation(FName("TipSocket"));// 获取武器上“TipSocket”插槽的位置（即武器前端或魔杖尖端）
			
			UKismetSystemLibrary::SphereTraceSingle(// 执行球体追踪（SphereTrace），检测从武器尖端到目标位置之间的物体
				OwnerCharacter,// 上下文对象（用于世界访问）
				SocketLocation,// 起点（武器尖端）
				BeamTargetLocation,// 终点（玩家点击或瞄准的位置）
				10.f,// 球体半径（光束宽度）
				TraceTypeQuery1, // 追踪通道（用于过滤碰撞类型）
				false, // 是否复杂追踪（false 表示用简单碰撞体）
				ActorsToIgnore,// 忽略的对象（避免误检测自己）
				EDrawDebugTrace::None,// 调试绘制类型
				HitResult,// 输出参数：命中的结果		
				true// 命中时是否返回物理材质
				);

			if (HitResult.bBlockingHit)// 如果检测到阻挡物（即光束命中目标）
			{
				MouseHitLocation = HitResult.ImpactPoint;// 保存命中点（用于生成光束或特效位置）
				MouseHitActor = HitResult.GetActor();// 保存命中的目标 Actor（用于施加伤害或锁定）
			}
		}
	}
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MouseHitActor))// 如果鼠标指向的 Actor 实现了战斗接口（即可以作为战斗目标）
	{
		if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this,&UAuraBeamSpell::PrimaryTargetDied))// 如果该目标的死亡委托中，还没有绑定本技能的“PrimaryTargetDied”函数
		{
			CombatInterface->GetOnDeathDelegate().AddDynamic(this,&UAuraBeamSpell::PrimaryTargetDied);// 动态绑定目标死亡事件，当目标死亡时自动调用“PrimaryTargetDied”
		}
	}
}

// 功能：在“光束法术（BeamSpell）”中，基于第一个目标（MouseHitActor），寻找周围最近的额外目标，并存入输出数组
void UAuraBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets)
{
	TArray<AActor*> ActorsToIgnore;// 创建一个“忽略列表”，防止射线或范围检测时命中自己或主要目标
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());// 忽略施法者（自己）
	ActorsToIgnore.Add(MouseHitActor);// 忽略第一个命中的目标（主目标）
	
	TArray<AActor*> OverlappingActors;// 用于存放范围内检测到的所有其他可攻击玩家

	// 调用自定义工具函数，在指定半径范围内搜索“活着的玩家”
	UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
		GetAvatarActorFromActorInfo(),// 搜索时的施法者（用于区分敌友或队伍）
		OverlappingActors,// 输出找到的活着的玩家
		ActorsToIgnore,// 搜索时忽略的对象
		850.f,// 搜索半径（850单位）
		MouseHitActor->GetActorLocation() // 以第一个目标为中心进行范围检测
		);

	int32 NumAdditionalTargets = FMath::Min(GetAbilityLevel() - 1,MaxNumShockTargets);
	// 指定要寻找的额外目标数量
	// 原逻辑：最多为“技能等级 - 1”，并受 MaxNumShockTargets 限制
	// int32 NumAdditionalTargets = FMath::Min(GetAbilityLevel() - 1, MaxNumShockTargets);
	// 临时改为固定 5 个目标，方便测试
	//int32 NumAdditionalTargets = 5;

	// 从范围内的存活玩家中，筛选出距离主目标最近的若干个，作为最终的附加目标
	UAuraAbilitySystemLibrary::GetClosestTargets(
		NumAdditionalTargets,// 需要的目标数量
		OverlappingActors,// 所有可选目标
		OutAdditionalTargets,// 输出数组，保存最近目标
		MouseHitActor->GetActorLocation()// 距离计算中心（第一个命中的目标）
		);

	for (AActor* Target : OutAdditionalTargets)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Target))
		{
			if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this,&UAuraBeamSpell::AdditionalTargetDied))// 如果该目标的死亡委托中，还没有绑定本技能的“AdditionalTargetDied”函数
			{
				CombatInterface->GetOnDeathDelegate().AddDynamic(this,&UAuraBeamSpell::AdditionalTargetDied);// 动态绑定目标死亡事件，当目标死亡时自动调用“PrimaryTargetDied”
			}
		}
	}
	
}
