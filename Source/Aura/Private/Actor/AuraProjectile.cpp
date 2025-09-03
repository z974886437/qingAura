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

void AAuraProjectile::Destroyed()
{
	// 如果没有命中任何目标 且 当前不是服务器（说明是客户端自己看到的销毁）
	// 这样做是为了客户端本地立即播放击中特效，减少网络延迟带来的体验落差
	if (!bHit && !HasAuthority())
	{
		// 在当前Actor位置播放音效（比如子弹击中声音）
		// 参数：this 是上下文对象，ImpactSound 是声音资源，位置是当前Actor位置，旋转是零
		UGameplayStatics::PlaySoundAtLocation(this,ImpactSound,GetActorLocation(),FRotator::ZeroRotator);
		// 在当前Actor位置生成 Niagara 粒子特效（比如爆炸特效）
		// 参数：this 是上下文对象，ImpactEffect 是 Niagara 特效资源，位置是当前Actor位置
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,ImpactEffect,GetActorLocation());
		if (LoopingSoundComponent) LoopingSoundComponent->Stop();
		bHit = true;
	}
	Super::Destroyed();// 调用父类的 Destroyed()，确保父类的销毁逻辑被执行（比如清理引用、释放资源等）
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!DamageEffectSpecHandle.Data.IsValid() || DamageEffectSpecHandle.Data.Get()->GetContext().GetEffectCauser() == OtherActor)
	{
		return;
	}
	if (!UAuraAbilitySystemLibrary::IsNotFriend(DamageEffectSpecHandle.Data.Get()->GetContext().GetEffectCauser(),OtherActor))
	{
		return;
	}
	if (!bHit)
	{
		// 在当前Actor位置播放音效（比如子弹击中声音）
		// 参数：this 是上下文对象，ImpactSound 是声音资源，位置是当前Actor位置，旋转是零
		UGameplayStatics::PlaySoundAtLocation(this,ImpactSound,GetActorLocation(),FRotator::ZeroRotator);
		// 在当前Actor位置生成 Niagara 粒子特效（比如爆炸特效）
		// 参数：this 是上下文对象，ImpactEffect 是 Niagara 特效资源，位置是当前Actor位置
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,ImpactEffect,GetActorLocation());
		if (LoopingSoundComponent) LoopingSoundComponent->Stop();
		bHit = true;
	}
	
	if (HasAuthority())	// 仅在服务器端执行销毁（避免客户端直接删除导致状态不同步）
	{
		// 如果目标 Actor 有 AbilitySystemComponent
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get()); // 把准备好的效果规格(比如伤害/治疗)应用到这个目标自己身上
		}
		
		Destroy();// 销毁当前Actor（投射物）
	}
	else
	{
		bHit = true;
	}
}



