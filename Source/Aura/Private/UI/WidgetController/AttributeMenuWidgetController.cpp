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
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(FAuraGameplayTags::Get().Attributes_Primary_Strength);
	Info.AttributeValue = AS->GetStrength();// 设置当前属性值为角色的实际力量数值
	AttributeInfoDelegate.Broadcast(Info);// 通过多播委托广播属性信息，供 UI 控件接收并更新显示
}
