// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimNotifyState_HitBox.h"
#include "DrawDebugHelpers.h"

void UMyAnimNotifyState_HitBox::NotifyBegin(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float TotalDuration)
{
	MeshComp->GetOwner();
	if (MeshComp->DoesSocketExist(SocketName))
	{
		auto pSocket = MeshComp->GetSocketByName(SocketName);
	}

	Received_NotifyBegin(MeshComp, Animation, TotalDuration);
}

void UMyAnimNotifyState_HitBox::NotifyTick(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float FrameDeltaTime)
{
	if (MeshComp->DoesSocketExist(SocketName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Socket Pos: %s"), *MeshComp->GetSocketLocation(SocketName).ToString());
	}
	
	Received_NotifyTick(MeshComp, Animation, FrameDeltaTime);
}

void UMyAnimNotifyState_HitBox::NotifyEnd(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation)
{
	Received_NotifyEnd(MeshComp, Animation);
}

FString UMyAnimNotifyState_HitBox::GetNotifyName_Implementation() const
{
	return UAnimNotifyState::GetNotifyName_Implementation();
}

bool UMyAnimNotifyState_HitBox::ValidateParameters(USkeletalMeshComponent* MeshComp)
{
	return true;
}
