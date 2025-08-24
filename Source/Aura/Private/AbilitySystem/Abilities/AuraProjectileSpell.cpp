// 青楼


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "Aura/Public/AuraGameplayTags.h"

//#include "Kismet/KismetSystemLibrary.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


	//UKismetSystemLibrary::PrintString(this,FString("ActivateAbility (C++)"),true,true,FLinearColor::Yellow,3);//界面显示C++调试
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();//判断当前逻辑是否在服务器端执行
	if (!bIsServer) return;// 客户端直接返回，不生成投射物

	// 尝试将技能的执行者转换为战斗接口（ICombatInterface）
	// 这样可以从接口获取武器插槽位置等信息，解耦角色实现
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (CombatInterface)
	{
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();// 获取战斗插槽（比如武器枪口、法杖末端）的世界位置
		// 计算从发射点指向目标点的旋转角度（让投射物面向目标方向）
		// (目标位置 - 发射位置) 得到方向向量，再用 Rotation() 转成 FRotator
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		
		FTransform SpawnTransform;// 用于描述生成位置、旋转、缩放的变换数据
		SpawnTransform.SetLocation(SocketLocation);// 设置生成位置（旋转稍后可设置）
		//ToDo:Set the Projectile Rotation 设置弹丸旋转
		SpawnTransform.SetRotation(Rotation.Quaternion());// 将计算好的朝向角（FRotator）转成四元数，设置到生成变换中
		
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(// 延迟生成一个 AAuraProjectile 类型的 Actor
			ProjectileClass,// 要生成的类（这里是你的投射物类，比如火球）
			SpawnTransform, // 生成的位置信息（FTransform 包含位置、旋转、缩放）
			GetOwningActorFromActorInfo(), // 拥有者（Outer），这里是技能的拥有者 Actor
			Cast<APawn>(GetOwningActorFromActorInfo()), // 生成时的 Instigator（通常是触发攻击的 Pawn）
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn// 冲突处理方式：总是生成，即使和其他物体重叠
			);

		//  Give the Projectile a Gameplay Effect Spec for causing Damage 为射弹提供造成伤害的游戏效果规格
		// 从当前技能 ActorInfo 中获取 Avatar（通常是角色 Pawn/Character），再取得它的 ASC
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();// 创建一个 GameplayEffectContextHandle，用来存放技能施放时的上下文信息
		EffectContextHandle.SetAbility(this);// 把当前技能自身绑定到 Context，便于后续追溯来源
		EffectContextHandle.AddSourceObject(Projectile);// 把投射物 Projectile 作为 SourceObject 加入上下文，用来标识效果的来源对象	
		TArray<TWeakObjectPtr<AActor>> Actors;// 添加相关的 Actor（此处是 Projectile），TWeakObjectPtr 确保不会导致强引用内存泄漏
		Actors.Add(Projectile);
		EffectContextHandle.AddActors(Actors);
		FHitResult HitResult;// 构造一次命中结果，用来记录投射物命中的位置
		HitResult.Location = ProjectileTargetLocation;
		EffectContextHandle.AddHitResult(HitResult);// 把命中结果加入 Context，方便伤害计算、表现（例如在命中位置生成特效）
		
		// 创建一个指定类型的 GameplayEffectSpec，用于之后应用到目标
		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass,GetAbilityLevel(),EffectContextHandle);

		const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();// 先获取全局的 GameplayTags（项目里定义的单例，存放所有标签）

		for (auto& Pair : DamageTypes)
		{
			const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());// 根据技能等级，从 Damage（曲线或数值表）中获取对应的伤害数值
			// 给即将应用的 GameplayEffectSpec（SpecHandle）添加一个 “SetByCaller” 类型的数值
			// 参数含义： SpecHandle → 目标 GameplayEffectSpec  Pair.Key   → 当前伤害类型对应的 GameplayTag（如 Damage.Fire / Damage.Ice） ScaledDamage → 本次实际计算出来的伤害值
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,Pair.Key,ScaledDamage);// 2. 把伤害值写进 GameplayEffectSpec 的 SetByCaller
		}
		
		Projectile->DamageEffectSpecHandle = SpecHandle;// 把带有伤害数值的 SpecHandle 交给 Projectile（投射物），
		
		Projectile->FinishSpawning(SpawnTransform);// 完成生成，开始在世界中生效
	}
}
