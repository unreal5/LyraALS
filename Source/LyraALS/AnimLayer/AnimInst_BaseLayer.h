// Lyra高级运动系统

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enums/Enums.h"
#include "AnimInst_BaseLayer.generated.h"

class ULyraAnimInstance;
class ALyraCharacter;
struct FAnimNodeReference;
struct FAnimUpdateContext;
/**
 * 动画分层基类
 */
UCLASS()
class LYRAALS_API UAnimInst_BaseLayer : public UAnimInstance
{
	GENERATED_BODY()

protected:
	/* 公共辅助函数 */
	UFUNCTION(BlueprintPure, Category = "设置|辅助函数", meta=(BlueprintThreadSafe))
	ALyraCharacter* GetLyraCharacter();
	UFUNCTION(BlueprintPure, Category = "设置|辅助函数", meta=(BlueprintThreadSafe))
	ULyraAnimInstance* GetABPBase();
	/* 空闲动画 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "空闲动画")
	TObjectPtr<UAnimSequenceBase> IdleAnim;


	UFUNCTION(BlueprintCallable, Category = "Idle|绑定函数", meta = (BlueprintThreadSafe))
	void IdleOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	// Cycle的Walk和Jogging的动画
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cycle|Walk动画", meta=(ToolTip="动画顺序: 前, 后, 右, 左"))
	TArray<TObjectPtr<UAnimSequenceBase>> WalkCycleAnims;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cycle|Jogging动画", meta=(ToolTip="动画顺序: 前, 后, 右, 左"))
	TArray<TObjectPtr<UAnimSequenceBase>> JoggingCycleAnims;

	UFUNCTION(BlueprintCallable, Category = "Cycle|绑定函数", meta = (BlueprintThreadSafe))
	void CycleOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

private:
	UAnimSequenceBase* SelectAnimBy(const TArray<TObjectPtr<UAnimSequenceBase>>& InWalkAnims,
	                                const TArray<TObjectPtr<UAnimSequenceBase>>& InJoggingAnims, EGait Gait,
	                                ELocomotionDirection Direction);
};