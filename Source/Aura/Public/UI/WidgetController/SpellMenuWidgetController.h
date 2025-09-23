// 青楼

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "GameplayTagContainer.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSpellGlobeSelectedSignature,bool,bSpendPointsButtonEnabled,bool,bEquipButtonEnabled);//拼写地球仪选择签名
/**
 * 
 */
UCLASS(BlueprintType,Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValues() override;//广播初始值
	virtual void BindCallbacksToDependencies() override;//绑定回调到依赖项

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStatChangedSignature SpellPointsChanged;//法术点已更改

	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectedSignature SpellGlobeSelectedDelegate;//Spell Globe 选定代表

	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);//已选择法术地球仪

private:

	static void ShouldEnableButtons(const FGameplayTag& AbilityStatus,int32 SpellPoints,bool& bShouldEnableSpellPointsButton,bool& bShouldEnableEquipButton);//应该启用按钮
};
