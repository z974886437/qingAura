// 青楼

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

UENUM(BlueprintType)
enum ESaveSlotStatus//E 保存槽位状态
{
	Vacant,//空缺
	EnterName,//输入姓名
	Taken//采取
};

/**
 * 
 */
UCLASS()
class AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FString SlotName = FString();//插槽名字

	UPROPERTY()
	int32 SlotIndex = 0;//插槽索引

	UPROPERTY()
	FString PlayerName = FString("Default Name");//玩家姓名

	UPROPERTY()
	FString MapName = FString("Default Map Name");//地图名

	UPROPERTY()
	FName PlayerStartTag;//播放器开始标签

	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = Vacant;//保存插槽状态

	/*
	 * Player
	 */

	UPROPERTY()
	int32 PlayerLevel = 0;

	UPROPERTY()
	int32 XP = 0;

	UPROPERTY()
	int32 SpellPoints = 0;

	UPROPERTY()
	int32 AttributePoints = 0;

	/*
	 * Attributes
	 */

	UPROPERTY()
	float Strength = 0;//力量

	UPROPERTY()
	float Intelligence = 0;//智力

	UPROPERTY()
	float Resilience = 0;//韧性

	UPROPERTY()
	float Vigor = 0;//活力
	
};
