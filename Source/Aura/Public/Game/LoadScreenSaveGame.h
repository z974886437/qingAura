// 青楼

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

class UGameplayAbility;

UENUM(BlueprintType)
enum ESaveSlotStatus//E 保存槽位状态
{
	Vacant,//空缺
	EnterName,//输入姓名
	Taken//采取
};

USTRUCT()
struct FSavedActor
{
	GENERATED_BODY()

	UPROPERTY()
	FName ActorName = FName();//演员姓名

	UPROPERTY()
	FTransform Transform = FTransform();//转换

	// Serialized variables from the Actor - only those marked with  SaveGame specifier 来自 Actor 的序列化变量 - 仅那些标有 SaveGame 说明符的变量
	UPROPERTY()
	TArray<uint8> Bytes;//字节
};

// 比较两个 FSavedActor 是否相等（只根据 ActorName 判断）
inline bool operator==(const FSavedActor& Left,const FSavedActor& Right)
{
	return Left.ActorName == Right.ActorName;// 判断左右对象的 ActorName 是否完全一致
}

USTRUCT()
struct FSavedMap
{
	GENERATED_BODY()

	UPROPERTY()
	FString MapAssetName = FString();

	UPROPERTY()
	TArray<FSavedActor> SavedActors;
};

USTRUCT(BlueprintType)
struct FSavedAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ClassDefaults")
	TSubclassOf<UGameplayAbility> GameplayAbility;//游戏能力

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FGameplayTag AbilityTag = FGameplayTag();//能力标签

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FGameplayTag AbilityStatus = FGameplayTag();//能力状况

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FGameplayTag AbilitySlot = FGameplayTag();//能力槽

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FGameplayTag AbilityType = FGameplayTag();//能力类型

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	int32 AbilityLevel;//能力水平
};

// 声明一个内联函数，重载 “==” 运算符，用于比较两个 FSavedAbility 对象是否相等
inline bool operator==(const FSavedAbility& Left,const FSavedAbility& Right)
{
	// 比较两个 FSavedAbility 对象是否相等：如果它们的 AbilityTag 完全一致，则认为相同
	// 如果两个技能的标签（AbilityTag）完全匹配（MatchesTagExact），则返回 true，否则返回 false
	return Left.AbilityTag.MatchesTagExact(Right.AbilityTag);
}

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
	FString MapAssetName = FString("Default Map Asset Name");//地图名

	UPROPERTY()
	FName PlayerStartTag;//播放器开始标签

	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = Vacant;//保存插槽状态

	UPROPERTY()
	bool bFirstTimeLoadIn = true;//b 第一次加载

	/*
	 * Player
	 */

	UPROPERTY()
	int32 PlayerLevel = 1;

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

	/*
	 * Abilities
	 */

	UPROPERTY()
	TArray<FSavedAbility> SavedAbilities;

	UPROPERTY()
	TArray<FSavedMap> SavedMaps;

	FSavedMap GetSavedMapWithMapName(const FString& InMapName);//获取带有地图名称的已保存地图
	bool HasMap(const FString& InMapName);
};
