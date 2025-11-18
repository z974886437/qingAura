// 青楼

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LootTiers.generated.h"

USTRUCT(BlueprintType)
struct FLootItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "LootTiers|Spawning")
	TSubclassOf<AActor> LootClass;//z战利品类

	UPROPERTY(EditAnywhere,Category = "LootTiers|Spawning")
	float ChanceToSpawn = 0.f;//生成几率

	UPROPERTY(EditAnywhere,Category = "LootTiers|Spawning")
	int32 MaxNumberToSpawn = 0.f;//生成最大数量

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "LootTiers|Spawning")
	bool bLootLevelOverride = true;
};
/**
 * 
 */
UCLASS()
class AURA_API ULootTiers : public UDataAsset
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	TArray<FLootItem> GetLootItems();//获取战利品

	UPROPERTY(EditDefaultsOnly,Category = "LootTiers|Spawning")
	TArray<FLootItem> LootItems;//战利品
};
