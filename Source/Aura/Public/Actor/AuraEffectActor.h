// 青楼

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "AuraEffectActor.generated.h"


class UAbilitySystemComponent;
class UGameplayEffect;
//class USphereComponent;
UENUM(BlueprintType)
enum class EEffectApplicationPolicy//E效应应用策略
{
	ApplyOnOverlap,//适用于重叠
	ApplyOnEndOverlap,//应用末端重叠
	DoNotApply //不申请
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy//影响去除策略
{
	RemoveOnEndOverlap,//删除端部重叠
	DoNotRemove//不删除
};

UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAuraEffectActor();

	//重叠功能 声明一个碰撞重叠（Overlap）事件的函数，通常用来响应某个碰撞体与其他物体发生重叠时的回调。
	//UFUNCTION()
	// virtual void OnOverlap(
	// 	UPrimitiveComponent* OverlappedComponent,//发生重叠事件的组件（通常是你的碰撞体）
	// 	AActor* OtherActor,//与之重叠的另一个 Actor。
	// 	UPrimitiveComponent* OtherComp,//与之重叠的另一个组件。
	// 	int32 OtherBodyIndex,//另一个组件的物理体索引（一般用于复杂物理体）。
	// 	bool bFromSweep,//是否是通过 Sweep（移动时检测）触发的重叠。
	// 	const FHitResult& SweepResult);//如果是 Sweep 触发，包含详细的碰撞信息。
	
	//结束重叠 一个虚函数，通常用于处理碰撞体之间的 “结束重叠”（End Overlap） 事件
	//UFUNCTION()
	//virtual void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
protected:

	virtual void BeginPlay() override;
	// UPROPERTY(VisibleAnywhere)
	// TObjectPtr<USphereComponent> Sphere;
	//
	// UPROPERTY(VisibleAnywhere)
	// TObjectPtr<UStaticMeshComponent> Mesh;

	UFUNCTION(BlueprintCallable)//蓝图可调用
	void ApplyEffectToTarget(AActor* TargetActor,TSubclassOf<UGameplayEffect> GameplayEffectClass);//对目标应用效果

	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);//重叠

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);//末端重叠

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Applied Effects")
	bool bDestroyOnEffectApplication = false;//删除效果 = 关闭

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Applied Effects")
	bool bApplyEffectsToEnemies = false;//应用效果到敌人 = 关闭
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;//即时游戏效果类

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Applied Effects")
	EEffectApplicationPolicy InstantEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;//即时效果应用程序策略/立竿见影的效果

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;//持续时间游戏效果类

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Applied Effects")
	EEffectApplicationPolicy DurationEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;//持续有效时间的应用程序策略

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;//无限游戏效果类

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Applied Effects")
	EEffectApplicationPolicy InfiniteEffectApplicationPolicy= EEffectApplicationPolicy::DoNotApply;//无线效果的应用策略

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Applied Effects")
	EEffectRemovalPolicy InfiniteEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;//无限效果删除策略

	TMap<FActiveGameplayEffectHandle,UAbilitySystemComponent*> ActiveEffectHandles;//活动效果手柄

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Applied Effects")
	float ActorLevel = 1.f;
};
