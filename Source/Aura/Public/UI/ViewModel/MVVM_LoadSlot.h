// 青楼

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "MVVM_LoadSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex,int32,WidgetSwiticherIndex);//F 设置小部件切换器索引

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

	void InitializeSlot();

	UPROPERTY()
	FString SlotIndex;//插槽索引

	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SlotStatus;//槽位状态
	/*
	 * Field Notifies
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter,Getter);
	FString PlayerName;//玩家名字

	void SetPlayerName(FString InPlayerName);//设置玩家名称
	void SetLoadSlotName(FString InLoadSlotName);//设置加载槽名称

	FString GetPlayerName() const { return PlayerName; }//获取玩家名称
	FString GetLoadSlotName() const { return LoadSlotName; }//获取加载槽名称
private:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess="true"));
	FString LoadSlotName;//加载槽名称
};
