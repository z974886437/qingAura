// 青楼


#include "Game/LoadScreenSaveGame.h"

// 根据传入的关卡名查找已保存的关卡数据并返回
FSavedMap ULoadScreenSaveGame::GetSavedMapWithMapName(const FString& InMapName)
{
	for (const FSavedMap& Map : SavedMaps) // 遍历所有已保存的关卡信息（SavedMaps 是保存关卡的数组）
	{
		if (Map.MapAssetName == InMapName) // 如果当前关卡的资源名与传入关卡名相同，则表示找到了目标关卡
		{
			return Map; // 返回找到的关卡数据
		}
	}
	return FSavedMap(); // 如果没有找到，则返回一个默认构造的 FSavedMap（表示空结果）
}

// 判断是否存在指定名称的保存关卡
bool ULoadScreenSaveGame::HasMap(const FString& InMapName)
{
	for (const FSavedMap& Map : SavedMaps) // 遍历所有已保存的关卡信息
	{
		if (Map.MapAssetName == InMapName) // 如果存在名称匹配的关卡，则说明该关卡已保存
		{
			return true;  // 返回 true 表示存在
		}
	}
	return false; // 走到这里说明没有找到对应关卡，返回 false
}
