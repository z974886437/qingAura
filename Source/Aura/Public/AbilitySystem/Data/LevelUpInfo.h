// 青楼

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

USTRUCT(BlueprintType)
struct FAuraLevelUpInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	int32 LevelUpRequirement = 0;//升级要求

	UPROPERTY(EditDefaultsOnly)
	int32 AttributePointAward = 1;//属性积分奖励

	UPROPERTY(EditDefaultsOnly)
	int32 SpellPointAward = 1;//法术点奖励
	
};

/**
 * 
 */
UCLASS()
class AURA_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly)
	TArray<FAuraLevelUpInfo> LevelUpInformation;//升级信息

	int32 FindLevelForXP(int32 XP) const;//查找 XP 级别
	
};
