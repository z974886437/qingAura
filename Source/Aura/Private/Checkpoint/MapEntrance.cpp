// 青楼


#include "Checkpoint/MapEntrance.h"

#include "Components/SphereComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

// AMapEntrance 的构造函数，用于初始化组件和设置附加关系
AMapEntrance::AMapEntrance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)// 调用父类构造函数，确保基类的初始化逻辑被执行
{
	Sphere->SetupAttachment(MoveToComponent);// 将 Sphere 组件附加到 MoveToComponent 上，确保两者的位置关系同步
}

void AMapEntrance::HighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(true);// 设置物体的渲染深度为自定义深度，显示高亮效果
}

void AMapEntrance::LoadActor_Implementation()
{
	// Do nothing when loading a Map Entrance 加载地图入口时不执行任何操作
}

// 触发碰撞体的重叠事件，处理玩家进入检查点时的行为
void AMapEntrance::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>()) // 判断进入范围的 Actor 是否带有 “Player” 标签，确保只有玩家触发检查点
	{
		bReached = true;// 标记已触达检查点，用于避免重复触发或用于视觉表现

		// 获取当前 GameMode，并确保转成 AAuraGameModeBase 成功
		if (AAuraGameModeBase* AuraGM = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			AuraGM->SaveWorldState(GetWorld(),DestinationMap.ToSoftObjectPath().GetAssetName());// 调用 GameMode 保存当前世界状态（保存 Actor 数据、位置等）
		}
		
		IPlayerInterface::Execute_SaveProgress(OtherActor,DestinationPlayerStartTag);// 调用接口函数 SaveProgress，将当前检查点的标签（DestinationPlayerStartTag）传入

		UGameplayStatics::OpenLevelBySoftObjectPtr(this,DestinationMap);// 打开目标关卡（DestinationMap）
	}
}
