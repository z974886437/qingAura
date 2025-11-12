// 青楼

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "MVVM_LoadSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex,int32,WidgetSwiticherIndex);//F 设置小部件切换器索引
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnableSelectSlotButton,bool,bEnable);//F 启用选择插槽按钮

/**
 * 
 */
UCLASS()
class AURA_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndex SetWidgetSwitcherIndex;//设置小部件切换器索引

	UPROPERTY(BlueprintAssignable)
	FEnableSelectSlotButton EnableSelectSlotButton;//启用选择插槽按钮

	void InitializeSlot();

	UPROPERTY()
	int32 SlotIndex;//插槽索引

	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SlotStatus;//槽位状态

	UPROPERTY()
	FName PlayerStartTag;//播放器开始标签
	/*
	 * Field Notifies
	 */
	
	void SetPlayerName(FString InPlayerName);//设置玩家名称
	void SetMapName(FString InMapName);//设置地图名称
	void SetPlayerLevel(int32 InLevel);//设置玩家等级
	void SetLoadSlotName(FString InLoadSlotName);//设置加载槽名称

	FString GetPlayerName() const { return PlayerName; }//获取玩家名称
	FString GetMapName() const { return MapName; }//获取地图名称
	int32 GetPlayerLevel() const { return PlayerLevel; }//获取玩家等级
	FString GetLoadSlotName() const { return LoadSlotName; }//获取加载槽名称
	
private:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter,Getter, meta = (AllowPrivateAccess = "true"));
	FString PlayerName;//玩家名字

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter,Getter, meta = (AllowPrivateAccess = "true"));
	FString MapName;//地图名字

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter,Getter, meta = (AllowPrivateAccess = "true"));
	int32 PlayerLevel;//玩家等级
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess="true"));
	FString LoadSlotName;//加载槽名称
};
