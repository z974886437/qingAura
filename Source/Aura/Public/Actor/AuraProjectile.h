// 青楼

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "GameFramework/Actor.h"
#include "AuraProjectile.generated.h"


class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;


UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuraProjectile();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;//抛射运动

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FDamageEffectParams DamageEffectParams;//伤害效果规格手柄

protected:
	virtual void BeginPlay() override;
	void OnHit();
	virtual void Destroyed() override;//摧毁
	
	UFUNCTION()
	void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,// [参数1] 当前触发重叠事件的组件（比如这个球体碰撞组件本身）
		AActor* OtherActor,// [参数2] 进入碰撞区域的另一个 Actor（例如玩家角色、NPC、物品等）
		UPrimitiveComponent* OtherComp,// [参数3] 对方 Actor 上参与碰撞的具体组件（例如对方的 CapsuleComponent）
		int32 OtherBodyIndex, // [参数4] 碰撞体索引（主要用于一个 Actor 有多个物理 Body 的情况）	
		bool bFromSweep, // [参数5] 是否是通过 Sweep（移动检测）触发的重叠
		const FHitResult& SweepResult// [参数6] 如果是 Sweep 触发，这里有命中信息（位置、法线等）
		);//球体覆盖

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<USphereComponent> Sphere;//球体
	
private:

	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.f;//寿命

	bool bHit = false;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;//冲击效果

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;//冲击声音

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound;//循环声音

	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;//循环声音组件
};
