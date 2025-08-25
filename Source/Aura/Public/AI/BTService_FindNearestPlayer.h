// 青楼

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "BTService_FindNearestPlayer.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UBTService_FindNearestPlayer : public UBTService_BlueprintBase
{
	GENERATED_BODY()

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory,float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly,EditAnywhere)
	FBlackboardKeySelector TargetToFollowSelector;//目标跟随选择器

	UPROPERTY(BlueprintReadOnly,EditAnywhere)
	FBlackboardKeySelector DistanceToTargetSelector;//到目标选择器的距离 
};

