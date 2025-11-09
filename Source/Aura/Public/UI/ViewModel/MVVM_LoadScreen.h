// 青楼

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadScreen.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSlotSelected);//F 已选择插槽

class UMVVM_LoadSlot;
/**
 * 
 */
UCLASS()
class AURA_API UMVVM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:

	void InitializeLoadSlots();//初始化加载槽

	UPROPERTY(BlueprintAssignable)
	FSlotSelected SlotSelected;//已选择插槽

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadSlot> LoadSlotViewModelClass;//加载槽视图模型类

	UFUNCTION(BlueprintPure)
	UMVVM_LoadSlot* GetLoadSlotViewModelByIndex(int32 Index) const;//通过索引获取加载槽视图模型

	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(int32 Slot,const FString& EnteredName);//按下新插槽按钮

	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed(int32 Slot);//按下新游戏按钮

	UFUNCTION(BlueprintCallable)
	void SelectSlotButtonPressed(int32 Slot);//按下选择插槽按钮

	UFUNCTION(BlueprintCallable)
	void DeleteButtonPressed();//删除按钮被按下

	void LoadData();//加载数据

	void SetNumLoadSlots(int32 InNumLoadSlots);//设置加载槽数

	int32 GetNumLoadSlots() const { return NumLoadSlots; }//获取加载槽数
	
private:

	UPROPERTY()
	TMap<int32,UMVVM_LoadSlot*> LoadSlots;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_0;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_1;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_2;

	UPROPERTY()
	UMVVM_LoadSlot* SelectedSlot;//选定的插槽

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess="true"));
	int32 NumLoadSlots;//装载槽数
};
