// 青楼

#pragma once

#include "CoreMinimal.h"
#include "Checkpoint/Checkpoint.h"
#include "MapEntrance.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AMapEntrance : public ACheckpoint
{
	GENERATED_BODY()

public:
	AMapEntrance(const FObjectInitializer& ObjectInitializer);//地图入口

	/* Highlight Interface */
	virtual void HighlightActor_Implementation() override;//突出演员

	/* end Highlight Interface */

	/* Save Interface */
	virtual void LoadActor_Implementation() override;//加载演员
	/* end Save Interface*/

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> DestinationMap;//目的地地图

	UPROPERTY(EditAnywhere)
	FName DestinationPlayerStartTag;//目标播放器开始标签

protected:
	
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,// [参数1] 当前触发重叠事件的组件（比如这个球体碰撞组件本身）
		AActor* OtherActor,// [参数2] 进入碰撞区域的另一个 Actor（例如玩家角色、NPC、物品等）
		UPrimitiveComponent* OtherComp,// [参数3] 对方 Actor 上参与碰撞的具体组件（例如对方的 CapsuleComponent）
		int32 OtherBodyIndex, // [参数4] 碰撞体索引（主要用于一个 Actor 有多个物理 Body 的情况）	
		bool bFromSweep, // [参数5] 是否是通过 Sweep（移动检测）触发的重叠
		const FHitResult& SweepResult// [参数6] 如果是 Sweep 触发，这里有命中信息（位置、法线等）
		) override;//球体覆盖
};
