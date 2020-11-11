// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MyAnimNotifyState_HitBox.generated.h"

/**
 * 
 */
UCLASS()
class MY2DTRY_API UMyAnimNotifyState_HitBox : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	TArray<FHitResult> HitResults;

	TArray<AActor*> HitActors;

	UPROPERTY(EditAnywhere, Category="HitBox", meta=(ToolTip="跟随的骨骼或插槽"))
	FName SocketName;

	UPROPERTY(EditAnywhere, Category="HitBox", meta=(ToolTip="", MakeEditWidget=true))
	FVector StartPos;

	UPROPERTY(EditAnywhere, Category="HitBox", meta=(ToolTip="", MakeEditWidget=true))
	FVector EndPos;

	UPROPERTY(EditAnywhere, Category="HitBox", meta=(ToolTip="插值数量"))
	int InterSection = 0;

	
	virtual void NotifyBegin(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float TotalDuration) override;
	virtual void NotifyTick(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation) override;

	// Overridden from UAnimNotifyState to provide custom notify name.
	FString GetNotifyName_Implementation() const override;
	
protected:
	bool ValidateParameters(USkeletalMeshComponent* MeshComp);
};
