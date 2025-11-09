// 青楼

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class ULoadScreenSaveGame;
class USaveGame;
class UMVVM_LoadSlot;
class UAbilityInfo;
class UCharacterClassInfo;//角色类别信息
/**
 * 
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly,Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;//角色类别信息

	UPROPERTY(EditDefaultsOnly,Category = "Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;//技能信息
	
	void SaveSlotData(UMVVM_LoadSlot* LoadSlot,int32 SlotIndex);//保存插槽数据
	ULoadScreenSaveGame* GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const;//获取保存槽数据
	static void DeleteSlot(const FString& SlotName, int32 SlotIndex);//删除槽

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;//加载屏幕保存游戏类

	UPROPERTY(EditDefaultsOnly)
	FString DefaultMapName;//默认地图名称

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> DefaultMap;//默认地图

	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSoftObjectPtr<UWorld>> Maps;//地图

protected:
	virtual void BeginPlay() override;
};
