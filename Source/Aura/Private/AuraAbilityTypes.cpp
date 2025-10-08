#include "AuraAbilityTypes.h"

//实现网络序列化
bool FAuraGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	uint32 RepBits = 0;// 用一个 32 位无符号整数作为“位掩码”，每一位代表一个要不要同步的属性
	if (Ar.IsSaving())// 判断当前是否在进行保存（序列化时保存阶段）
	{
		if (bReplicateInstigator && Instigator.IsValid())
		{
			RepBits |= 1 << 0;// 如果需要同步 Instigator 且有效，就把第 0 位设为 1
		}
		if (bReplicateEffectCauser && EffectCauser.IsValid() )
		{
			RepBits |= 1 << 1;// 如果需要同步 EffectCauser 且有效，就把第 1 位设为 1
		}
		if (AbilityCDO.IsValid())
		{
			RepBits |= 1 << 2;// 如果 Ability 的类默认对象有效，就把第 2 位设为 1
		}
		if (bReplicateSourceObject && SourceObject.IsValid())
		{
			RepBits |= 1 << 3;// 如果需要同步 SourceObject 且有效，就把第 3 位设为 1
		}
		if (Actors.Num() > 0)
		{
			RepBits |= 1 << 4;// 如果有 Actor 列表，就把第 4 位设为 1
		}
		if (HitResult.IsValid())
		{
			RepBits |= 1 << 5;// 如果命中结果有效，就把第 5 位设为 1
		}
		if (bHasWorldOrigin)
		{
			RepBits |= 1 << 6;// 如果有世界坐标，就把第 6 位设为 1
		}
		if (bIsBlockedHit)
		{
			RepBits |= 1 << 7; // 如果是阻挡命中，就把第 7 位设为 1
		}
		if (bIsCriticalHit)
		{
			RepBits |= 1 << 8;// 如果是暴击，就把第 8 位设为 1
		}
		if (bIsSuccessfulDebuff)
		{
			RepBits |= 1 << 9;// 如果是成功的减益，就把第 9 位设为 1
		}
		if (DebuffDamage > 0.f)
		{
			RepBits |= 1 << 10;
		}
		if (DebuffDuration > 0.f)
		{
			RepBits |= 1 << 11;
		}
		if (DebuffFrequency > 0.f)
		{
			RepBits |= 1 << 12;
		}
		if (DamageType.IsValid())
		{
			RepBits |= 1 << 13;
		}
		if (!DeathImpulse.IsZero())
		{
			RepBits |= 1 << 14;
		}
	}
	
	Ar.SerializeBits(&RepBits, 14);// 先读取/写入 9 位二进制数据到 RepBits（对应之前保存的9个标志位）这里 9 表示最多用到 0~8 位（也就是你定义的9个属性）

	if (RepBits & (1 << 0))
	{
		Ar << Instigator;// 如果第0位是1，就序列化 Instigator
	}
	if (RepBits & (1 << 1))
	{
		Ar << EffectCauser;// 如果第1位是1，就序列化 EffectCauser
	}
	if (RepBits & (1 << 2))
	{
		Ar << AbilityCDO; // 如果第2位是1，就序列化 AbilityCDO
	}
	if (RepBits & (1 << 3))
	{
		Ar << SourceObject; // 如果第3位是1，就序列化 SourceObject
	}
	if (RepBits & (1 << 4))
	{
		SafeNetSerializeTArray_Default<31>(Ar, Actors);// 如果第4位是1，就序列化 Actors 数组 这里用 SafeNetSerializeTArray_Default 来保证安全的数组序列化，最多31个元素
	}
	if (RepBits & (1 << 5))
	{
		if (Ar.IsLoading())
		{
			if (!HitResult.IsValid())
			{
				HitResult = TSharedPtr<FHitResult>(new FHitResult());// 如果正在加载（反序列化）并且 HitResult 还没创建，就新建一个
			}
		}
		HitResult->NetSerialize(Ar, Map, bOutSuccess); // 调用 HitResult 自带的 NetSerialize 进行网络序列化/反序列化
	}
	if (RepBits & (1 << 6))
	{
		Ar << WorldOrigin;
		bHasWorldOrigin = true;// 如果第6位是1，就序列化 WorldOrigin，并标记 bHasWorldOrigin = true
	}
	else
	{
		bHasWorldOrigin = false;// 否则说明没有 WorldOrigin
	}
	if (RepBits & (1 << 7))
	{
		Ar << bIsBlockedHit;// 如果第7位是1，就序列化是否被阻挡命中
	}
	if (RepBits & (1 << 8))
	{
		Ar << bIsCriticalHit; // 如果第8位是1，就序列化是否是暴击
	}
	if (RepBits & (1 << 9))
	{
		Ar << bIsSuccessfulDebuff; // 如果第8位是1，就序列化是否是暴击
	}
	if (RepBits & (1 << 10))
	{
		Ar << DebuffDamage; // 如果第8位是1，就序列化是否是暴击
	}
	if (RepBits & (1 << 11))
	{
		Ar << DebuffDuration; // 如果第8位是1，就序列化是否是暴击
	}
	if (RepBits & (1 << 12))
	{
		Ar << DebuffFrequency; // 如果第8位是1，就序列化是否是暴击
	}
	if (RepBits & (1 << 13))
	{
		if (Ar.IsLoading())
		{
			if (!DamageType.IsValid())
			{
				DamageType = TSharedPtr<FGameplayTag>(new FGameplayTag());// 如果正在加载（反序列化）并且 HitResult 还没创建，就新建一个
			}
		}
		DamageType->NetSerialize(Ar, Map, bOutSuccess); // 调用 HitResult 自带的 NetSerialize 进行网络序列化/反序列化
	}
	if (RepBits & (1 << 14))
	{
		DeathImpulse.NetSerialize(Ar, Map, bOutSuccess);
	}

	if (Ar.IsLoading())
	{
		AddInstigator(Instigator.Get(), EffectCauser.Get()); // 只是为了初始化 InstigatorAbilitySystemComponent
	}	
	
	bOutSuccess = true;
	
	return true;
}
