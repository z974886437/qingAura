// 青楼

#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "AuraInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
public:
	//UserClass - 你要绑定事件的类类型 ,PressedFuncType - 对应“按下”动作的成员函数类型 ,ReleasedFuncType - 对应“松开”动作的成员函数类型, HeldFuncType - 对应“持续按住”动作的成员函数类型
	template<class UserClass,typename PressedFuncType,typename ReleasedFuncType,typename HeldFuncType>
	void BindAbilityActions(const UAuraInputConfig* InputConfig,UserClass* Object,PressedFuncType PressedFunc,ReleasedFuncType ReleasedFunc,HeldFuncType HeldFunc);//能力绑定输入动作
};

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UAuraInputComponent::BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object,PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);

	// 遍历输入配置中定义的所有技能输入动作
	for (const FAuraInputAction& Action : InputConfig->AbilityInputActions)
	{
		// 确保该输入动作存在，且有对应的 GameplayTag
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)// 绑定“按下”事件
			{
				BindAction(Action.InputAction,ETriggerEvent::Started,Object,PressedFunc,Action.InputTag);
			}

			if (ReleasedFunc)// 绑定“松开”事件
			{
				BindAction(Action.InputAction,ETriggerEvent::Completed,Object,ReleasedFunc,Action.InputTag);
			}
			
			if (HeldFunc)// 绑定“持续按住”事件（适用于蓄力、连发等操作）
			{
				BindAction(Action.InputAction,ETriggerEvent::Triggered,Object,HeldFunc,Action.InputTag);
			}
		}
	}
}
