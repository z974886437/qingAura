// 青楼

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraSummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocations();//获取生成位置

	UPROPERTY(EditDefaultsOnly,Category = "Summoning")
	int32 NumMinions = 5;//仆从数

	UPROPERTY(EditDefaultsOnly,Category = "Summoning")
	TArray<TSubclassOf<APawn>> MinionClasses;//仆从类

	UPROPERTY(EditDefaultsOnly,Category = "Summoning")
	float MinSpawnDistance = 50.f;//最小生成距离

	UPROPERTY(EditDefaultsOnly,Category = "Summoning")
	float MaxSpawnDistance = 250.f;//最大生成距离

	UPROPERTY(EditDefaultsOnly,Category = "Summoning")
	float SpawnSpread = 90.f;//生成范围
};
