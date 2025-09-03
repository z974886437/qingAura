// 青楼


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();// 获取角色面向方向（前向向量）
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation(); // 获取角色当前位置
	const float DeltaSpread = SpawnSpread / NumMinions;//Delta范围

	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f,FVector::UpVector);// 计算扇形左边界方向（向前向量绕 Z 轴旋转 SpawnSpread/2 度）
	TArray<FVector> SpawnLocations;
	for (int32 i = 0;i < NumMinions;i ++ )
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i,FVector::UpVector);// 从左边界开始，每次往右旋转 DeltaSpread 度，得到不同方向
		// 在这个方向上，随机选取一个距离（范围 = [MinSpawnDistance, MaxSpawnDistance]）
		const FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance,MaxSpawnDistance);
		SpawnLocations.Add(ChosenSpawnLocation);// 把生成点存到数组里，供后续使用（比如召唤小怪）

		// 🔽 以下是调试可视化 🔽
		// 画一个青色球，显示实际的生成位置
		DrawDebugSphere(GetWorld(),ChosenSpawnLocation,18.f,12,FColor::Cyan,false,3.f);
		// 画一条绿色箭头，从角色位置指向方向的最远点（MaxSpawnDistance）
		UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(),Location,Location + Direction * MaxSpawnDistance,4.f,FLinearColor::Green,3.f);
		// 画两个红色球：方向上的最小距离点和最大距离点，用来显示随机范围
		DrawDebugSphere(GetWorld(),Location + Direction * MinSpawnDistance,5.f,12,FColor::Red,false,3.f);
		DrawDebugSphere(GetWorld(),Location + Direction * MaxSpawnDistance,5.f,12,FColor::Red,false,3.f);
	}
	
	return SpawnLocations;// 当前只返回一个空数组（还没真正生成坐标点）
}
