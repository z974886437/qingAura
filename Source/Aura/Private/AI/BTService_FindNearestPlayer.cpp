// 青楼


#include "AI/BTService_FindNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);// 调用父类 TickNode，保证父类逻辑正常执行，这是行为树节点常规做法

	APawn* OwningPawn = AIOwner->GetPawn();// 获取当前 AI 控制器所操控的 Pawn（也就是 AI 自己）

	// 判断自己是否有 "Player" 标签： - 如果自己是 Player，就要寻找 "Enemy"- 如果自己不是 Player，就要寻找 "Player"  用标签区分敌我阵营，这是最简单的阵营识别方法
	const FName TargetTag = OwningPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");

	// 从场景中查找所有带有指定标签的 Actor，并存入数组 ActorsWithTag 注意：这是全局查找，性能开销较大，不建议在 Tick 每帧调用
	TArray<AActor*> ActorsWithTag;
	UGameplayStatics::GetAllActorsWithTag(OwningPawn,TargetTag,ActorsWithTag);

	float ClosestDistance = TNumericLimits<float>::Max();//先把 ClosestDistance 初始化为 float 类型能表示的最大值（大约 3.402823e+38）
	AActor* ClosestActor = nullptr;// 初始化最近的 Actor 为 nullptr
	for (AActor* Actor : ActorsWithTag)// 遍历所有找到的 Actor
	{
		GEngine->AddOnScreenDebugMessage(-1,0.5f,FColor::Orange,*Actor->GetName());

		if (IsValid(Actor) && IsValid(OwningPawn))
		{
			const float Distance = OwningPawn->GetDistanceTo(Actor);// 计算 OwningPawn 到当前 Actor 的距离
			if (Distance < ClosestDistance) // 如果距离小于当前最小距离，则更新
			{
				ClosestDistance = Distance;// 更新最近距离
				ClosestActor = Actor;// 更新最近的 Actor
			}
		}
	}
	// 将最近的 Actor 和距离写入黑板，用于行为树控制 AI
	UBTFunctionLibrary::SetBlackboardValueAsObject(this,TargetToFollowSelector,ClosestActor);
	UBTFunctionLibrary::SetBlackboardValueAsFloat(this,DistanceToTargetSelector,ClosestDistance);
	
}
