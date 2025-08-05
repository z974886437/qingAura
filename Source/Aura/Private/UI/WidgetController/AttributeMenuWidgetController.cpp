// 青楼


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AuraGameplayTags.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	// 将基类 AttributeSet 强制转换为你自定义的 AuraAttributeSet，方便访问你自定义的 TagsToAttributes 等内容
	// CastChecked 如果转换失败，会在开发环境中触发断言，便于调试
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);

	check(AttributeInfo);// 检查 AttributeInfo 是否有效，防止后续空指针崩溃
	
	// 遍历你在 AttributeSet 中维护的 标签 → 属性 映射表（TagsToAttributes）
	// 例如：FGameplayTag("Attributes.Health") → FGameplayAttribute(Health)
	for (auto& Pair : AS->TagsToAttributes)
	{
		// 对每一个属性（比如 Health、Mana）注册一个值变化的回调函数（Delegate）
		// 当这个属性数值发生变化时，会自动触发这个 lambda 回调
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			// 这是回调函数，当属性值变化时会触发此函数
			// 捕获 this 是为了访问当前类的成员（比如 AttributeInfoDelegate）
			[this,Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key,Pair.Value());
				// FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Pair.Key);// 根据标签查找 UI 所需的属性信息（比如图标、描述等）
				// // 获取当前属性的最新数值，并设置到 Info 结构中
				// // GetNumericValue 需要传入 AttributeSet 实例，它会返回 float 类型的值
				// Info.AttributeValue = Pair.Value().GetNumericValue(AS);
				// AttributeInfoDelegate.Broadcast(Info);// 广播属性信息，让 UI 更新对应的属性显示
			}
		);
	}
}



void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	// 将 AttributeSet 强制转换为 UAuraAttributeSet 类型，确保是我们定义的属性集类
	// CastChecked 如果转换失败，会直接 crash，便于调试错误
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);

	check(AttributeInfo);// 检查 AttributeInfo 是否有效，防止后续空指针崩溃

	// 查找与 "力量（Strength）" 对应的属性信息（用于 UI 显示标签、图标、描述等）
	//FAuraAttributeInfo StrengthInfo = AttributeInfo->FindAttributeInfoForTag(FAuraGameplayTags::Get().Attributes_Primary_Strength);
	//StrengthInfo.AttributeValue = AS->GetStrength();// 设置当前属性值为角色的实际力量数值
	//AttributeInfoDelegate.Broadcast(StrengthInfo);// 通过多播委托广播属性信息，供 UI 控件接收并更新显示

	for (auto& Pair : AS->TagsToAttributes)// 遍历 AttributeSet 中的标签 → 属性映射表（TagsToAttributes）
	{
		BroadcastAttributeInfo(Pair.Key,Pair.Value());
	}
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);// 根据标签查找 UI 所需的属性信息（比如图标、描述等）
	// 获取当前属性的最新数值，并设置到 Info 结构中
	// GetNumericValue 需要传入 AttributeSet 实例，它会返回 float 类型的值
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);// 广播属性信息，让 UI 更新对应的属性显示
}

