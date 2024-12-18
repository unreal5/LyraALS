// Lyra高级运动系统

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "空闲动画")
	TObjectPtr<UAnimSequenceBase> CrouchingIdleAnim;

	UFUNCTION(BlueprintCallable, Category = "Idle|绑定函数", meta = (BlueprintThreadSafe))
	void IdleOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	// Cycle的Walk和Jogging的动画
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cycle|动画", meta=(ToolTip="动画顺序: 前, 后, 右, 左"))
	TArray<TObjectPtr<UAnimSequenceBase>> WalkCycleAnims;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cycle|动画", meta=(ToolTip="动画顺序: 前, 后, 右, 左"))
	TArray<TObjectPtr<UAnimSequenceBase>> JoggingCycleAnims;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cycle|动画", meta=(ToolTip="动画顺序: 前, 后, 右, 左"))
	TArray<TObjectPtr<UAnimSequenceBase>> CrouchCycleAnims;
	/** Cycle 绑定函数 */
	UFUNCTION(BlueprintCallable, Category = "Cycle|绑定函数", meta = (BlueprintThreadSafe))
	void CycleOnBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable, Category = "Cycle|绑定函数", meta = (BlueprintThreadSafe))
	void CycleOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	/** Stop 绑定函数 */
	UFUNCTION(BlueprintCallable, Category = "Stop|绑定函数", meta = (BlueprintThreadSafe))
	void StopOnBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable, Category = "Stop|绑定函数", meta = (BlueprintThreadSafe))
	void StopOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	// Stop的动画
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stop|动画", meta=(ToolTip="动画顺序: 前, 后, 右, 左"))
	TArray<TObjectPtr<UAnimSequenceBase>> WalkStopAnims;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stop|动画", meta=(ToolTip="动画顺序: 前, 后, 右, 左"))
	TArray<TObjectPtr<UAnimSequenceBase>> JoggingStopAnims;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stop|动画", meta=(ToolTip="动画顺序: 前, 后, 右, 左"))
	TArray<TObjectPtr<UAnimSequenceBase>> CrouchStopAnims;
	// Start的动画
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Start|动画", meta=(ToolTip="动画顺序: 前, 后, 右, 左"))
	TArray<TObjectPtr<UAnimSequenceBase>> WalkStartAnims;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Start|动画", meta=(ToolTip="动画顺序: 前, 后, 右, 左"))
	TArray<TObjectPtr<UAnimSequenceBase>> JoggingStartAnims;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Start|动画", meta=(ToolTip="动画顺序: 前, 后, 右, 左"))
	TArray<TObjectPtr<UAnimSequenceBase>> CrouchStartAnims;
	/** Start 绑定函数 */
	UFUNCTION(BlueprintCallable, Category = "Start|绑定函数", meta = (BlueprintThreadSafe))
	void StartOnBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable, Category = "Start|绑定函数", meta = (BlueprintThreadSafe))
	void StartOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);


	/** Pivot 绑定函数 */
	UFUNCTION(BlueprintCallable, Category = "Pivot|绑定函数", meta = (BlueprintThreadSafe))
	void PivotOnBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable, Category = "Pivot|绑定函数", meta = (BlueprintThreadSafe))
	void PivotOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	// Pivot的动画
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pivot|动画", meta=(ToolTip="动画顺序: 前, 后, 右, 左"))
	TArray<TObjectPtr<UAnimSequenceBase>> WalkPivotAnims;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pivot|动画", meta=(ToolTip="动画顺序: 前, 后, 右, 左"))
	TArray<TObjectPtr<UAnimSequenceBase>> JoggingPivotAnims;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pivot|动画", meta=(ToolTip="动画顺序: 前, 后, 右, 左"))
	TArray<TObjectPtr<UAnimSequenceBase>> CrouchPivotAnims;
	/** TurnInPlace */
	UPROPERTY(BlueprintReadWrite, Category = "TurnInPlace|动画")
	bool bShouldTurnLeft = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TurnInPlace")
	TObjectPtr<UAnimSequenceBase> TurnInPlaceLeftAnim90;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TurnInPlace")
	TObjectPtr<UAnimSequenceBase> TurnInPlaceRightAnim90;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TurnInPlace")
	TObjectPtr<UAnimSequenceBase> TurnInPlaceLeftAnim180;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TurnInPlace")
	TObjectPtr<UAnimSequenceBase> TurnInPlaceRightAnim180;

	UFUNCTION(BlueprintCallable, Category = "TurnInPlace|绑定函数", meta = (BlueprintThreadSafe))
	void SetupTurnInPlaceEntryState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable, Category = "TurnInPlace|绑定函数", meta = (BlueprintThreadSafe))
	void TurnInPlaceBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable, Category = "TurnInPlace|绑定函数", meta = (BlueprintThreadSafe))
	void TurnInPlaceOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	// UFUNCTION(BlueprintCallable, Category = "TurnInPlace|绑定函数", meta = (BlueprintThreadSafe))
	// void TurnInPlaceRecoveryBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	// UFUNCTION(BlueprintCallable, Category = "TurnInPlace|绑定函数", meta = (BlueprintThreadSafe))
	// void TurnInPlaceRecoveryOnUpdate(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category="TurnInPlace")
	float TurnInPlaceTime = 0.f;
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category="TurnInPlace")
	TObjectPtr<UAnimSequenceBase> FinalTurnInPlaceAnim = nullptr;
};
