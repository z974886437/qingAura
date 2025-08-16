// 青楼


#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharacterClassDefaultInfo UCharacterClassInfo::GetClassDefaultInfo(ECharacterClass CharacterClass)
{
	// 从 CharacterClassInformation（TMap<ECharacterClass, FCharacterClassDefaultInfo>）中查找 CharacterClass 对应的值
	// FindChecked 会返回对应的值，如果找不到键会触发断言（程序报错并中断），适合“必须有”的情况
	return CharacterClassInformation.FindChecked(CharacterClass);
}
