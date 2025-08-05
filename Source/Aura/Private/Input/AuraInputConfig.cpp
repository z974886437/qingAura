// 青楼


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag,bool bLogNotFound) const
{
	// 遍历配置中的所有技能输入绑定
	for (const FAuraInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)// 如果这个 Action 设置了输入，并且它的标签与目标标签一致
		{
			return Action.InputAction;// 返回这个输入动作
		}
	}

	if (bLogNotFound)// 如果开启了 bLogNotFound 参数，就打印一条错误日志
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] , on InputConfig [%s]"),*InputTag.ToString(),*GetNameSafe(this));
	}
	return nullptr;// 没找到就返回空指针
}
