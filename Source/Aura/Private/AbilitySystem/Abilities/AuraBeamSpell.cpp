// 青楼


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "GameFramework/Character.h"

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
