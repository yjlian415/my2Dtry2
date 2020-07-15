// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_AnyBranch.generated.h"

/**
 * 
 */
UCLASS()
class TRYNODE_API UK2Node_AnyBranch : public UK2Node
{
	GENERATED_BODY()
	
public:
	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual void PreloadRequiredAssets() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const;
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	//~ End  UEdGraphNode Interface

	//~ Begin UK2Node Interfaces
	virtual FText GetMenuCategory() const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph);
	FNodeHandlingFunctor* CreateNodeHandler(FKismetCompilerContext& CompilerContext) const;
	//~ End UK2Node Interfaces

	UEdGraphPin* AddUniqueConditionPin();
	FName GetUniquePinName();
	static FName GetPinNameGivenIndex(int32 Index);
	bool IsConditionPin(UEdGraphPin* pin);
	bool IsHaveUnlinkConditionPin();

	void RemovePin(UEdGraphPin* targetPin);

	//getter
	UEdGraphPin* GetThenPin();
	UEdGraphPin* GetElsePin();
	UEdGraphPin* GetConditionPinByIndex(int32 Index);
};
