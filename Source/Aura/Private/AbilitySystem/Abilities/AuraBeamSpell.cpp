// 青楼


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

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
}
