// 青楼


#include "Actor/MagicCircle.h"

#include "Components/DecalComponent.h"

AMagicCircle::AMagicCircle()
{
	PrimaryActorTick.bCanEverTick = true;// 允许此Actor在每一帧调用Tick函数（启用帧更新）

	MagicCircleDecal = CreateDefaultSubobject<UDecalComponent>("MagicCircleDecal");// 创建一个默认的贴花组件（用于显示法阵的地面图案）
	MagicCircleDecal->SetupAttachment(GetRootComponent());// 将贴花组件附加到Actor的根组件上（确保法阵贴花跟随角色或场景根节点移动）
}

void AMagicCircle::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMagicCircle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

