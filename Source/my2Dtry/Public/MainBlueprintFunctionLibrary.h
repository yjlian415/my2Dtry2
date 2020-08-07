// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MainBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class MY2DTRY_API UMainBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
 	UFUNCTION(BlueprintCallable, Category="Input", meta=(DefaultToSelf="pController"))
 	static bool GenerateInputKey(APlayerController* pController, FKey key, EInputEvent EventType, float AmountDepressed);

	UFUNCTION(BlueprintCallable, Category="ZTest")
	static int TestAdd(int a, int b);
};
