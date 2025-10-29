// 青楼


#include "Actor/AuraFireBall.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

void AAuraFireBall::BeginPlay()
{
	Super::BeginPlay();
	StartOutgoingTimeline();
}

void AAuraFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor)) return;

	if (HasAuthority())	// 确保只有在服务器端才会处理伤害与销毁逻辑（避免客户端与服务器不同步）
	{
		// 如果目标 Actor 有 AbilitySystemComponent（能力系统组件），则应用伤害
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;// 计算死亡冲击力，将角色的前方向量与死亡冲击力大小相乘
			DamageEffectParams.DeathImpulse = DeathImpulse;// 将计算出的冲击力赋值给 DamageEffectParams 的 DeathImpulse 属性
			
			// 设置目标的 AbilitySystemComponent，并应用伤害效果
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);// 应用伤害效果，传入 DamageEffectParams
		}
	}
}
