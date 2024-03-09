// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PrefixTree.h"
#include "ScrabbleDictionary.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class SCRABBLEGAME_API UScrabbleDictionary : public UObject
{
	GENERATED_BODY()
	
private:
	PrefixTree Trie;
	
public:
	// should receive a text file with the words in sorted order preferably
	UScrabbleDictionary();
	
	UFUNCTION(BlueprintCallable, Category = "ScrabbleDictionary")
	bool IsValidWord(FString Word);

	UFUNCTION(BlueprintCallable, Category = "ScrabbleDictionary")
	static UScrabbleDictionary* ConstructScrabbleDictionary(FString FileName, int32 MinWordLength);
};
