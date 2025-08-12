// 青楼


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"


UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();// 从当前技能(Ability)的 ActorInfo 获取 PlayerController 指针
	FHitResult CursorHit;// 定义一个命中结果变量，用来保存鼠标光标射线检测到的结果
	PC->GetHitResultUnderCursor(ECC_Visibility,false,CursorHit);// 从鼠标光标位置向世界发射一条射线，检测是否击中可见对象
	ValidData.Broadcast(CursorHit.Location);// 广播命中位置到所有绑定的回调函数
}
