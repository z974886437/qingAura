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
	return HitReactMontage;//命中反应montage
}

void AAuraCharacterBase::Die()
{
	// 把武器（Weapon）从它当前挂载的组件上分离
	// 使用 FDetachmentTransformRules 来决定分离后的位移/旋转/缩放规则
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld,true));
	MulticastHandleDeath();//多播句柄死亡
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation()
{
	Weapon->SetSimulatePhysics(true);// 开启物理模拟 → 武器不再由动画/代码控制位置，而是交给物理引擎计算
	Weapon->SetEnableGravity(true);// 启用重力 → 武器会像真实物体一样掉到地上
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);// 开启碰撞（仅物理碰撞，不用于射线检测）→ 避免武器穿过地面

	GetMesh()->SetSimulatePhysics(true);// 让角色的骨骼网格（SkeletalMesh）进入物理模拟 → 开启布娃娃效果
	GetMesh()->SetEnableGravity(true);// 开启重力，让骨骼网格受重力作用（倒地摔落）
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);// 设置碰撞只用于物理 → 角色 ragdoll 会跟地面/物体碰撞，但不再响应射线检测
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);// 设置和“世界静态物体”（地面、墙壁）碰撞响应为 Block → 避免 ragdoll 穿透地面
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);// 禁用角色的胶囊体碰撞 → 避免和 ragdoll 骨骼产生双重碰撞
	Dissolve();//溶解
	bDead = true;
}

// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation()
{
	check(Weapon);
	return Weapon->GetSocketLocation(WeaponTipSocketName);
}

bool AAuraCharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* AAuraCharacterBase::GetAvatar_Implementation() 
{
	return this;
}

TArray<FTaggedMontage> AAuraCharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontages;
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

void AAuraCharacterBase::Dissolve()
{
	if (IsValid(DissolveMaterialInstance))// 如果 DissolveMaterialInstance 有效（非空且未被销毁）
	{
		// 创建一个动态材质实例（Dynamic Material Instance）,参数1：源材质（DissolveMaterialInstance）,参数2：Outer，绑定到当前对象生命周期
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance,this);
		GetMesh()->SetMaterial(0,DynamicMatInst);// 将角色网格（SkeletalMesh）第0号材质槽替换为动态材质实例,这样就可以在运行时修改 DynamicMatInst 的参数
		StartDissolveTimeline(DynamicMatInst);//启动溶解时间轴
	}
	if (IsValid(WeaponDissolveMaterialInstance))
	{
		// 创建一个动态材质实例（Dynamic Material Instance）,参数1：源材质（WeaponDissolveMaterialInstance）,参数2：Outer，绑定到当前对象生命周期
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance,this);
		Weapon->SetMaterial(0,DynamicMatInst);// 将角色网格（SkeletalMesh）第0号材质槽替换为动态材质实例,这样就可以在运行时修改 DynamicMatInst 的参数
		StartWeaponDissolveTimeline(DynamicMatInst);//启动武器溶解时间轴
	}
}



