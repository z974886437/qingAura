// 青楼

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"

USTRUCT(BlueprintType)
struct FUIWidgetRow : public  FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();//消息标签

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FText Message = FText();//消息

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<class UAuraUserWidget> MessageWidget;//消息小部件

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UTexture2D* Image = nullptr;//图像
};
class UAuraUserWidget;
class UAbilityInfo;
class UAuraAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangenSignature, float, NewHealth);//用于声明一个支持蓝图绑定的多播委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChangenSignature,float,NewMaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnManaChangenSignature,float,NewMana);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxManaChangenSignature,float,NewMaxMana);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature,FUIWidgetRow,Row);

/**
 * 
 */
UCLASS(BlueprintType,Blueprintable)
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValues() override;//广播初始值
	virtual void BindCallbacksToDependencies() override;//绑定回调到依赖项

	UPROPERTY(BlueprintAssignable,Category = "GAS|Attributes")
	FOnHealthChangenSignature OnHealthChanged;//健康变化

	UPROPERTY(BlueprintAssignable,Category = "GAS|Attributes")
	FOnHealthChangenSignature OnMaxHealthChanged;//最大健康变化

	UPROPERTY(BlueprintAssignable,Category = "GAS|Attributes")
	FOnManaChangenSignature OnManaChanged;//法力变化

	UPROPERTY(BlueprintAssignable,Category = "GAS|Attributes")
	FOnMaxManaChangenSignature OnMaxManaChanged;//最大法力值变化

	UPROPERTY(BlueprintAssignable,Category = "GAS|Messages")
	FMessageWidgetRowSignature MessageWidgetRowDelegate;//消息小部件 行 委托
	

protected:

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;//消息小部件数据表
	
	void HealthChanged(const FOnAttributeChangeData& Data) const;//健康变化
	void MaxHealthChanged(const FOnAttributeChangeData& Data) const;//最大健康变化
	void ManaChanged(const FOnAttributeChangeData& Data) const;//法力值变化
	void MaxManaChanged(const FOnAttributeChangeData& Data) const;//最大法力值变化

	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable,const FGameplayTag& Tag);//按标签的表格行 接受一个数据表和一个标签
};

template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(),TEXT(""));//从 UDataTable 查找数据
}
