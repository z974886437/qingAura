// 青楼


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"//这个类提供了很多 静态工具函数，主要用于在蓝图或 C++ 中访问 AbilitySystemComponent、Attributes、GameplayEffect 等 GAS 核心内容。
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AAuraEffectActor::AAuraEffectActor()
{
	//将此参与者设置为每帧调用Tick（）。如果你不需要它，你可以关闭它来提高性能。
	PrimaryActorTick.bCanEverTick = false;
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot")));//构造函数中设置默认根组件（RootComponent） 的标准做法，尤其在自定义 Actor 类中非常常见
	
}

void AAuraEffectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime; // 累加经过的时间
	const float SinePeriod = 2 * PI / SinePeriodConstant;// 计算一个周期（以常数 SinePeriodConstant 为基础）
	if (RunningTime > SinePeriod) // 如果经过的时间超过一个周期
	{
		RunningTime = 0.f; // 重置时间
	}
	ItemMovement(DeltaTime);// 执行物体的运动
}

void AAuraEffectActor::ItemMovement(float DeltaTime)
{
	if (bRotates) // 如果启用旋转
	{
		const FRotator DeltaRotation(0.f,DeltaTime * RotationRate , 0.f);  // 根据旋转速率，计算当前帧的旋转增量
		CalculatedRotation = UKismetMathLibrary::ComposeRotators(CalculatedRotation,DeltaRotation);  // 使用 ComposeRotators 组合旋转（避免逐帧修改旋转值，直接叠加旋转）
	}
	if (bSinusoidalMovement) // 如果启用正弦波运动
	{
		const float Sine = SineAmplitude * FMath::Sin(RunningTime * SinePeriodConstant); // 根据正弦函数计算垂直方向的偏移量
		CalculatedLocation = InitialLocation + FVector(0.f,0.f,Sine); // 根据正弦波偏移量更新位置
	}
}

// Called when the game starts or when spawned
void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = GetActorLocation(); // 获取初始位置
	CalculatedLocation = InitialLocation; // 初始化位置
	CalculatedRotation = GetActorRotation(); // 获取初始旋转
	
}

void AAuraEffectActor::StartSinusoidalMovement()
{
	bSinusoidalMovement = true; // 启用正弦波运动
	InitialLocation = GetActorLocation(); // 重新获取物体的初始位置
	CalculatedLocation = InitialLocation; // 重置位置
}

void AAuraEffectActor::StartRotation()
{
	bRotates = true; // 启用旋转
	CalculatedRotation = GetActorRotation(); // 重新获取物体的旋转
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;// 如果目标 Actor 有 "Enemy" 标签 并且 设置了“不对敌人应用效果” → 就直接返回，不继续执行
	
	//UAbilitySystemBlueprintLibrary 提供的一个静态函数，用于在 蓝图或 C++ 中 从任意 Actor 获取其 UAbilitySystemComponent
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);//获取能力系统组件
	if (TargetASC == nullptr) return;

	check(GameplayEffectClass);
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();//创建效果上线问并返回其句柄的函数
	EffectContextHandle.AddSourceObject(this);//调用添加源对象
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass,ActorLevel,EffectContextHandle);//制作传出规格,构建一个效果应用对象
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());//游戏效果规范应用与自身    

	//判断一个 GameplayEffectSpecHandle 是否是「无限持续时间的效果」
	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	//如果这个 GameplayEffect 是无限持续的，并且你的策略是“在离开范围时移除（RemoveOnEndOverlap）”
	if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle,TargetASC);//那就记录下它的 Handle 和目标 ASC。
	}

	// 如果设置了在效果应用后销毁 Actor，并且该效果是瞬时效果（Instant）
	// bDestroyOnEffectApplication: 控制是否应用效果后销毁 Actor
	// EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy: 获取 GameplayEffect 的持续时间类型
	if (!bIsInfinite)
	{
		Destroy();
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;// 如果目标 Actor 有 "Enemy" 标签 并且 设置了“不对敌人应用效果” → 就直接返回，不继续执行
	
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)//即时效果应用策略 == 重叠
	{
		ApplyEffectToTarget(TargetActor,InstantGameplayEffectClass);//可以使用即时游戏效果类
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)//持续有效时间效果应用策略 == 重叠
	{
		ApplyEffectToTarget(TargetActor,DurationGameplayEffectClass);//可以使用持续时间游戏效果类
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)//无限效果应用策略 == 重叠
	{
		ApplyEffectToTarget(TargetActor,InfiniteGameplayEffectClass);//可以使用无限游戏效果类
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;// 如果目标 Actor 有 "Enemy" 标签 并且 设置了“不对敌人应用效果” → 就直接返回，不继续执行
	
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)//即时效果应用策略 == 末端重叠重叠
	{
		ApplyEffectToTarget(TargetActor,InstantGameplayEffectClass);//可以使用即时游戏效果类
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)//持续有效时间效果应用策略 == 末端重叠重叠
	{
		ApplyEffectToTarget(TargetActor,DurationGameplayEffectClass);//可以使用持续时间游戏效果类
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)//无限效果应用策略 == 末端重叠
	{
		ApplyEffectToTarget(TargetActor,InfiniteGameplayEffectClass);//可以使用无限游戏效果类
	}
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)// 如果是“重叠结束移除”策略
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);//获取 能力系统组件
		if (!IsValid(TargetASC)) return;//判断对象是否 无效或已被销毁

		TArray<FActiveGameplayEffectHandle> HandlesToRemove;// 临时存储需要移除的 GameplayEffect handle
		for (TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
		{
			if (TargetASC == HandlePair.Value)// 如果找到的是当前的目标组件
			{
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key,1);// 从 ASC 上移除这个 Effect
				HandlesToRemove.Add(HandlePair.Key);// 标记为需要从 Map 中移除
			}
		}
		for (FActiveGameplayEffectHandle& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);// 安全地从 Map 中移除对应的 Handle（确保 Handle 一定存在）
		}
	}
}





	
