// 青楼


#include "Character/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values主要用于初始化一些角色的组件，设置碰撞属性，并附加相关功能
AAuraCharacterBase::AAuraCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;// 设置角色每帧都调用 Tick() 函数。如果不需要频繁更新，可以关闭以提升性能。

	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get(); // 获取全局的游戏标签（GameplayTags），以便为后续操作引用
	
	BurnDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("BurnDebuffComponent");// 创建一个烧伤（Burn） debuff 组件，并将其附加到角色根组件上。
	BurnDebuffComponent->SetupAttachment(GetRootComponent());
	BurnDebuffComponent->DebuffTag = GameplayTags.Debuff_Burn;// 设置 debuff 标签为烧伤类型

	StunDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("StunDebuffComponent");// 创建一个眩晕（Stun） debuff 组件，并将其附加到角色根组件上。
	StunDebuffComponent->SetupAttachment(GetRootComponent());
	StunDebuffComponent->DebuffTag = GameplayTags.Debuff_Stun;// 设置 debuff 标签为眩晕类型

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);//让胶囊体碰撞体对相机通道忽略碰撞（防止相机被角色身体挡住）
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);//关闭角色胶囊碰撞体的重叠事件
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);// 让角色网格体（骨骼模型）对相机通道忽略碰撞（避免近距离相机穿模或挡视线）
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile,ECR_Overlap);// 设置 Mesh 对自定义投射物通道（ECC_Projectile）的碰撞响应为 Overlap（重叠）
	GetMesh()->SetGenerateOverlapEvents(true);// 开启网格生成重叠事件

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");//创建默认子对象来指定使用骨架网格物体组件并在引号中我们将拥有简单武器
	Weapon->SetupAttachment(GetMesh(),FName("WeaponHandSocket"));//将武器组件附加到角色骨骼网格WeaponHandSocket上
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);//关闭武器碰撞检测

	EffectAttachComponent = CreateDefaultSubobject<USceneComponent>("EffectAttachPoint");// 创建一个场景组件，用来作为所有特效的挂点（方便统一移动和旋转）
	EffectAttachComponent->SetupAttachment(GetRootComponent());// 把挂点组件附加到角色（或物体）的根组件上
	HaloOfProtectionNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("HaloOfProtectionComponent");// 创建被动特效组件：保护光环（Halo Of Protection）
	HaloOfProtectionNiagaraComponent->SetupAttachment(EffectAttachComponent);// 把保护光环特效附加到挂点组件上
	LifeSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("LifeSiphonNiagaraComponent");// 创建被动特效组件：生命吸取（Life Siphon）
	LifeSiphonNiagaraComponent->SetupAttachment(EffectAttachComponent);// 把生命吸取特效附加到挂点组件上
	ManaSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("ManaSiphonNiagaraComponent");// 创建被动特效组件：法力吸取（Mana Siphon）
	ManaSiphonNiagaraComponent->SetupAttachment(EffectAttachComponent);// 把法力吸取特效附加到挂点组件上
}

// 每帧调用一次，用于执行持续更新逻辑（例如动画、特效、状态检测等）
void AAuraCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	EffectAttachComponent->SetWorldRotation(FRotator::ZeroRotator);// 每帧将特效挂点（EffectAttachComponent）的世界旋转重置为零角度（即无旋转）
}

void AAuraCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);// 先调用父类的注册逻辑，确保继承的可复制变量也会被正确处理

	DOREPLIFETIME(AAuraCharacterBase,bIsStunned);// 告诉 Unreal：bIsStunned 这个变量需要被网络复制（从服务器同步到客户端）
	DOREPLIFETIME(AAuraCharacterBase,bIsBurned);// 告诉 Unreal：bIsBurned 这个变量需要被网络复制（从服务器同步到客户端）
	DOREPLIFETIME(AAuraCharacterBase,bIsBeingShocked);// 告诉 Unreal：bIsBurned 这个变量需要被网络复制（从服务器同步到客户端）
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;//命中反应montage
}

void AAuraCharacterBase::Die(const FVector& DeathImpulse)
{
	// 把武器（Weapon）从它当前挂载的组件上分离
	// 使用 FDetachmentTransformRules 来决定分离后的位移/旋转/缩放规则
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld,true));
	MulticastHandleDeath(DeathImpulse);//多播句柄死亡
}

FOnDeathSignature& AAuraCharacterBase::GetOnDeathDelegate()
{
	return OnDeathDelegate;
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	// 在角色当前位置和朝向播放死亡音效，使用全局工具类 UGameplayStatics
	UGameplayStatics::PlaySoundAtLocation(this,DeathSound,GetActorLocation(),GetActorRotation());
	
	Weapon->SetSimulatePhysics(true);// 开启物理模拟 → 武器不再由动画/代码控制位置，而是交给物理引擎计算
	Weapon->SetEnableGravity(true);// 启用重力 → 武器会像真实物体一样掉到地上
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);// 开启碰撞（仅物理碰撞，不用于射线检测）→ 避免武器穿过地面
	Weapon->AddImpulse(DeathImpulse * 0.1f, NAME_None, true);

	GetMesh()->SetSimulatePhysics(true);// 让角色的骨骼网格（SkeletalMesh）进入物理模拟 → 开启布娃娃效果
	GetMesh()->SetEnableGravity(true);// 开启重力，让骨骼网格受重力作用（倒地摔落）
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);// 设置碰撞只用于物理 → 角色 ragdoll 会跟地面/物体碰撞，但不再响应射线检测
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);// 设置和“世界静态物体”（地面、墙壁）碰撞响应为 Block → 避免 ragdoll 穿透地面
	GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);// 禁用角色的胶囊体碰撞 → 避免和 ragdoll 骨骼产生双重碰撞
	Dissolve();//溶解
	bDead = true; // 设置死亡标志
	BurnDebuffComponent->Deactivate();// 禁用燃烧 debuff
	StunDebuffComponent->Deactivate();// 禁用眩晕 debuff
	OnDeathDelegate.Broadcast(this);
}

void AAuraCharacterBase::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bIsStunned = NewCount > 0;// 当 NewCount > 0 时，说明角色正在受击 → bHitReacting = true，否则 = false
	GetCharacterMovement()->MaxWalkSpeed = bIsStunned ? 0.f : BaseWalkSpeed;// 如果正在受击 → 移动速度设为 0，禁止走动；否则恢复到基础移动速度
}

void AAuraCharacterBase::OnRep_Stunned()
{
	
}

void AAuraCharacterBase::OnRep_Burned()
{
}

// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	//TODO Return correct socket based on montageTag.根据 montageTag 返回正确的套接字
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();// 获取全局的 GameplayTags 单例，方便比较标签
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon) && IsValid(Weapon))// 如果技能蒙太奇标签是“武器攻击”，并且武器有效 → 返回武器尖端的插槽位置（常用于生成投射物、特效）
	{
		return Weapon->GetSocketLocation(WeaponTipSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))// 如果技能蒙太奇标签是“左手攻击” → 返回左手骨骼插槽位置
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))// 如果技能蒙太奇标签是“右手攻击” → 返回右手骨骼插槽位置
	{
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Tail))// 如果技能蒙太奇标签是“右手攻击” → 返回右手骨骼插槽位置
	{
		return GetMesh()->GetSocketLocation(TailSocketName);
	}
	return FVector();// 如果没有匹配到任何标签 → 返回一个零向量（相当于无效位置）
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

UNiagaraSystem* AAuraCharacterBase::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

FTaggedMontage AAuraCharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (FTaggedMontage TaggedMontage : AttackMontages)// 遍历 AttackMontages 数组中的每一个元素（类型是 FTaggedMontage）
	{
		if (TaggedMontage.MontageTage == MontageTag)// 如果当前元素的 MontageTage 等于传入的 MontageTag
		{
			return TaggedMontage;// 就返回这个 FTaggedMontage 作为结果
		}
	}
	return FTaggedMontage();
}

int32 AAuraCharacterBase::GetMinionCount_Implementation()
{
	return MinionCount;
}

void AAuraCharacterBase::IncrementMinionCount_Implementation(int32 Amount)
{
	MinionCount += Amount;
}

ECharacterClass AAuraCharacterBase::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

FOnASCRegistered& AAuraCharacterBase::GetOnASCRegisteredDelegate()
{
	return OnAscRegistered;
}

USkeletalMeshComponent* AAuraCharacterBase::GetWeapon_Implementation()
{
	return Weapon;
}

void AAuraCharacterBase::SetIsBeingShocked_Implementation(bool bInShock)
{
	bIsBeingShocked = bInShock;
}

bool AAuraCharacterBase::IsBeingShocked_Implementation() const
{
	return bIsBeingShocked;
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
	// 获取并检查角色的能力系统组件（AbilitySystemComponent），确保它是 UAuraAbilitySystemComponent 类型
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	if (!HasAuthority()) return;//如果当前对象不是服务端（Authority），就直接返回，不执行后续逻辑。

	AuraASC->AddCharacterAbilities(StartupAbilities); // 添加角色的启动能力（StartupAbilities），这通常是角色一开始就拥有的主动技能
	AuraASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);// 添加角色的启动被动能力（StartupPassiveAbilities），这些能力是角色从一开始就拥有的被动效果
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



