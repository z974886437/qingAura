// 青楼


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(int32 XP)
{
	int32 Level = 1; // 从等级1开始查找
	bool bSearching = true;//b 搜索 控制循环是否继续
	while (bSearching)
	{
		//LevelUpInformation[1] = Level 1 Information
		//LevelUpInformation[2] = Level 1 Information
		// 注意：LevelUpInformation数组是从0开始存储的，
		// 所以 Num()-1 实际上对应最后一个合法下标
		if (LevelUpInformation.Num() -1 <= Level) return Level;// 如果 Level 已经超过数组最大下标，就直接返回当前等级

		if (XP >= LevelUpInformation[Level].LevelUpRequirement)// 判断经验是否达到当前等级的升级需求
		{
			++Level;// 满足条件 → 升一级，继续循环检查
		}
		else
		{
			bSearching = false;// 不满足 → 停止搜索
		}
		
	}
	return Level;// 返回最终计算出的等级
}
