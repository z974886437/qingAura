// 青楼

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraFireBlast.generated.h"

class AAuraFireBall;
/**
 * 
 */
UCLASS()
class AURA_API UAuraFireBlast : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
public:
	virtual FString GetDescription(int32 Level) override;//获取描述
	virtual FString GetNextLevelDescription(int32 Level) override;//获取下一级描述

	UFUNCTION(BlueprintCallable)
	TArray<AAuraFireBall*> SpawnFireBalls();//生成火球
protected:

	UPROPERTY(EditDefaultsOnly,Category = "FireBlast")
	int32 NumFireBalls = 12;//火球数量

private:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAuraFireBall> FireBallClass;//火球类
};
