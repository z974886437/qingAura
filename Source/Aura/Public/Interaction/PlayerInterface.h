// 青楼

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelForXP(int32 InXP) const;
	
	UFUNCTION(BlueprintNativeEvent)
	int32 GetXP() const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointsReward(int32 Level) const;//获得属性积分奖励

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointsReward(int32 Level) const;//获得法术积分奖励

	UFUNCTION(BlueprintNativeEvent)
	void AddToXP(int32 InXP);//添加到XP

	UFUNCTION(BlueprintNativeEvent)
	void AddToPlayerLevel(int32 InPlayerLevel);//添加到玩家等级

	UFUNCTION(BlueprintNativeEvent)
	void AddToAttributePoints(int32 InAttributePoints);//添加到属性点

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePoints() const;//获取属性点

	UFUNCTION(BlueprintNativeEvent)
	void AddToSpellPoints(int32 InSpellPoints);//添加到法术点

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPoints() const;//获取法术点
	
	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);//展示魔法阵

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void HideMagicCircle();//隐藏魔法阵

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void SaveProgress(const FName& CheckpointTag);//保存进度
};
