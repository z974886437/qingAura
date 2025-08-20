// 青楼


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"

// 定义一个结构体，用来集中管理“伤害计算”过程中需要捕获的属性
struct AuraDamageStatics
{
	// 声明一个属性捕获定义，用来描述我们要捕获的属性（这里是 Armor）
	// DECLARE_ATTRIBUTE_CAPTUREDEF 会生成一个 FGameplayEffectAttributeCaptureDefinition 成员变量
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	
	AuraDamageStatics()// 构造函数：在这里初始化捕获逻辑
	{
		// DEFINE_ATTRIBUTE_CAPTUREDEF(属性集类, 属性名, 来源/目标, 是否快照)
		// UAuraAttributeSet::Armor 代表我们要捕获的属性
		// Target 表示从“目标角色”身上捕获
		// false 表示不做快照，每次都会动态获取最新值
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,Armor,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,BlockChance,Target,false);
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
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();// 从 ExecutionParams 获取技能的来源（施法者）的 AbilitySystemComponent
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();// 从 ExecutionParams 获取技能的目标（被击中者）的 AbilitySystemComponent

	const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;// 从来源 ASC 拿到施法者的 AvatarActor（通常是角色 Pawn/Character）
	const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;// 从目标 ASC 拿到目标的 AvatarActor

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();// 获取当前正在执行的 GameplayEffectSpec（包含技能等级、标签、SetByCaller 参数等）

	const FGameplayTagContainer* SoureTags = Spec.CapturedSourceTags.GetAggregatedTags();// 从 Spec 中获取已经捕获的 Source 和 Target 的 GameplayTags（执行计算时引擎会提供）
	const FGameplayTagContainer* Targetags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;// 定义一个参数结构，传给 AttemptCalculateCapturedAttributeMagnitude，用来影响属性计算
	EvaluationParameters.SourceTags = SoureTags;// 来源的标签（可能影响属性，比如 Buff）
	EvaluationParameters.TargetTags = Targetags; // 目标的标签（可能影响属性，比如 Debuff）

	// Get Damage Set by Caller Magnitude
	// 从 Spec 中获取 SetByCaller 的伤害值
	// 在技能释放时我们会用 AssignTagSetByCallerMagnitude 来动态指定这个值
	float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);
	
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
	//If Block,halve the damage. 如果格挡，则将伤害减半。
	float TargetBlockChance = 0.f;
	// 尝试从目标的属性集中抓取 BlockChance（格挡几率），结果放到 TargetBlockChance 中
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef,EvaluationParameters,TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance,0.f);// 防止格挡率为负数，最小限制为 0

	const bool bBlocked = FMath::RandRange(1,100) < TargetBlockChance;// 随机生成 1~100 的整数，判断是否小于格挡几率（TargetBlockChance），如果是则触发格挡
	Damage = bBlocked ? Damage / 2.f : Damage;// 如果触发格挡，就把伤害减半，否则伤害保持不变

	// 创建修正数据对象：表示要对目标的 IncomingDamage 属性做“加法修正”，加的值是最终伤害
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);// 把修正结果写入输出，Execution 完成后会应用到目标属性
}
