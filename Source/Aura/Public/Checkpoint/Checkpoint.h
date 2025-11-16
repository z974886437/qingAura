// 青楼

#pragma once

#include "CoreMinimal.h"
#include "Aura/Aura.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/HighlightInterface.h"
#include "Interaction/SaveInterface.h"
#include "Checkpoint.generated.h"

class USphereComponent;
/**
 * 
 */
UCLASS()
class AURA_API ACheckpoint : public APlayerStart, public ISaveInterface, public IHighlightInterface
{
	GENERATED_BODY()

public:
	ACheckpoint(const FObjectInitializer& ObjectInitializer);//一个检查站

	/* Save Interface */
	virtual bool ShouldLoadTransform_Implementation() override { return false; }//应该加载变换
	virtual void LoadActor_Implementation() override;//加载演员
	/* end Save Interface*/

	UPROPERTY(BlueprintReadWrite, SaveGame)
	bool bReached = false;//达到

	UPROPERTY(EditAnywhere)
	bool bBindOverlapCallback = true;//b 绑定重叠回调
protected:

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,// [参数1] 当前触发重叠事件的组件（比如这个球体碰撞组件本身）
		AActor* OtherActor,// [参数2] 进入碰撞区域的另一个 Actor（例如玩家角色、NPC、物品等）
		UPrimitiveComponent* OtherComp,// [参数3] 对方 Actor 上参与碰撞的具体组件（例如对方的 CapsuleComponent）
		int32 OtherBodyIndex, // [参数4] 碰撞体索引（主要用于一个 Actor 有多个物理 Body 的情况）	
		bool bFromSweep, // [参数5] 是否是通过 Sweep（移动检测）触发的重叠
		const FHitResult& SweepResult// [参数6] 如果是 Sweep 触发，这里有命中信息（位置、法线等）
		);//球体覆盖

	virtual void BeginPlay() override;

	/* Highlight Interface */
	virtual void SetMoveToLocation_Implementation(FVector& OutDestination) override;//设置移动到位置
	virtual void HighlightActor_Implementation() override;//突出演员
	virtual void UnHighlightActor_Implementation() override;//取消突出演员

	/* end Highlight Interface */

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> MoveToComponent;//移动到组件

	UPROPERTY(EditDefaultsOnly)
	int32 CustomDepthStencilOverride = CUSTOM_DEPTH_TAN;//自定义深度模板覆盖

	UFUNCTION(BlueprintImplementableEvent)
	void CheckpointReached(UMaterialInstanceDynamic* DynamicMaterialInstance);//已到达检查点

	UFUNCTION(BlueprintCallable)
	void HandleGlowEffects();//处理发光效果

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> CheckpointMesh;//检查点网格
	
private:
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
	
};
