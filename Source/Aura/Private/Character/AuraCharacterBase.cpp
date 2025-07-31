// 青楼


#include "Character/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "WorldPartition/Cook/WorldPartitionCookPackage.h"

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");//创建默认子对象来指定使用骨架网格物体组件并在引号中我们将拥有简单武器
	Weapon->SetupAttachment(GetMesh(),FName("WeaponHandSocket"));//将武器组件附加到角色骨骼网格WeaponHandSocket上
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);//关闭武器碰撞检测

}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}

void AAuraCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	//初始化主属性
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass,Level,ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(),GetAbilitySystemComponent());
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes,1.f);//将默认主要属性应用到自身
	ApplyEffectToSelf(DefaultSecondaryAttributes,1.f);//将默认次要属性应用到自身
}



