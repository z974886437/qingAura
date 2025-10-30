// 青楼


#include "Actor/AuraFireBall.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Components/AudioComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameplayCueManager.h"

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

void AAuraFireBall::OnHit()
{
	if (GetOwner())// 判断投射物的拥有者是否存在
	{
		FGameplayCueParameters CueParams;// 创建一个游戏效果参数对象
		CueParams.Location = GetActorLocation(); // 设置触发效果的位置为火球当前的位置

		// 执行游戏效果，触发火爆的视觉效果或其他关联效果
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetOwner(),FAuraGameplayTags::Get().GameplayCue_FireBlast,CueParams);
	}
	
	if (LoopingSoundComponent)// 如果投射物有循环播放的飞行音效（例如火球呼啸声）
	{
		LoopingSoundComponent->Stop();// 如果存在循环音效组件，则停止该循环音效
		LoopingSoundComponent->DestroyComponent();// 销毁该音效组件，释放资源
	}

	bHit = true;// 标记投射物已经命中
}
