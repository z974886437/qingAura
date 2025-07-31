// 青楼


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	//创建并初始化一个 UAuraAbilitySystemComponent 子对象的标准方式，常用于构造函数里，比如在一个 ACharacter 派生类中。
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	//用来**开启网络同步（Replicate）**功能的，告诉 Unreal Engine 这个组件（AbilitySystemComponent）需要在服务器与客户端之间同步数据。
	AbilitySystemComponent->SetIsReplicated(true);
	//Gameplay Ability System（GAS） 设置 AbilitySystemComponent 的复制模式（Replication Mode） Minimal（最小复制）
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");//属性集
	
	NetUpdateFrequency = 100.f;//该 Actor 每秒发送多少次网络更新（replication updates）给客户端。
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//讲某些内容标记为已复制的最基本的宏
	DOREPLIFETIME(AAuraPlayerState,Level);
	
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
}
