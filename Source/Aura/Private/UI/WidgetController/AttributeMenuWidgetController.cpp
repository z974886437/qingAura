// 青楼


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AuraGameplayTags.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{

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
		FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Pair.Key);// 根据标签查找 UI 所需的属性信息（比如图标、描述等）
		// 调用静态函数指针，得到对应的 FGameplayAttribute
		// 然后通过 GetNumericValue(AS) 从 AttributeSet 中获取当前属性的数值
		Info.AttributeValue = Pair.Value().GetNumericValue(AS);
		AttributeInfoDelegate.Broadcast(Info);// 广播属性信息，让 UI 更新对应的属性显示
	}
}
