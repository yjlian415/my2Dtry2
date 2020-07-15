// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_BothBranch.h"

#include "tryNodePCH.h"

#include "EdGraph/EdGraphPin.h"
#include "GraphEditorSettings.h"
#include "EdGraphSchema_K2.h"
#include "EdGraphUtilities.h"
#include "KismetCompilerMisc.h"
#include "KismetCompiler.h"
#include "BlueprintNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "EditorStyleSet.h"
#include "BlueprintActionDatabaseRegistrar.h"


//Compile Handler
class FKCHandler_BothBranch : public FNodeHandlingFunctor
{
public:
	FKCHandler_BothBranch(FKismetCompilerContext& InCompilerContext, int32 conditionNum)
		: FNodeHandlingFunctor(InCompilerContext), m_conditionNum(conditionNum)
	{
	}

	int32 m_conditionNum = 1;
	virtual void Compile(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		// For imperative nodes, make sure the exec function was actually triggered and not just included due to an output data dependency
		FEdGraphPinType ExpectedExecPinType;
		ExpectedExecPinType.PinCategory = UEdGraphSchema_K2::PC_Exec;

		FEdGraphPinType ExpectedBoolPinType;
		ExpectedBoolPinType.PinCategory = UEdGraphSchema_K2::PC_Boolean;


		UEdGraphPin* ExecTriggeringPin = Context.FindRequiredPinByName(Node, UEdGraphSchema_K2::PN_Execute, EGPD_Input);
		if ((ExecTriggeringPin == NULL) || !Context.ValidatePinType(ExecTriggeringPin, ExpectedExecPinType))
		{
			CompilerContext.MessageLog.Error(*LOCTEXT("NoValidExecutionPinForBranch_Error", "@@ must have a valid execution pin @@").ToString(), Node, ExecTriggeringPin);
			return;
		}
		else if (ExecTriggeringPin->LinkedTo.Num() == 0)
		{
			CompilerContext.MessageLog.Warning(*LOCTEXT("NodeNeverExecuted_Warning", "@@ will never be executed").ToString(), Node);
			return;
		}

		// Generate the output impulse from this node
		TArray<UEdGraphPin*> CondPins;
		for (int32 i = 0; i < Node->Pins.Num() - 3; ++i)
		{
			UEdGraphPin* condPin = Context.FindRequiredPinByName(Node, UK2Node_BothBranch::GetPinNameGivenIndex(i), EGPD_Input);
			if (condPin && Context.ValidatePinType(condPin, ExpectedBoolPinType))
			{
				CondPins.Add(condPin);
			}
			else 
			{
				CompilerContext.MessageLog.Warning(*LOCTEXT("NodeNeverExecuted_Warning", "@@ number @@ pin is not boolen").ToString(), Node, condPin);
				return;
			}
		}

		UEdGraphPin* ThenPin = Context.FindRequiredPinByName(Node, UEdGraphSchema_K2::PN_Then, EGPD_Output);
		UEdGraphPin* ElsePin = Context.FindRequiredPinByName(Node, UEdGraphSchema_K2::PN_Else, EGPD_Output);
		if (Context.ValidatePinType(ThenPin, ExpectedExecPinType) &&
			Context.ValidatePinType(ElsePin, ExpectedExecPinType))
		{
			int32 condIndex = 0;
			for (auto CondPin : CondPins)
			{
				bool bIsFinal = (CondPins.Num() == (++condIndex));
				UEdGraphPin* PinToTry = FEdGraphUtilities::GetNetFromPin(CondPin);
				FBPTerminal** CondTerm = Context.NetMap.Find(PinToTry);

				if (CondTerm != NULL) //
				{
					// First skip the if, if the term is false
					{
						FBlueprintCompiledStatement& SkipIfGoto = Context.AppendStatementForNode(Node);
						SkipIfGoto.Type = KCST_GotoIfNot;
						SkipIfGoto.LHS = *CondTerm;
						Context.GotoFixupRequestMap.Add(&SkipIfGoto, ElsePin);
					}

					// Now go to the If branch
					if (bIsFinal)
					{
						FBlueprintCompiledStatement& GotoThen = Context.AppendStatementForNode(Node);
						GotoThen.Type = KCST_UnconditionalGoto;
						GotoThen.LHS = *CondTerm;
						Context.GotoFixupRequestMap.Add(&GotoThen, ThenPin);
					}
				}
				else
				{
					CompilerContext.MessageLog.Error(*LOCTEXT("ResolveTermPassed_Error", "Failed to resolve term passed into @@").ToString(), CondPin);
				}
			}
		}
	}
};


void UK2Node_BothBranch::AllocateDefaultPins()
{
	//创建输入控制流的pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);

	//创建输出控制流的pin  true
	UEdGraphPin* pinTrue = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
	pinTrue->PinFriendlyName = LOCTEXT("AllocateDefaultPins_True", "true");

	// false
	UEdGraphPin* pinFalse = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Else);
	pinFalse->PinFriendlyName = LOCTEXT("AllocateDefaultPins_False", "false");

	AddUniqueConditionPin();

	Super::AllocateDefaultPins();
}

void UK2Node_BothBranch::PreloadRequiredAssets()
{
	
}

FText UK2Node_BothBranch::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("Condition Both Title", "Branch Both Condition");
}

FLinearColor UK2Node_BothBranch::GetNodeTitleColor() const
{
	return GetDefault<UGraphEditorSettings>()->ExecutionPinTypeColor;
}

FText UK2Node_BothBranch::GetTooltipText() const
{
	return LOCTEXT("Condition Both Tip", "Goes to true if All pin gets true");
}

FSlateIcon UK2Node_BothBranch::GetIconAndTint(FLinearColor& OutColor) const
{
	static FSlateIcon icon{FEditorStyle::GetStyleSetName(), "GraphEditor.Branch_16x"};
	return icon;
}

void UK2Node_BothBranch::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	//创建输入控制流的pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);

	//创建输出控制流的pin  true
	UEdGraphPin* pinTrue = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
	pinTrue->PinFriendlyName = LOCTEXT("AllocateDefaultPins_True", "true");

	// false
	UEdGraphPin* pinFalse = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Else);
	pinFalse->PinFriendlyName = LOCTEXT("AllocateDefaultPins_False", "false");

	for (int i = 0; i < OldPins.Num() - 3; ++i)
	{
		AddUniqueConditionPin();
	}

	RestoreSplitPins(OldPins);
}

FText UK2Node_BothBranch::GetMenuCategory() const
{
	return FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::FlowControl);
}

void UK2Node_BothBranch::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* NodeClass = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(NodeClass))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(NodeClass);
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(NodeClass, NodeSpawner);
	}
}

void UK2Node_BothBranch::PinConnectionListChanged(UEdGraphPin* Pin)
{
	if (IsConditionPin(Pin))
	{
		//如果取消了连接  删除pin
		if (Pin->LinkedTo.Num() == 0)
		{
			RemovePin(Pin);
		}

		//如果添加并占满了   新加一个pin
		if (!IsHaveUnlinkConditionPin())
		{
			AddUniqueConditionPin();
		}

		//更新外观
		GetGraph()->NotifyGraphChanged();
	}

	Super::PinConnectionListChanged(Pin);
}

void UK2Node_BothBranch::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

// 	//取得输入的pin
// 	TArray<UEdGraphPin*> ConditionPins;
// 	for (int32 i = 0; true; ++i)
// 	{
// 		UEdGraphPin* pin = FindPin(GetPinNameGivenIndex(i));
// 		if (!pin)
// 		{
// 			break;
// 		}
// 		ConditionPins.Add(pin);
// 	}
// 
// 	CompilerContext.SpawnIntermediateNode();
}

UEdGraphPin* UK2Node_BothBranch::AddUniqueConditionPin()
{
	UEdGraphPin* pin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, GetUniquePinName());
	pin->DefaultValue = TEXT("True");
	return pin;
}

FName UK2Node_BothBranch::GetUniquePinName()
{
	FName pinName;
	for (int32 i = 0; true; ++i)
	{
		pinName = GetPinNameGivenIndex(i);
		if (!FindPin(pinName))
		{
			break;
		}
	}
	return pinName;
}

FName UK2Node_BothBranch::GetPinNameGivenIndex(int32 Index)
{
	return *FString::Printf(TEXT("%s_%d"), *UEdGraphSchema_K2::PN_Condition.ToString(), Index);
}

bool UK2Node_BothBranch::IsConditionPin(UEdGraphPin* pin)
{
	if (pin && pin->Direction == EGPD_Input && pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Boolean)
	{
		return true;
	}
	return false;
}

bool UK2Node_BothBranch::IsHaveUnlinkConditionPin()
{
	for (const auto& pin : Pins)
	{
		if (IsConditionPin(pin) && pin->LinkedTo.Num() == 0)
		{
			return true;
		}
	}
	return false;
}

void UK2Node_BothBranch::RemovePin(UEdGraphPin* targetPin)
{
	DestroyPin(targetPin);
	Pins.Remove(targetPin);

	int index = 0;
	for (auto pin : Pins)
	{
		if (IsConditionPin(pin))
		{
			pin->PinName = GetPinNameGivenIndex(index++);
		}
	}
}

UEdGraphPin* UK2Node_BothBranch::GetThenPin()
{
	UEdGraphPin* pin = FindPin(UEdGraphSchema_K2::PN_Then);
	check(pin);
	return pin;
}

UEdGraphPin* UK2Node_BothBranch::GetElsePin()
{
	UEdGraphPin* pin = FindPin(UEdGraphSchema_K2::PN_Else);
	check(pin);
	return pin;
}

UEdGraphPin* UK2Node_BothBranch::GetConditionPinByIndex(int32 Index)
{
	return FindPin(GetPinNameGivenIndex(Index));
}

FNodeHandlingFunctor* UK2Node_BothBranch::CreateNodeHandler(FKismetCompilerContext& CompilerContext) const
{
	return new FKCHandler_BothBranch(CompilerContext, Pins.Num() - 3);
}
