// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "CameraGuideAnchorComp.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = "CameraGuide", hidecategories = (Object, LOD, Lighting, TextureStreaming), editinlinenew, meta = (DisplayName = "Camera Guide Anchor", BlueprintSpawnableComponent))
class MY2DTRY_API UCameraGuideAnchorComp : public UBoxComponent
{
	GENERATED_BODY()
	FCalculateCustomPhysics OnCalculateCustomPhysics;
public:
	UCameraGuideAnchorComp(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float focusWeight;
};
