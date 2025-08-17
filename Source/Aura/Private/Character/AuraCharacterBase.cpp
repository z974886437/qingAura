// 青楼


#include "Character/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"
#include "WorldPartition/Cook/WorldPartitionCookPackage.h"

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);//让胶囊体碰撞体对相机通道忽略碰撞（防止相机被角色身体挡住）
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);//关闭角色胶囊碰撞体的重叠事件
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);// 让角色网格体（骨骼模型）对相机通道忽略碰撞（避免近距离相机穿模或挡视线）
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile,ECR_Overlap);// 设置 Mesh 对自定义投射物通道（ECC_Projectile）的碰撞响应为 Overlap（重叠）
	GetMesh()->SetGenerateOverlapEvents(true);// 开启网格生成重叠事件

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");//创建默认子对象来指定使用骨架网格物体组件并在引号中我们将拥有简单武器
	Weapon->SetupAttachment(GetMesh(),FName("WeaponHandSocket"));//将武器组件附加到角色骨骼网格WeaponHandSocket上
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);//关闭武器碰撞检测

}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector AAuraCharacterBase::GetCombatSocketLocation()
{
	check(Weapon);
	return Weapon->GetSocketLocation(WeaponTipSocketName);
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}

void AAuraCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	//初始化主属性
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();// 创建 GameplayEffect 的上下文（Context），它包含来源、命中位置、投射物等信息
	ContextHandle.AddSourceObject(this);// 将当前对象（通常是一个 Ability 或 Actor）作为效果的“源对象”加入 Context
	//创建一个“效果规格”句柄（规格说明了要应用哪个效果、应用等级、上下文等）
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass,Level,ContextHandle);
	// 将这个效果规范（Spec）应用到目标上，通常目标就是自己（如果目标是敌人，也可以传入别的 ASC）
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(),GetAbilitySystemComponent());
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes,1.f);//将默认主要属性应用到自身
	ApplyEffectToSelf(DefaultSecondaryAttributes,1.f);//将默认次要属性应用到自身
	ApplyEffectToSelf(DefaultVitalAttributes,1.f);//将默认重要属性应用到自身
}

void AAuraCharacterBase::AddCharacterAbilities()
{
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	if (!HasAuthority()) return;//如果当前对象不是服务端（Authority），就直接返回，不执行后续逻辑。

	AuraASC->AddCharacterAbilities(StartupAbilities);
}



