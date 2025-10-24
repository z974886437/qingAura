// 青楼


#include "Actor/PointCollection.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// 构造函数：初始化一组固定数量的点（SceneComponent），用于位置集合或曲线采样等用途
APointCollection::APointCollection()
{
	PrimaryActorTick.bCanEverTick = false;// 禁止 Tick，每帧不更新，提高性能（因为这些点通常是静态位置）

	// 创建第一个点 Pt_0，并设置为根组件（RootComponent）
	Pt_0 = CreateDefaultSubobject<USceneComponent>("Pt_0");// 创建场景组件，名字为 Pt_0
	ImmutablePts.Add(Pt_0);// 加入到点集合数组中，方便统一管理
	SetRootComponent(Pt_0);// 将 Pt_0 设置为根节点

	// 以下依次创建 Pt_1 ~ Pt_10，并附加到根组件 Pt_0 下
	Pt_1 = CreateDefaultSubobject<USceneComponent>("Pt_1");// 创建第二个点
	ImmutablePts.Add(Pt_1);// 加入数组
	Pt_1->SetupAttachment(GetRootComponent());// 附加到根节点

	Pt_2 = CreateDefaultSubobject<USceneComponent>("Pt_2");
	ImmutablePts.Add(Pt_2);
	Pt_2->SetupAttachment(GetRootComponent());

	Pt_3 = CreateDefaultSubobject<USceneComponent>("Pt_3");
	ImmutablePts.Add(Pt_3);
	Pt_3->SetupAttachment(GetRootComponent());

	Pt_4 = CreateDefaultSubobject<USceneComponent>("Pt_4");
	ImmutablePts.Add(Pt_4);
	Pt_4->SetupAttachment(GetRootComponent());

	Pt_5 = CreateDefaultSubobject<USceneComponent>("Pt_5");
	ImmutablePts.Add(Pt_5);
	Pt_5->SetupAttachment(GetRootComponent());

	Pt_6 = CreateDefaultSubobject<USceneComponent>("Pt_6");
	ImmutablePts.Add(Pt_6);
	Pt_6->SetupAttachment(GetRootComponent());

	Pt_7 = CreateDefaultSubobject<USceneComponent>("Pt_7");
	ImmutablePts.Add(Pt_7);
	Pt_7->SetupAttachment(GetRootComponent());

	Pt_8 = CreateDefaultSubobject<USceneComponent>("Pt_8");
	ImmutablePts.Add(Pt_8);
	Pt_8->SetupAttachment(GetRootComponent());

	Pt_9 = CreateDefaultSubobject<USceneComponent>("Pt_9");
	ImmutablePts.Add(Pt_9);
	Pt_9->SetupAttachment(GetRootComponent());

	Pt_10 = CreateDefaultSubobject<USceneComponent>("Pt_10");
	ImmutablePts.Add(Pt_10);
	Pt_10->SetupAttachment(GetRootComponent());
	
}

TArray<USceneComponent*> APointCollection::GetGroundPoints(const FVector& GroundLocation, int32 NumPoints,float YawOverride)
{
	// 检查 ImmutablePts 的数量是否足够，否则报错防止越界访问
	checkf(ImmutablePts.Num() >= NumPoints,TEXT("Attempted to access ImmutablePts out of bounds."/*尝试越界访问 ImmutablePts。*/));

	TArray<USceneComponent*> ArrayCopy;// 创建一个空数组用于存储结果点

	for (USceneComponent* Pt : ImmutablePts)// 遍历所有固定点组件（ImmutablePts 是在构造函数中建立的多个点）
	{
		if (ArrayCopy.Num() >= NumPoints) return ArrayCopy;// 如果已经收集的点数达标，则立即返回结果数组

		if (Pt != Pt_0)// 跳过第一个点（Pt_0），因为它是基准点，其余点基于它旋转偏移
		{
			FVector ToPoint = Pt->GetComponentLocation() - Pt_0->GetComponentLocation();// 计算当前点相对 Pt_0 的向量方向
			ToPoint = ToPoint.RotateAngleAxis(YawOverride,FVector::UpVector);// 根据 YawOverride（偏航角）旋转该方向向量，使得点绕 Z 轴旋转
			Pt->SetWorldLocation(Pt_0->GetComponentLocation() + ToPoint);// 将旋转后的点更新为新位置
		}

		// 定义一条从点向上500单位到向下500单位的射线（用于地面检测）
		const FVector RaisedLocation = FVector(Pt->GetComponentLocation().X,Pt->GetComponentLocation().Y,Pt->GetComponentLocation().Z + 500.f);
		const FVector LoweredLocation = FVector(Pt->GetComponentLocation().X,Pt->GetComponentLocation().Y,Pt->GetComponentLocation().Z - 500.f);

		FHitResult HitResult;// 存储射线检测结果
		TArray<AActor*> IgnoreActors;// 要忽略的演员（通常是玩家或自己）
		// 获取半径1500范围内的所有存活玩家，添加到忽略列表中
		UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(this,IgnoreActors,TArray<AActor*>(),1500.f,GetActorLocation());

		// 设置碰撞查询参数
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActors(IgnoreActors);
		// 进行射线检测：从 RaisedLocation 向 LoweredLocation 发射一条“BlockAll”通道的线
		GetWorld()->LineTraceSingleByProfile(HitResult,RaisedLocation,LoweredLocation,FName("BlockAll"),QueryParams);

		// 使用检测到的地面碰撞点调整当前点的高度（Z）
		const FVector AdjustedLocation = FVector(Pt->GetComponentLocation().X,Pt->GetComponentLocation().Y,HitResult.ImpactPoint.Z);
		Pt->SetWorldLocation(AdjustedLocation);
		
		Pt->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal));// 让点的朝向对齐到地面法线，使其“贴合地面”

		ArrayCopy.Add(Pt);// 将调整好的点添加到结果数组中
	}
	return ArrayCopy;// 返回最终计算出的地面点数组
}

void APointCollection::BeginPlay()
{
	Super::BeginPlay();
	
}


