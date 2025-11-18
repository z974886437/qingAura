// 青楼


#include "Actor/AuraEnemySpawnPoint.h"

#include "Character/AuraEnemy.h"

void AAuraEnemySpawnPoint::SpawnEnemy()
{
	FActorSpawnParameters SpawnParams; // 设置生成敌人的参数

	// 设置碰撞处理方式为：如果可能的话调整碰撞，但始终生成
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AAuraEnemy* Enemy = GetWorld()->SpawnActorDeferred<AAuraEnemy>(EnemyClass,GetActorTransform()); // 使用延迟生成方式，确保可以在生成前设置属性
	Enemy->SetLevel(EnemyLevel); // 设置敌人的等级
	Enemy->SetCharacterClass(CharacterClass);// 设置敌人的角色类型
	Enemy->FinishSpawning(GetActorTransform());// 完成敌人的生成，并且把它放入世界中
	Enemy->SpawnDefaultController();  // 为敌人生成默认的控制器
}
