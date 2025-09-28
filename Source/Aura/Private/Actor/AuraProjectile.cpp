// 青楼


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;// 禁用该 Actor 的 Tick 函数调用。
	bReplicates = true;//如果为 true，则此 actor 将复制到远程计算机

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
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
	SetLifeSpan(LifeSpan);// 设置Actor的寿命，单位是秒，到时间后Actor会自动调用 Destroy()
	Sphere->OnComponentBeginOverlap.AddDynamic(this,&AAuraProjectile::OnSphereOverlap);//球体碰撞组件（Sphere）的 重叠开始事件 绑定到 AAuraProjectile::OnSphereOverlap 函数

	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound,GetRootComponent());// 播放一个持续循环的声音，并将其绑定到当前Actor的根组件上
}

// 处理 projectile 碰撞后的效果和逻辑
void AAuraProjectile::OnHit()
{
	// 播放碰撞时的音效（ImpactSound）并设置音效的位置为当前投射物的位置
	UGameplayStatics::PlaySoundAtLocation(this,ImpactSound,GetActorLocation(),FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,ImpactEffect,GetActorLocation());// 在碰撞位置生成特效（ImpactEffect）
	if (LoopingSoundComponent) LoopingSoundComponent->Stop();// 如果存在循环音效组件，则停止该循环音效
	bHit = true;// 标记投射物已经命中
}

void AAuraProjectile::Destroyed()
{
	// 如果没有命中任何目标 且 当前不是服务器（说明是客户端自己看到的销毁）
	// 这样做是为了客户端本地立即播放击中特效，减少网络延迟带来的体验落差
	if (!bHit && !HasAuthority()) OnHit();
	Super::Destroyed();// 调用父类的 Destroyed()，确保父类的销毁逻辑被执行（比如清理引用、释放资源等）
}

// 处理投射物与目标物体的重叠事件（如碰撞）
void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();// 获取施法者的 AvatarActor（通常是角色对象）

	if (SourceAvatarActor == OtherActor) return;// 检查伤害效果是否有效，或者施加者是否是自己，如果是自己，直接返回（避免伤害自己）
	if (!UAuraAbilitySystemLibrary::IsNotFriend(SourceAvatarActor,OtherActor)) return;// 检查施加者与目标是否是敌对关系，如果不是敌人（即友方），则不处理伤害
	if (!bHit) OnHit();// 如果投射物未命中过，调用 OnHit 处理碰撞音效和特效等
	
	if (HasAuthority())	// 确保只有在服务器端才会处理伤害与销毁逻辑（避免客户端与服务器不同步）
	{
		// 如果目标 Actor 有 AbilitySystemComponent（能力系统组件），则应用伤害
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			// 设置目标的 AbilitySystemComponent，并应用伤害效果
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
		
		Destroy();// 销毁当前Actor（投射物）
	}
	else bHit = true;// 如果是客户端，直接标记为已命中，防止重复触发
}



