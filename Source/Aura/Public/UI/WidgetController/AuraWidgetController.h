// 青楼

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AuraWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, int32, NewValue);//球员统计数据更改签名的多播委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature,const FAuraAbilityInfo&,Info);//技能信息签名

class UAttributeSet;
class UAbilitySystemComponent;
class AAuraPlayerController;
class AAuraPlayerState;
class UAuraAbilitySystemComponent;
class UAuraAttributeSet;
class UAbilityInfo;

USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()

	FWidgetControllerParams(){}
	FWidgetControllerParams(APlayerController* PC,APlayerState* PS,UAbilitySystemComponent* ASC,UAttributeSet* AS)
	:PlayerController(PC),PlayerState(PS),AbilitySystemComponent(ASC),AttributeSet(AS) {}

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;//玩家控制器为空

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;//玩家状态为空

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;//能力系统组件为空

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;//属性集为空
};
/**
 * 
 */
UCLASS()
class AURA_API UAuraWidgetController : public UObject 
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);//设置小部件控制器参数

	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues();//广播初始值
	virtual void BindCallbacksToDependencies();//绑定回调到依赖项

	UPROPERTY(BlueprintAssignable,Category = "GAS|Messages")
	FAbilityInfoSignature AbilityInfoDelegate;//技能信息委托

	void BroadcastAbilityInfo();//广播能力信息
protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Widget Data")
	TObjectPtr<UAbilityInfo> AbilityInfo;//技能信息

	UPROPERTY(BlueprintReadOnly,Category = "WidgetController")
	TObjectPtr<APlayerController> PlayerController;//玩家控制器

	UPROPERTY(BlueprintReadOnly,Category = "WidgetController")
	TObjectPtr<APlayerState> PlayerState;//玩家状态

	UPROPERTY(BlueprintReadOnly,Category = "WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;//能力系统组件

	UPROPERTY(BlueprintReadOnly,Category = "WidgetController")
	TObjectPtr<UAttributeSet> AttributeSet;//属性集

	UPROPERTY(BlueprintReadOnly,Category = "WidgetController")
	TObjectPtr<AAuraPlayerController> AuraPlayerController;//玩家控制器

	UPROPERTY(BlueprintReadOnly,Category = "WidgetController")
	TObjectPtr<AAuraPlayerState> AuraPlayerState;//玩家状态

	UPROPERTY(BlueprintReadOnly,Category = "WidgetController")
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;//能力系统组件

	UPROPERTY(BlueprintReadOnly,Category = "WidgetController")
	TObjectPtr<UAuraAttributeSet> AuraAttributeSet;//属性集

	AAuraPlayerController* GetAuraPC();
	AAuraPlayerState* GetAuraPS();
	UAuraAbilitySystemComponent* GetAuraASC();
	UAuraAttributeSet* GetAuraAS();
};
