// 青楼

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

struct FGameplayAttribute;
class UAttributeInfo;
struct FAuraAttributeInfo;
struct FGameplayTag;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FAuraAttributeInfo&,Info);

/**
 * 
 */
UCLASS(BlueprintType,Blueprintable)
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BindCallbacksToDependencies() override;//绑定回调到依赖关系
	virtual void BroadcastInitialValues() override;//广播初始值

	UPROPERTY(BlueprintAssignable,Category = "GSA|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;//属性信息委托

	UPROPERTY(BlueprintAssignable,Category = "GSA|Attributes")
	FOnPlayerStatChangedSignature AttributePointsChangedDelegate;//属性点已更改委托

	UFUNCTION(BlueprintCallable)
	void UpgradeAttribute(const FGameplayTag& AttributeTag);//升级属性

protected:

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAttributeInfo> AttributeInfo;//属性信息

private:

	void BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const;//广播属性信息
};
