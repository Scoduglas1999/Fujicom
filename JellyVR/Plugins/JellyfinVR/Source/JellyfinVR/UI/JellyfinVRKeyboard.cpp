// Copyright JellyVR Project. All Rights Reserved.

#include "JellyfinVRKeyboard.h"
#include "JellyfinVRModule.h"

void UJellyfinVRKeyboard::NativeConstruct()
{
	Super::NativeConstruct();
	InitializeLayouts();
}

void UJellyfinVRKeyboard::InitializeLayouts()
{
	// QWERTY Layout (lowercase)
	QWERTYLayout.Empty();
	QWERTYLayout.Add({ TEXT("q"), TEXT("w"), TEXT("e"), TEXT("r"), TEXT("t"), TEXT("y"), TEXT("u"), TEXT("i"), TEXT("o"), TEXT("p") });
	QWERTYLayout.Add({ TEXT("a"), TEXT("s"), TEXT("d"), TEXT("f"), TEXT("g"), TEXT("h"), TEXT("j"), TEXT("k"), TEXT("l") });
	QWERTYLayout.Add({ TEXT("z"), TEXT("x"), TEXT("c"), TEXT("v"), TEXT("b"), TEXT("n"), TEXT("m") });

	// QWERTY Layout (uppercase)
	QWERTYLayoutShift.Empty();
	QWERTYLayoutShift.Add({ TEXT("Q"), TEXT("W"), TEXT("E"), TEXT("R"), TEXT("T"), TEXT("Y"), TEXT("U"), TEXT("I"), TEXT("O"), TEXT("P") });
	QWERTYLayoutShift.Add({ TEXT("A"), TEXT("S"), TEXT("D"), TEXT("F"), TEXT("G"), TEXT("H"), TEXT("J"), TEXT("K"), TEXT("L") });
	QWERTYLayoutShift.Add({ TEXT("Z"), TEXT("X"), TEXT("C"), TEXT("V"), TEXT("B"), TEXT("N"), TEXT("M") });

	// Numeric Layout
	NumericLayout.Empty();
	NumericLayout.Add({ TEXT("1"), TEXT("2"), TEXT("3"), TEXT("4"), TEXT("5"), TEXT("6"), TEXT("7"), TEXT("8"), TEXT("9"), TEXT("0") });
	NumericLayout.Add({ TEXT("-"), TEXT("/"), TEXT(":"), TEXT(";"), TEXT("("), TEXT(")"), TEXT("$"), TEXT("&"), TEXT("@") });
	NumericLayout.Add({ TEXT("."), TEXT(","), TEXT("?"), TEXT("!"), TEXT("'"), TEXT("\"") });

	// Symbols Layout
	SymbolsLayout.Empty();
	SymbolsLayout.Add({ TEXT("["), TEXT("]"), TEXT("{"), TEXT("}"), TEXT("#"), TEXT("%"), TEXT("^"), TEXT("*"), TEXT("+"), TEXT("=") });
	SymbolsLayout.Add({ TEXT("_"), TEXT("\\"), TEXT("|"), TEXT("~"), TEXT("<"), TEXT(">"), TEXT("€"), TEXT("£"), TEXT("¥") });
	SymbolsLayout.Add({ TEXT("•"), TEXT("°"), TEXT("©"), TEXT("®"), TEXT("™") });
}

void UJellyfinVRKeyboard::ShowKeyboard(const FString& InitialText, const FString& PlaceholderText)
{
	CurrentText = InitialText;
	Placeholder = PlaceholderText;
	bIsVisible = true;
	bShiftActive = false;
	CurrentLayout = EKeyboardLayout::QWERTY;

	SetVisibility(ESlateVisibility::Visible);
	OnVisibilityUpdated(true);
	UpdateTextDisplay();
	OnLayoutUpdated(CurrentLayout, bShiftActive);

	UE_LOG(LogJellyfinVR, Log, TEXT("VR Keyboard shown"));
}

void UJellyfinVRKeyboard::HideKeyboard()
{
	bIsVisible = false;
	SetVisibility(ESlateVisibility::Collapsed);
	OnVisibilityUpdated(false);

	UE_LOG(LogJellyfinVR, Log, TEXT("VR Keyboard hidden"));
}

void UJellyfinVRKeyboard::SetText(const FString& Text)
{
	CurrentText = Text.Left(MaxTextLength);
	UpdateTextDisplay();
	OnTextChanged.Broadcast(CurrentText);
}

void UJellyfinVRKeyboard::SetPasswordMode(bool bIsPassword)
{
	bPasswordMode = bIsPassword;
	UpdateTextDisplay();
}

void UJellyfinVRKeyboard::SetMaxLength(int32 MaxLength)
{
	MaxTextLength = FMath::Max(1, MaxLength);
	if (CurrentText.Len() > MaxTextLength)
	{
		CurrentText = CurrentText.Left(MaxTextLength);
		UpdateTextDisplay();
		OnTextChanged.Broadcast(CurrentText);
	}
}

void UJellyfinVRKeyboard::SetLayout(EKeyboardLayout Layout)
{
	if (CurrentLayout != Layout)
	{
		CurrentLayout = Layout;
		OnLayoutUpdated(CurrentLayout, bShiftActive);
	}
}

void UJellyfinVRKeyboard::OnKeyPressed(const FString& Character)
{
	if (Character.IsEmpty() || CurrentText.Len() >= MaxTextLength)
	{
		return;
	}

	CurrentText += Character;

	// Auto-disable shift after typing a letter
	if (bShiftActive && CurrentLayout == EKeyboardLayout::QWERTY)
	{
		bShiftActive = false;
		OnLayoutUpdated(CurrentLayout, bShiftActive);
	}

	UpdateTextDisplay();
	OnTextChanged.Broadcast(CurrentText);
}

void UJellyfinVRKeyboard::OnBackspace()
{
	if (CurrentText.Len() > 0)
	{
		CurrentText = CurrentText.LeftChop(1);
		UpdateTextDisplay();
		OnTextChanged.Broadcast(CurrentText);
	}
}

void UJellyfinVRKeyboard::OnClear()
{
	CurrentText.Empty();
	UpdateTextDisplay();
	OnTextChanged.Broadcast(CurrentText);
}

void UJellyfinVRKeyboard::OnSpace()
{
	if (CurrentText.Len() < MaxTextLength)
	{
		CurrentText += TEXT(" ");
		UpdateTextDisplay();
		OnTextChanged.Broadcast(CurrentText);
	}
}

void UJellyfinVRKeyboard::OnShift()
{
	bShiftActive = !bShiftActive;
	OnLayoutUpdated(CurrentLayout, bShiftActive);
}

void UJellyfinVRKeyboard::OnSubmit()
{
	OnSubmitted.Broadcast(CurrentText);
	HideKeyboard();
}

void UJellyfinVRKeyboard::OnCancel()
{
	OnCancelled.Broadcast();
	HideKeyboard();
}

void UJellyfinVRKeyboard::UpdateTextDisplay()
{
	FString DisplayText;

	if (CurrentText.IsEmpty())
	{
		DisplayText = Placeholder;
	}
	else if (bPasswordMode)
	{
		// Show dots for password
		DisplayText = FString::ChrN(CurrentText.Len(), TEXT('•'));
	}
	else
	{
		DisplayText = CurrentText;
	}

	OnTextUpdated(DisplayText, bPasswordMode && !CurrentText.IsEmpty());
}

TArray<FString> UJellyfinVRKeyboard::GetKeysForRow(int32 RowIndex) const
{
	const TArray<TArray<FString>>* LayoutPtr = nullptr;

	switch (CurrentLayout)
	{
	case EKeyboardLayout::QWERTY:
		LayoutPtr = bShiftActive ? &QWERTYLayoutShift : &QWERTYLayout;
		break;
	case EKeyboardLayout::Numeric:
		LayoutPtr = &NumericLayout;
		break;
	case EKeyboardLayout::Symbols:
		LayoutPtr = &SymbolsLayout;
		break;
	}

	if (LayoutPtr && RowIndex >= 0 && RowIndex < LayoutPtr->Num())
	{
		return (*LayoutPtr)[RowIndex];
	}

	return TArray<FString>();
}

int32 UJellyfinVRKeyboard::GetRowCount() const
{
	switch (CurrentLayout)
	{
	case EKeyboardLayout::QWERTY:
		return QWERTYLayout.Num();
	case EKeyboardLayout::Numeric:
		return NumericLayout.Num();
	case EKeyboardLayout::Symbols:
		return SymbolsLayout.Num();
	}
	return 0;
}
