// 青楼


#include "Checkpoint/Checkpoint.h"

#include "Components/SphereComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

// 构造函数，使用 FObjectInitializer 初始化对象，用于在对象创建时配置子组件
ACheckpoint::ACheckpoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)// 调用父类构造函数，确保基类的初始化逻辑被执行
{
	PrimaryActorTick.bCanEverTick = false;// 禁用 Tick，每帧不需要更新逻辑，提高性能
	
	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>("CheckpointMesh");// 创建并注册一个静态网格组件，用于显示检查点的视觉外观
	CheckpointMesh->SetupAttachment(GetRootComponent());// 将静态网格组件附加到根组件上（如果没有根组件，将会附加到默认根）
	CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);// 启用查询与物理碰撞（既能检测重叠也能产生物理反应，例如阻挡）
	CheckpointMesh->SetCollisionResponseToAllChannels(ECR_Block);// 设置网格对所有通道的碰撞响应为“阻挡”，即任何物体都会被它挡住

	CheckpointMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_TAN);// 设置自定义深度模板值，用于后期处理效果
	CheckpointMesh->MarkRenderStateDirty();// 标记渲染状态为脏，确保更新渲染状态
	
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");// 创建一个球形碰撞体组件，用来检测角色是否触发检查点
	Sphere->SetupAttachment(CheckpointMesh);// 将球形组件附加到网格组件上，使其跟随网格一起移动
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);//只进行“查询类”碰撞检测，不进行物理模拟
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);//把 Sphere 对所有碰撞通道的响应全部设为 忽略（Ignore）
	Sphere->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);// 对角色（Pawn）启用“重叠”检测，这样投射物可以检测到玩家或敌人。

	MoveToComponent = CreateDefaultSubobject<USceneComponent>("MoveToComponent");// 创建并设置移动目标组件，用于目标位置的存储与设置
	MoveToComponent->SetupAttachment(GetRootComponent());// 将移动目标组件附加到根组件上
}

void ACheckpoint::LoadActor_Implementation()
{
	if (bReached)
	{
		HandleGlowEffects();
	}
}

// 当球形碰撞体检测到有物体进入（重叠）时调用的回调函数
void ACheckpoint::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>()) // 判断进入范围的 Actor 是否带有 “Player” 标签，确保只有玩家触发检查点
	{
		bReached = true;// 标记已触达检查点，用于避免重复触发或用于视觉表现

		// 获取当前 GameMode，并确保转成 AAuraGameModeBase 成功
		if (AAuraGameModeBase* AuraGM = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			AuraGM->SaveWorldState(GetWorld());// 调用 GameMode 保存当前世界状态（保存 Actor 数据、位置等）
		}
		
		IPlayerInterface::Execute_SaveProgress(OtherActor,PlayerStartTag);// 调用接口函数 SaveProgress，将当前检查点的标签（PlayerStartTag）传入
		HandleGlowEffects();   // 触发发光效果（表示检查点被激活）
	}
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	// 将球体组件的重叠事件绑定到自定义函数 OnSphereOverlap
	// 当玩家进入球体范围时，自动调用 OnSphereOverlap 函数
	Sphere->OnComponentBeginOverlap.AddDynamic(this,&ACheckpoint::OnSphereOverlap);//球体碰撞组件（Sphere）的 重叠开始事件 绑定到 AAuraProjectile::OnSphereOverlap 函数
}

// 设置目标位置为当前组件的位置
void ACheckpoint::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	OutDestination = MoveToComponent->GetComponentLocation();// 获取目标组件位置，并将其设置为目标位置
}

// 高亮显示物体
void ACheckpoint::HighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(true);// 设置物体的渲染深度为自定义深度，显示高亮效果
}

// 取消高亮显示物体
void ACheckpoint::UnHighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(false);// 关闭物体的自定义渲染深度，移除高亮效果
}

// 处理检查点发光效果的逻辑函数
void ACheckpoint::HandleGlowEffects()
{
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 禁用球体的碰撞，防止重复触发同一个检查点

	// 创建一个动态材质实例，用于在运行时修改材质参数（如亮度、颜色）
	UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(CheckpointMesh->GetMaterial(0),this);
	CheckpointMesh->SetMaterial(0,DynamicMaterialInstance); // 将动态材质应用到检查点的网格上
	CheckpointReached(DynamicMaterialInstance); // 调用自定义函数（可能是蓝图事件），执行具体发光逻辑或动画
}
