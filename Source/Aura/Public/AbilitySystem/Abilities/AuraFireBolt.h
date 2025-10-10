// 青楼

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "AuraFireBolt.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraFireBolt : public UAuraProjectileSpell
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;//获取描述
	virtual FString GetNextLevelDescription(int32 Level) override;//获取下一级描述

	UFUNCTION(BlueprintCallable)
	void SpawnProjectiles(const FVector& ProjectileTargetLocation,const FGameplayTag& SocketTag,bool bOverridePitch,float PitchOverride,AActor* HomingTarget);//生成射弹

protected:

	UPROPERTY(EditDefaultsOnly,Category = "FireBolt")
	float ProjectileSpread = 90.f;//射弹扩散

	UPROPERTY(EditDefaultsOnly,Category = "FireBolt")
	int32 MaxNumProjectiles = 5;//射弹扩散

	UPROPERTY(EditDefaultsOnly,Category = "FireBolt")
	float HomingAccelerationMin = 1600.f;//归位加速度最小值

	UPROPERTY(EditDefaultsOnly,Category = "FireBolt")
	float HomingAccelerationMax = 3200.f;//归位加速度最大值

	UPROPERTY(EditDefaultsOnly,Category = "FireBolt")
	bool bLaunchHomingProjectiles = true;//b 发射寻的射弹
};
