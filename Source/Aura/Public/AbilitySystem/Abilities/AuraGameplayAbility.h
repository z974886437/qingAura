// 青楼

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly,Category = "Input")
	FGameplayTag StartupInputTag;//启动输入标签

	virtual FString GetDescription(int32 Level);//获取描述
	virtual FString GetNextLevelDescription(int32 Level);//获取下一级描述
	static FString GetLockedDescription(int32 Level);//获取锁定描述

protected:

	float GetManaCost(float InLevel = 1.f) const;//获取法力消耗
	float GetCooldown(float InLevel = 1.f) const;//获取冷却
};
