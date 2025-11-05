// 青楼

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

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

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;//加载屏幕保存游戏类
};
