// 青楼


#include "Character/AuraEnemy.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/AuraUserWidget.h"

AAuraEnemy::AAuraEnemy()
{
	//设置当前角色的网格（GetMesh()）在 ECC_Visibility 通道上的碰撞响应为 Block（阻挡）
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);

	//创建并初始化一个 UAuraAbilitySystemComponent 子对象的标准方式，常用于构造函数里，比如在一个 ACharacter 派生类中。
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	//用来**开启网络同步（Replicate）**功能的，告诉 Unreal Engine 这个组件（AbilitySystemComponent）需要在服务器与客户端之间同步数据。
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);//设置 AbilitySystemComponent 的效果同步模式

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");// 创建默认的属性集子对象（管理血量、蓝量、攻击力等 GAS 属性）

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");// 创建默认的 Widget 组件（用来显示血条 UI）
	HealthBar->SetupAttachment(GetRootComponent());// 将血条 UI 挂到角色的根组件上（跟着角色移动）
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
	InitAbilityActorInfo();//初始化能力Actor信息

	if (UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))// 如果血条组件里装的是真正的 Aura UI Widget，就给它设置控制器
	{
		AuraUserWidget->SetWidgetController(this);// 让 Widget 能访问这个控制器，方便绑定数据
	}
	
	if (const UAuraAttributeSet* AuraAS = Cast<UAuraAttributeSet>(AttributeSet))// 把通用 AttributeSet 转成我们自定义的 Aura 属性集
	{
		// 监听 Health 属性变化
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute()).AddLambda(
			[this] (const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue); // 当血量变化时，广播新的血量值
			}
		);
		// 监听 MaxHealth 属性变化
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute()).AddLambda(
			[this] (const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);  // 当最大血量变化时，广播新的最大血量值
			}
		);
		// 初始化 UI 显示（避免进场 UI 没数据）
		OnHealthChanged.Broadcast(AuraAS->GetHealth());
		OnMaxHealthChanged.Broadcast(AuraAS->GetMaxHealth());
	}
}

void AAuraEnemy::InitAbilityActorInfo()
{
	//初始化 AbilitySystemComponent，让它知道谁是“实际拥有者”和“执行者”
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	InitializeDefaultAttributes();//初始化默认属性
}

void AAuraEnemy::InitializeDefaultAttributes() const
{
	// 调用自定义的工具函数，用来初始化角色的默认属性
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this,CharacterClass,Level,AbilitySystemComponent);
}

