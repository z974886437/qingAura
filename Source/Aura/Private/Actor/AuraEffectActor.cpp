// 青楼


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"//这个类提供了很多 静态工具函数，主要用于在蓝图或 C++ 中访问 AbilitySystemComponent、Attributes、GameplayEffect 等 GAS 核心内容。
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/SphereComponent.h"


// Sets default values
AAuraEffectActor::AAuraEffectActor()
{
	//将此参与者设置为每帧调用Tick（）。如果你不需要它，你可以关闭它来提高性能。
	PrimaryActorTick.bCanEverTick = false;

	// Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	// SetRootComponent(Mesh);//设置根组件为Mesh
	// Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	// Sphere->SetupAttachment(GetRootComponent());//球体设置附件在根组件上
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot")));//构造函数中设置默认根组件（RootComponent） 的标准做法，尤其在自定义 Actor 类中非常常见
	
}

// void AAuraEffectActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
// 	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
// {
// 	
// }

// void AAuraEffectActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
// 	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
// {
// 	//TODO: Change this to apply a Gameplay Effect. For now,using const_cast as a hack!（将其更改为应用游戏效果。目前，暂时使用const_cast作为权宜之计！)
// 	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
// 	{
// 		//从 GAS 系统中获取并强转出你自己的属性集类 UAuraAttributeSet，用于读取属性（比如生命值、蓝量等）
// 		const UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(
// 			ASCInterface->GetAbilitySystemComponent()->GetAttributeSet(UAuraAttributeSet::StaticClass()));
//
// 		UAuraAttributeSet* MutableAuraAttributeSet = const_cast<UAuraAttributeSet*>(AuraAttributeSet);
// 		MutableAuraAttributeSet->SetHealth(AuraAttributeSet->GetHealth() + 25.f);//设置呼叫设置健康
// 		MutableAuraAttributeSet->SetMana(AuraAttributeSet->GetMana() - 25.f);//设置呼叫设置法力
// 		Destroy();//销毁一个 Actor
//
// 		
// 	}
// }

// Called when the game starts or when spawned
void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

	//Sphere->OnComponentBeginOverlap.AddDynamic(this,&AAuraEffectActor::OnOverlap);//触发回调函数以响应重叠内容
	//Sphere->OnComponentEndOverlap.AddDynamic(this,&AAuraEffectActor::EndOverlap);//触发回调函数以响应结束重叠内容
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



	
