// 青楼


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

// 根据技能等级生成一段技能描述字符串
FString UAuraGameplayAbility::GetDescription(int32 Level)
{
	// 使用 FString::Printf 格式化字符串，插入技能名字和等级
	// 注意：这里的 <Default>、<Level> 是富文本标记（用于 UI 样式），不是普通 HTML
	return FString::Printf(TEXT("<Default>%s,</><Level>%d</>"),// 格式模板，%s = 技能名，%d = 技能等级
		L"Default Ability Name - LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum ",// 技能名（写死的测试文本）
		Level);// 技能等级（由函数参数传入）
}

// 根据传入的等级，生成下一等级的技能描述
FString UAuraGameplayAbility::GetNextLevelDescription(int32 Level)
{
	// 使用 FString::Printf 来格式化描述文本
	// 格式里用到了富文本标签 <Default> 和 <Level>，UI 会根据这些标签显示不同样式
	return FString::Printf(TEXT("<Default>Next Level: </><Level>%d</> \n<Default>Causes much more damage </>"),// 格式字符串，%d 会被替换成技能等级
		Level); // 要显示的技能等级
}

FString UAuraGameplayAbility::GetLockedDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Spell Locked Until Level: %d</>"),Level);
}
