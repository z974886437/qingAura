// 青楼


#include "Actor/AuraEnemySpawnVolume.h"

#include "Actor/AuraEnemySpawnPoint.h"
#include "Components/BoxComponent.h"
#include "Interaction/PlayerInterface.h"

// AAuraEnemySpawnVolume 构造函数
AAuraEnemySpawnVolume::AAuraEnemySpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;// 禁用每帧更新

	// 创建 Box 组件并设置为根组件
	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	SetRootComponent(Box);
	
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);// 只进行查询，不与其他物体发生物理反应
	Box->SetCollisionObjectType(ECC_WorldStatic);// 设置为静态物体
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);// 对所有通道的碰撞响应设置为忽略
	Box->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);// 对玩家碰撞通道设置为重叠

}

// 加载敌人生成体积（如果已经到达某个状态，销毁该体积）
void AAuraEnemySpawnVolume::LoadActor_Implementation()
{
	if (bReached)// 如果已经到达状态
	{
		Destroy();// 销毁该体积
	}
}

void AAuraEnemySpawnVolume::BeginPlay()// 开始游戏时调用
{
	Super::BeginPlay();
	Box->OnComponentBeginOverlap.AddDynamic(this,&AAuraEnemySpawnVolume::OnBoxOverlap);// 当 Box 组件与其他物体发生重叠时，调用 OnBoxOverlap 函数
	
}

// 处理 Box 组件与其他物体的重叠
void AAuraEnemySpawnVolume::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->Implements<UPlayerInterface>()) return;// 如果其他物体没有实现 UPlayerInterface 接口，返回
	
	bReached = true;// 设置到达状态

	for (AAuraEnemySpawnPoint* Point : SpawnPoints) // 遍历所有生成点，并生成敌人
	{
		if (IsValid(Point))// 确保生成点有效
		{
			Point->SpawnEnemy(); // 调用生成敌人的函数
		}
	}
	Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // 禁用 Box 的碰撞
}



