// 青楼


#include "Character/AuraEnemy.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"

AAuraEnemy::AAuraEnemy()
{
	//设置当前角色的网格（GetMesh()）在 ECC_Visibility 通道上的碰撞响应为 Block（阻挡）
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);

	//创建并初始化一个 UAuraAbilitySystemComponent 子对象的标准方式，常用于构造函数里，比如在一个 ACharacter 派生类中。
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	//用来**开启网络同步（Replicate）**功能的，告诉 Unreal Engine 这个组件（AbilitySystemComponent）需要在服务器与客户端之间同步数据。
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);//设置 AbilitySystemComponent 的效果同步模式

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

void AAuraEnemy::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);//用于设置角色网格体是否启用 Custom Depth 渲染通道。
	//设置网格体的 Custom Depth Stencil 值，用于配合后处理材质进行效果区分（比如不同颜色的描边）。
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);

	Weapon->SetRenderCustomDepth(true);//设置武器
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AAuraEnemy::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);//用于设置角色网格体是否启用 Custom Depth 渲染通道。
	Weapon->SetRenderCustomDepth(false);
}

int32 AAuraEnemy::GetPlayerLevel()
{
	return Level;
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	InitAbilityActorInfo();
}

void AAuraEnemy::InitAbilityActorInfo()
{
	//初始化 AbilitySystemComponent，让它知道谁是“实际拥有者”和“执行者”
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	
}

