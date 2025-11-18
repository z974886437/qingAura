// 青楼


#include "AbilitySystem/Data/LootTiers.h"

TArray<FLootItem> ULootTiers::GetLootItems()
{
	TArray<FLootItem> ReturnItems;// 创建一个返回掉落物品的空数组

	for (FLootItem& Item : LootItems) // 遍历所有的 LootItems 数组中的物品
	{
		for (int32 i = 0; i < Item.MaxNumberToSpawn; ++i)// 根据每个物品的最大生成数量来决定生成多少个该物品
		{
			if (FMath::FRandRange(1.f,100.f) < Item.ChanceToSpawn) // 使用随机数判断该物品是否生成，生成的概率由 ChanceToSpawn 决定
			{
				FLootItem NewItem; // 创建一个新掉落物品
				NewItem.LootClass = Item.LootClass; // 设置掉落物品的类型
				NewItem.bLootLevelOverride = Item.bLootLevelOverride; // 是否覆盖物品的等级
				ReturnItems.Add(NewItem); // 将生成的物品加入掉落列表
			}
		}
	}

	return ReturnItems; // 返回所有生成的物品
}
