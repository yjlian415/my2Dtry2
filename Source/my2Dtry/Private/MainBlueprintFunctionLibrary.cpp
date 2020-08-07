// Fill out your copyright notice in the Description page of Project Settings.


#include "MainBlueprintFunctionLibrary.h"
#include "GameFramework/PlayerController.h"
 
bool UMainBlueprintFunctionLibrary::GenerateInputKey(APlayerController* pController, FKey key, EInputEvent EventType, float AmountDepressed)
{
	if (pController)
		return pController->InputKey(key, EventType, AmountDepressed, false);
	return false;
}

int UMainBlueprintFunctionLibrary::TestAdd(int a, int b)
{
	return a + b;
}