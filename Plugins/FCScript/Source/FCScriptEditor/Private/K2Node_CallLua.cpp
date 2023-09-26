
#include "K2Node_CallLua.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "KismetCompilerMisc.h"
#include "KismetCompiler.h"
#include "BlueprintNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "K2Node_CallFunction.h"
#include "ToolMenu.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "ScopedTransaction.h"
#include "FCFunctionLibrary.h"

#define LOCTEXT_NAMESPACE "K2Node_CallLua"


UK2Node_CallLua::UK2Node_CallLua(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UK2Node_CallLua::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

}

FText UK2Node_CallLua::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return Super::GetNodeTitle(TitleType);
}


void UK2Node_CallLua::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);
	SynchronizeArgumentPinType(Pin);
}


FLinearColor UK2Node_CallLua::GetNodeTitleColor() const
{
	return FLinearColor::White;
}

FName UK2Node_CallLua::GetUniquePinName()
{
	FName NewPinName;
	int32 i = 0;
	while (true)
	{
		NewPinName = GetPinNameGivenIndex(i++);
		if (!FindPin(NewPinName))
		{
			break;
		}
	}

	return NewPinName;
}

void UK2Node_CallLua::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{	
	Super::ExpandNode(CompilerContext, SourceGraph);

	return;	
}

void UK2Node_CallLua::AddInputPin()
{
	Modify();
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, GetUniquePinName());
	UpdateVariadicNum();
}

bool UK2Node_CallLua::CanRemovePin(const UEdGraphPin* Pin) const
{
	return IsParamInputPin(Pin);
}

bool UK2Node_CallLua::CanAddPin() const
{
	return  VariadicNum < 16;
}


void UK2Node_CallLua::PostReconstructNode()
{
	Super::PostReconstructNode();

	if (!IsTemplate())
	{
		// Make sure we're not dealing with a menu node
		UEdGraph* OuterGraph = GetGraph();
		if (OuterGraph && OuterGraph->Schema)
		{
			for (UEdGraphPin* CurrentPin : Pins)
			{
				if(IsParamInputPin(CurrentPin))
				{
					SynchronizeArgumentPinType(CurrentPin);
				}
			}
		}
	}
}


void UK2Node_CallLua::SynchronizeArgumentPinType(UEdGraphPin* Pin)
{
	if(!IsParamInputPin(Pin))
	{
		return;
	}

	const UEdGraphSchema_K2* K2Schema = Cast<const UEdGraphSchema_K2>(GetSchema());

	bool bPinTypeChanged = false;
	if (Pin->LinkedTo.Num() == 0)
	{
		static const FEdGraphPinType WildcardPinType = FEdGraphPinType(UEdGraphSchema_K2::PC_Wildcard, NAME_None, nullptr, EPinContainerType::None, false, FEdGraphTerminalType());

		// Ensure wildcard
		if (Pin->PinType != WildcardPinType)
		{
			Pin->PinType = WildcardPinType;
			bPinTypeChanged = true;
		}
	}
	else
	{
		UEdGraphPin* ArgumentSourcePin = Pin->LinkedTo[0];

		// Take the type of the connected pin
		if (Pin->PinType != ArgumentSourcePin->PinType)
		{
			Pin->PinType = ArgumentSourcePin->PinType;
			bPinTypeChanged = true;
		}
	}

	if (bPinTypeChanged)
	{
		// Let the graph know to refresh
		GetGraph()->NotifyGraphChanged();

		UBlueprint* Blueprint = GetBlueprint();
		if (!Blueprint->bBeingCompiled)
		{
			FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
			Blueprint->BroadcastChanged();
		}
	}
}


void UK2Node_CallLua::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	Super::GetNodeContextMenuActions(Menu, Context);

	
	if (!Context->bIsDebugging)
	{
		static FName CommutativeAssociativeBinaryOperatorNodeName = FName("UK2Node_CallLuaNode");
		FText CommutativeAssociativeBinaryOperatorStr = LOCTEXT("UK2Node_CallLuaNode", "Operator Node");
		if (Context->Pin != NULL)
		{
			if(CanRemovePin(Context->Pin))
			{
				FToolMenuSection& Section = Menu->AddSection(CommutativeAssociativeBinaryOperatorNodeName, CommutativeAssociativeBinaryOperatorStr);
				Section.AddMenuEntry(
					"RemovePin",
					LOCTEXT("RemovePin", "Remove pin"),
					LOCTEXT("RemovePinTooltip", "Remove this input pin"),
					FSlateIcon(),
					FUIAction(
						FExecuteAction::CreateUObject(const_cast<UK2Node_CallLua*>(this), &UK2Node_CallLua::RemovePinFromExecutionNode, const_cast<UEdGraphPin*>(Context->Pin))
					)
				);
			}
		}
	}
}

void UK2Node_CallLua::RemovePinFromExecutionNode(UEdGraphPin* TargetPin) 
{
	FScopedTransaction Transaction( LOCTEXT("RemovePinTx", "RemovePin") );
	Modify();

	
	UK2Node_CallLua* OwningSeq = Cast<UK2Node_CallLua>( TargetPin->GetOwningNode() );
	if (OwningSeq)
	{
		OwningSeq->Pins.Remove(TargetPin);
		TargetPin->MarkAsGarbage(); 
		//TargetPin->MarkPendingKill();
		// Renumber the pins so the numbering is compact
		int32 ThenIndex = 0;
		for (int32 i = 0; i < OwningSeq->Pins.Num(); ++i)
		{
			UEdGraphPin* PotentialPin = OwningSeq->Pins[i];
			if (IsParamInputPin(PotentialPin))
			{
				PotentialPin->PinName = GetPinNameGivenIndex(ThenIndex);
				++ThenIndex;
			}
		}
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
	}
	UpdateVariadicNum();
}


FName UK2Node_CallLua::GetPinNameGivenIndex(int32 Index) const
{
	return *FString::Printf(TEXT("%s%d"), *ParamPinPrefixed(), Index);
}

bool UK2Node_CallLua::IsParamInputPin(const UEdGraphPin* TestPin) const
{
	if (UEdGraphSchema_K2::IsExecPin(*TestPin) || (TestPin->Direction != EGPD_Input) || TestPin->bHidden)
	{
		return false;
	}

	return TestPin->GetName().StartsWith(ParamPinPrefixed());
}

int UK2Node_CallLua::GetParamIndex(UEdGraphPin* TargetPin) const
{
	FString Name = TargetPin->GetName();
	if(!Name.StartsWith(ParamPinPrefixed()))
	{
		return -1;
	}
	FString Num = Name.Replace(*ParamPinPrefixed(), TEXT(""));
	return  FCString::Atoi(*Num);
}

FString UK2Node_CallLua::ParamPinPrefixed() const
{
	return FString(TEXT("Param_"));
}


void UK2Node_CallLua::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	Super::ReallocatePinsDuringReconstruction(OldPins);
	for (int i = 0; i < VariadicNum; ++i)
	{
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, GetUniquePinName());
	}
}

#define GET_UFUNCTION_CHECKED(CLASS, NAME) CLASS::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(CLASS, NAME))

void UK2Node_CallLua::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	auto CustomizeInputNodeLambda = [](UEdGraphNode* NewNode, bool bIsTemplateNode, TWeakObjectPtr<UFunction> Func)
	{
		if(Func.IsValid())
		{
			UK2Node_CallLua* InputNode = CastChecked<UK2Node_CallLua>(NewNode);
			InputNode->SetFromFunction(Func.Get());
			//InputNode->AllocateDefaultPins();
		}
	};
	UClass* ActionKey = GetClass();
	UFunction* ListenMessageFunc = nullptr;

	// config here <======
	UFunction* Functions [] = {
		GET_UFUNCTION_CHECKED(UFCFunctionLibrary, CallLuaTableFunction),
        GET_UFUNCTION_CHECKED(UFCFunctionLibrary, CallLuaFunction),
	};
	
	
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		for (auto Function : Functions)
		{
			UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
			check(NodeSpawner != nullptr);
			TWeakObjectPtr<UFunction> Func(Function);
			NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(CustomizeInputNodeLambda, Func);
	
			if (Function->HasMetaData(TEXT("Category")))
			{
				const FString & Category = Function->GetMetaData(TEXT("Category"));
				NodeSpawner->DefaultMenuSignature.Category = FText::FromString(Category);
			}
			else
			{
				NodeSpawner->DefaultMenuSignature.Category = GetMenuCategory();
			}

			if (Function->HasMetaData(TEXT("ToolTip")))
			{
				const FString & Tooltip = Function->GetMetaData(TEXT("ToolTip"));
				NodeSpawner->DefaultMenuSignature.Tooltip = FText::FromString(Tooltip);
			}
			else
			{
				NodeSpawner->DefaultMenuSignature.Tooltip = FText::FromString(Function->GetName());
			}
			
			NodeSpawner->DefaultMenuSignature.MenuName = FText::FromString(Function->GetName());
			
			ActionRegistrar.AddBlueprintAction(Function, NodeSpawner);	
		}
	}
}

FText UK2Node_CallLua::GetMenuCategory() const
{
	return FText::FromString(TEXT("Varadic"));
}


void UK2Node_CallLua::UpdateVariadicNum()
{
	int ParamPinNum = 0;
	for(auto Pin : Pins)
	{
		if(IsParamInputPin(Pin))
		{
			ParamPinNum ++;
		}
	}
	VariadicNum = ParamPinNum;
}

#undef LOCTEXT_NAMESPACE
