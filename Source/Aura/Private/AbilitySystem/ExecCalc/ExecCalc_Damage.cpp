// 青楼


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

// 定义一个结构体，用来集中管理“伤害计算”过程中需要捕获的属性
struct AuraDamageStatics
{
	// 声明一个属性捕获定义，用来描述我们要捕获的属性（这里是 Armor）
	// DECLARE_ATTRIBUTE_CAPTUREDEF 会生成一个 FGameplayEffectAttributeCaptureDefinition 成员变量
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);//护甲
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);//护甲穿透
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);//格挡几率
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);//暴击几率
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);//暴击抗性
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);//暴击伤害
	
	AuraDamageStatics()// 构造函数：在这里初始化捕获逻辑
	{
		// DEFINE_ATTRIBUTE_CAPTUREDEF(属性集类, 属性名, 来源/目标, 是否快照)
		// UAuraAttributeSet::Armor 代表我们要捕获的属性
		// Target 表示从“目标角色”身上捕获
		// false 表示不做快照，每次都会动态获取最新值
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,Armor,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,ArmorPenetration,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,BlockChance,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,CriticalHitChance,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,CriticalHitResistance,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,CriticalHitDamage,Source,false);
	}
};

// 提供一个静态函数，返回全局唯一的 AuraDamageStatics 实例
// 这样可以避免每次都创建新的对象，保证效率和一致性
static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;// 静态局部变量：只会初始化一次，保证全局唯一

	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()// 构造函数：当 GEC（执行计算类）创建时会调用
{
	// RelevantAttributesToCapture 是一个数组，用来保存“需要捕获的属性”
	// DamageStatics().ArmorDef 就是我们在 AuraDamageStatics 里定义的 Armor 捕获定义
	// 这样 UE 才知道在执行计算时，要从 Target 上抓取 Armor 值
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();// 从 ExecutionParams 获取技能的来源（施法者）的 AbilitySystemComponent
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();// 从 ExecutionParams 获取技能的目标（被击中者）的 AbilitySystemComponent

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;// 从来源 ASC 拿到施法者的 AvatarActor（通常是角色 Pawn/Character）
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;// 从目标 ASC 拿到目标的 AvatarActor
	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);// 尝试将施法者的 Avatar 转成战斗接口类型
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);// 尝试将目标的 Avatar 转成战斗接口类型

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();// 获取当前正在执行的 GameplayEffectSpec（包含技能等级、标签、SetByCaller 参数等）

	const FGameplayTagContainer* SoureTags = Spec.CapturedSourceTags.GetAggregatedTags();// 从 Spec 中获取已经捕获的 Source 和 Target 的 GameplayTags（执行计算时引擎会提供）
	const FGameplayTagContainer* Targetags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;// 定义一个参数结构，传给 AttemptCalculateCapturedAttributeMagnitude，用来影响属性计算
	EvaluationParameters.SourceTags = SoureTags;// 来源的标签（可能影响属性，比如 Buff）
	EvaluationParameters.TargetTags = Targetags; // 目标的标签（可能影响属性，比如 Debuff）
	
	float Damage = 0.f;// 初始化总伤害为 0
	for (const TTuple<FGameplayTag, FGameplayTag>& Pair : FAuraGameplayTags::Get().DamageTypesToResistance)// 遍历所有在标签容器（GameplayTags）里定义的伤害类型，例如火焰、冰霜、雷电等
	{
		const float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key); // 从 Spec（技能效果规格）中读取该伤害类型的实际数值
		Damage += DamageTypeValue; // 把该类型的伤害值累加到总伤害中
	}
	
	// float Armor = 0.f;// 定义一个变量存储捕获到的 Armor 值
	// // 从 ExecutionParams 中尝试计算出目标的 Armor 属性数值
	// // - DamageStatics().ArmorDef = 捕获定义（告诉引擎抓取谁的什么属性）
	// // - EvaluationParameters = 标签修饰（有些属性可能依赖标签才生效）
	// // - Armor = 输出值
	// ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef,EvaluationParameters,Armor);
	// Armor = FMath::Max<float>(0.f,Armor);// 防止出现负数，把 Armor 最小限制为 0
	// ++Armor;// 这里你写了 ++Armor，相当于额外加了 1（有点像给护甲做基准修正）
	//const FGameplayModifierEvaluatedData EvaluatedData(DamageStatics().ArmorProperty,EGameplayModOp::Additive,Armor);

	// 创建修正数据，告诉 ASC 要修改哪个属性以及怎么改
	// - UAuraAttributeSet::GetIncomingDamageAttribute() → 我们定义的“即将受到的伤害”属性
	// - EGameplayModOp::Additive → 以加法方式叠加
	// - Damage → 刚才计算出来的伤害值
	//const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(),EGameplayModOp::Additive,Damage);

	//capture BlockChance on Target,and Determine if there was a successful Block 捕获目标上的 BlockChance，并确定是否有成功的 Block
	float TargetBlockChance = 0.f;
	// 尝试从目标的属性集中抓取 BlockChance（格挡几率），结果放到 TargetBlockChance 中
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef,EvaluationParameters,TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance,0.f);// 防止格挡率为负数，最小限制为 0
	const bool bBlocked = FMath::RandRange(1,100) < TargetBlockChance;// 随机生成 1~100 的整数，判断是否小于格挡几率（TargetBlockChance），如果是则触发格挡

	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();// 从 GameplayEffectSpec 中获取 GameplayEffectContext 的句柄
	
	// 3. 设置格挡状态到上下文如果 bBlocked = true，就在 Context 里写入“格挡命中”标记 后续任何地方都能通过 EffectContextHandle 查询这次攻击是否格挡
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle,bBlocked);
	
	//If Block,halve the damage. 如果格挡，则将伤害减半。
	Damage = bBlocked ? Damage / 2.f : Damage;// 如果触发格挡，就把伤害减半，否则伤害保持不变

	//ArmorPenetration ignores a percentage of the Target's Armor 护甲穿透会忽略目标一定比例的护甲
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef,EvaluationParameters,TargetArmor);
	TargetArmor = FMath::Max<float>(TargetArmor,0.f);

	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef,EvaluationParameters,SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration,0.f);

	// 从施法者 Avatar 获取角色职业信息类（CharacterClassInfo）这里封装了角色的基础属性、成长曲线、技能系数等
	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	// 从角色信息里的 DamageCalculationCoefficients（伤害计算系数数据）里找到名为 "ArmorPenetration" 的曲线
	// FindCurve 参数：FName("ArmorPenetration") = 曲线名，FString() = 子曲线名（可选，留空则找主曲线）
	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"),FString());
	// 根据施法者等级（从 CombatInterface 获取）在曲线上评估对应的护甲穿透系数
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetPlayerLevel());

	// 先计算“有效护甲”- TargetArmor = 目标的护甲- SourceArmorPenetration = 攻击方的护甲穿透百分比 - 乘以 0.25f 表示：穿透只发挥 25% 的效率（相当于削弱护甲，而不是完全无效化）- (100 - X) / 100.f 把百分比转成倍率
	const float EffectiveArmor = TargetArmor * ( 100 - SourceArmorPenetration * ArmorPenetrationCoefficient ) / 100.f;

	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"),FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());
	// 计算最终伤害缩放 - 有效护甲每 1 点 ≈ 0.333% 的减伤（这里用 *0.333f 来近似） - (100 - 减伤%) / 100.f 得到最终伤害倍率
	Damage *= ( 100 - EffectiveArmor * EffectiveArmorCoefficient ) / 100.f;

	float SourceCriticalHitChance = 0.f;// 从来源 ASC 抓取暴击率（CriticalHitChance），初始设为 0
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().CriticalHitChanceDef,// 要抓取的属性定义
		EvaluationParameters,// 计算参数（包含 Source/Target ASC）
		SourceCriticalHitChance   // 输出参数
		);
	SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance,0.f);// 防止出现负数，保证暴击率最小为 0

	// 从目标 ASC 抓取暴击抗性（CriticalHitResistance）
	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef,EvaluationParameters,TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(TargetCriticalHitResistance,0.f);

	// 从来源 ASC 抓取暴击伤害加成（CriticalHitDamage）
	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef,EvaluationParameters,SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage,0.f);

	// 从角色职业信息中获取伤害计算相关系数表里的 "CriticalHitResistance" 曲线
	const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"),FString());
	// 根据目标玩家等级，从曲线上评估对应的暴击抗性系数
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetCombatInterface->GetPlayerLevel());
	
	// Critical Hit Resistance reduces Critical Hit Chance by a certain percentage暴击抗性使暴击率降低一定百分比
	// 计算有效暴击率：来源暴击率 - (目标暴击抗性 * 系数) 系数 0.15f 表示：每点抗性降低 0.15% 的暴击几率
	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
	const bool bCriticalHit = FMath::RandRange(1,100) < EffectiveCriticalHitChance;// 用随机数判定是否暴击，范围 1~100

	// 3. 把暴击结果写进 EffectContext 和格挡一样，把“是否暴击”记到上下文 方便后续伤害结算和 UI 查询 
	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle,bCriticalHit);

	//Double damage plus a bonus if critical hit双倍伤害加上暴击时加成
	// 6. 如果暴击，造成2倍伤害 + 暴击伤害加成
	//    例如：基础伤害100，暴击伤害加成为20，则暴击伤害 = 100*2 + 20 = 220
	Damage = bCriticalHit ? 2.f * Damage + SourceCriticalHitDamage : Damage;// 计算最终伤害：如果暴击 = 原伤害 × 2 + 暴击加成，否则 = 原伤害
	
	// 创建修正数据对象：表示要对目标的 IncomingDamage 属性做“加法修正”，加的值是最终伤害
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);// 把修正结果写入输出，Execution 完成后会应用到目标属性
	
}
