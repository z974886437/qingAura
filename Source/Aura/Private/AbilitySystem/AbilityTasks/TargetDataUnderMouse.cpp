// 青楼


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"
#include "Aura/Aura.h"


UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	// 判断当前技能是否由本地玩家控制（返回 true 表示本地控制，false 表示远程控制）
	// 使用 GAS 提供的 ActorInfo 的 IsLocallyControlled() 而不是直接调用 Pawn/Controller 的 IsLocallyControlled()
	// 原因：ActorInfo 会根据当前技能上下文正确识别控制权（无论 AvatarActor 是 Pawn 还是其他对象）
	// 主要用途：区分客户端本地逻辑（UI、特效、相机）与服务器权威逻辑（伤害计算、状态同步），避免误在服务器执行客户端特效
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		SendMouseCursorData();// 如果是本地控制角色，则立即发送鼠标光标的目标数据给服务器
	}
	else
	{
		// We are on the server,so listen for target data 我们在服务器端，需要等待并监听来自客户端的目标数据
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();// 获取当前技能实例的唯一标识
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();// 获取技能激活预测键（保证多客户端同步时的数据对应正确）
		// 注册监听器：当客户端发来的目标数据到达时，调用 OnTargetDataReplicatedCallback
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle,ActivationPredictionKey).AddUObject(this,&UTargetDataUnderMouse::OnTargetDataReplicatedCallback);
		// 如果目标数据已经提前到达（可能网络延迟很低），则立即调用委托
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle,ActivationPredictionKey);
		if (!bCalledDelegate)// 如果目标数据还没到（客户端还没传），则进入等待状态
		{
			SetWaitingOnRemotePlayerData();// 标记任务正在等待远程玩家数据
		}
	}
	
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
	// 创建一个作用域预测窗口对象，关联到当前的 AbilitySystemComponent，用于启用客户端预测机制
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
	
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();// 从当前技能(Ability)的 ActorInfo 获取 PlayerController 指针
	FHitResult CursorHit;// 定义一个命中结果变量，用来保存鼠标光标射线检测到的结果
	PC->GetHitResultUnderCursor(ECC_Target,false,CursorHit);// 从鼠标光标位置向世界发射一条射线，检测是否击中可见对象

	FGameplayAbilityTargetDataHandle DataHandle;// 定义一个技能目标数据句柄，用于管理目标数据集合
	// 创建一个新的 FGameplayAbilityTargetData_SingleTargetHit 实例，用于存储单个目标的命中数据
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;// 把检测到的命中结果赋值给目标数据
	DataHandle.Add(Data);// 将目标数据添加到数据句柄中，便于技能系统统一管理多个目标数据

	// 服务器端设置带有预测键的技能目标数据，确保网络同步与预测正确
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),// 技能规格句柄，标识当前技能实例
		GetActivationPredictionKey(),// 预测键，保证客户端和服务器预测一致
		DataHandle,// 目标数据句柄，包含刚才创建的命中目标数据
		FGameplayTag(), // 额外标签（这里为空）
		AbilitySystemComponent->ScopedPredictionKey// 作用域预测键，进一步确保预测上下文
		);
	
	if (ShouldBroadcastAbilityTaskDelegates())// 判断当前技能任务是否需要广播委托事件
	{
		ValidData.Broadcast(DataHandle);// 触发 ValidData 广播事件，将技能目标数据句柄 DataHandle 传递给所有监听者
	}
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,FGameplayTag ActivationTag)
{
	// 消费（清除）客户端已复制到服务器的目标数据，防止重复使用
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(),GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())// 判断当前技能任务是否需要广播委托事件
	{
		ValidData.Broadcast(DataHandle);// 触发 ValidData 广播事件，将技能目标数据句柄 DataHandle 传递给所有监听者
	}
}
