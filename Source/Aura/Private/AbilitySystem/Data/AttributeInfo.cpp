// 青楼


#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	// 遍历 AttributeInformation 数组中的每一个属性信息
	for (const FAuraAttributeInfo& Info : AttributeInformation)
	{
		// 如果该条属性信息的标签与传入的标签完全匹配
		if (Info.AttributeTag.MatchesTagExact(AttributeTag))
		{
			// 找到匹配项，返回该属性信息
			return Info;
		}
	}

	// 如果未找到且要求记录日志
	if (bLogNotFound)
	{
		// 打印错误日志，标明找不到对应的属性信息
		UE_LOG(LogTemp,Error,TEXT("Can't find Info for AttributeTag [%s] on AttributeInfo [%s]."),*AttributeTag.ToString(),*GetNameSafe(this));
	}

	// 返回一个默认构造的 FAuraAttributeInfo（即空对象）
	return FAuraAttributeInfo();
}
