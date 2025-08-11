// 青楼


#include "Actor/AuraProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;// 禁用该 Actor 的 Tick 函数调用。
	bReplicates = true;//如果为 true，则此 actor 将复制到远程计算机

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);//只进行“查询类”碰撞检测，不进行物理模拟
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);//把 Sphere 对所有碰撞通道的响应全部设为 忽略（Ignore）
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Overlap);//单独对 WorldDynamic 通道设置为 重叠（Overlap）
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;//重力设置 = 0
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this,&AAuraProjectile::OnSphereOverlap);//球体碰撞组件（Sphere）的 重叠开始事件 绑定到 AAuraProjectile::OnSphereOverlap 函数
	
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}



