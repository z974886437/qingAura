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
	AAuraEffectActor();

	virtual void Tick(float DeltaTime) override;

	
protected:

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	FVector CalculatedLocation;//计算位置

	UPROPERTY(BlueprintReadOnly)
	FRotator CalculatedRotation;//计算旋转

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Pickup Movement")
	bool bRotates = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Pickup Movement")
	float RotationRate = 45.f;//旋转速度

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Pickup Movement")
	bool bSinusoidalMovement = false;//b 正弦运动

	UFUNCTION(BlueprintCallable)
	void StartSinusoidalMovement();//开始正弦运动

	UFUNCTION(BlueprintCallable)
	void StartRotation();//开始旋转

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Pickup Movement")
	float SineAmplitude = 1.f;//正弦幅度

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Pickup Movement")
	float SinePeriodConstant = 1.f;//正弦周期 

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Pickup Movement")
	FVector InitialLocation;//初始位置
	
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

	UPROPERTY()
	TMap<FActiveGameplayEffectHandle,UAbilitySystemComponent*> ActiveEffectHandles;//活动效果手柄

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Applied Effects")
	float ActorLevel = 1.f;

private:

	float RunningTime = 0.f;

	void ItemMovement(float DeltaTime);//物品移动
};
