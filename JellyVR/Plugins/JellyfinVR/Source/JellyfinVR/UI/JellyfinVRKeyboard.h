// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JellyfinVRKeyboard.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeyboardTextChanged, const FString&, NewText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeyboardSubmitted, const FString&, FinalText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnKeyboardCancelled);

UENUM(BlueprintType)
enum class EKeyboardLayout : uint8
{
	QWERTY,
	Numeric,
	Symbols
};

/**
 * VR-optimized keyboard widget for text input
 * Designed for controller/hand tracking interaction
 */
UCLASS(BlueprintType, Blueprintable)
class JELLYFINVR_API UJellyfinVRKeyboard : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/**
	 * Show the keyboard with optional initial text
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Keyboard")
	void ShowKeyboard(const FString& InitialText = TEXT(""), const FString& PlaceholderText = TEXT("Enter text..."));

	/**
	 * Hide the keyboard
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Keyboard")
	void HideKeyboard();

	/**
	 * Get current input text
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Keyboard")
	FString GetText() const { return CurrentText; }

	/**
	 * Set text programmatically
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Keyboard")
	void SetText(const FString& Text);

	/**
	 * Set whether input should be masked (password mode)
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Keyboard")
	void SetPasswordMode(bool bIsPassword);

	/**
	 * Set maximum input length
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Keyboard")
	void SetMaxLength(int32 MaxLength);

	/**
	 * Switch keyboard layout
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Keyboard")
	void SetLayout(EKeyboardLayout Layout);

	/**
	 * Get current layout
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Keyboard")
	EKeyboardLayout GetLayout() const { return CurrentLayout; }

	// ============ Key Input ============

	/**
	 * Called when a character key is pressed
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Keyboard")
	void OnKeyPressed(const FString& Character);

	/**
	 * Delete last character
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Keyboard")
	void OnBackspace();

	/**
	 * Clear all text
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Keyboard")
	void OnClear();

	/**
	 * Add a space
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Keyboard")
	void OnSpace();

	/**
	 * Toggle shift/caps
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Keyboard")
	void OnShift();

	/**
	 * Submit the current text
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Keyboard")
	void OnSubmit();

	/**
	 * Cancel input
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Keyboard")
	void OnCancel();

	/**
	 * Check if shift is active
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Keyboard")
	bool IsShiftActive() const { return bShiftActive; }

	/**
	 * Check if keyboard is visible
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Keyboard")
	bool IsKeyboardVisible() const { return bIsVisible; }

	// ============ Events ============

	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|Keyboard")
	FOnKeyboardTextChanged OnTextChanged;

	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|Keyboard")
	FOnKeyboardSubmitted OnSubmitted;

	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|Keyboard")
	FOnKeyboardCancelled OnCancelled;

	// ============ Key Layout Data ============

	/**
	 * Get keys for specified row and layout
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Keyboard")
	TArray<FString> GetKeysForRow(int32 RowIndex) const;

	/**
	 * Get number of rows in current layout
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Keyboard")
	int32 GetRowCount() const;

protected:
	/**
	 * Called when text changes - override in Blueprint to update display
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "JellyfinVR|Keyboard")
	void OnTextUpdated(const FString& DisplayText, bool bIsPasswordMode);

	/**
	 * Called when layout changes - override in Blueprint to update keys
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "JellyfinVR|Keyboard")
	void OnLayoutUpdated(EKeyboardLayout NewLayout, bool bIsShiftActive);

	/**
	 * Called when keyboard visibility changes
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "JellyfinVR|Keyboard")
	void OnVisibilityUpdated(bool bVisible);

	void UpdateTextDisplay();
	void InitializeLayouts();

private:
	UPROPERTY()
	FString CurrentText;

	UPROPERTY()
	FString Placeholder;

	EKeyboardLayout CurrentLayout = EKeyboardLayout::QWERTY;
	bool bShiftActive = false;
	bool bPasswordMode = false;
	bool bIsVisible = false;
	int32 MaxTextLength = 256;

	// Key layouts
	TArray<TArray<FString>> QWERTYLayout;
	TArray<TArray<FString>> QWERTYLayoutShift;
	TArray<TArray<FString>> NumericLayout;
	TArray<TArray<FString>> SymbolsLayout;
};
