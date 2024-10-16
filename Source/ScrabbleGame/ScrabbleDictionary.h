// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrefixTree/PrefixTree.h"
#include "ScrabbleDictionary.generated.h"

#define MAX_WORD_LENGTH 16

#ifdef _WIN32
	#define NEWLINE "\r\n"
#elif defined macintosh // OS 9
	#define NEWLINE "\r"
#else
	#define NEWLINE "\n" // Mac OS X uses \n
#endif

/**
 * 
 */
UCLASS(BlueprintType)
class SCRABBLEGAME_API UScrabbleDictionary : public UObject
{
	GENERATED_BODY()
	
private:
	FPrefixTree Trie;
	
public:
	// should receive a text file with the words in sorted order preferably
	UScrabbleDictionary();
	
	UFUNCTION(BlueprintCallable, Category = "ScrabbleDictionary")
	bool IsValidWord(const FString Word) const;
	bool IsValidPrefix(const FString& Prefix) const;

	UFUNCTION(BlueprintCallable, Category = "ScrabbleDictionary")
	static UScrabbleDictionary* ConstructScrabbleDictionary(const FString FileName, const int32 MinWordLength);
};
