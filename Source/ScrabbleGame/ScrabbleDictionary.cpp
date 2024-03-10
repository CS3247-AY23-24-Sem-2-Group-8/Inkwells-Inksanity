// Fill out your copyright notice in the Description page of Project Settings.

#include "ScrabbleDictionary.h"
#include "Kismet/KismetStringLibrary.h"
#include <iostream>

// constructor but not sure if it needs to be used
UScrabbleDictionary::UScrabbleDictionary()
{
	
}

UScrabbleDictionary* UScrabbleDictionary::ConstructScrabbleDictionary(const FString FileName, const int32 MinWordLength)
{
	UScrabbleDictionary* Dict = NewObject<UScrabbleDictionary>();
	FString ParsedText;
	const FString Path = FPaths::ProjectContentDir() + FileName;

	// fails if the specified path name does not exist
	if (const TCHAR* PathName = *(Path); !FFileHelper::LoadFileToString(ParsedText, PathName))
	{
		std::cout << "bruh" << std::endl;
		return Dict;
	}

	// create an array of words from the text
	TArray<FString> ParsedWords = UKismetStringLibrary::ParseIntoArray(ParsedText, "\n");

	for (FString& Word : ParsedWords)
	{
		if (Word.Len() < MinWordLength || Word.Len() > MAX_WORD_LENGTH) continue;
		
		Dict->Trie.InsertWord(TCHAR_TO_ANSI(*Word));
	}

	return Dict;
}

bool UScrabbleDictionary::IsValidWord(const FString Word) const
{
	return Trie.Contains(TCHAR_TO_ANSI(*Word));
}

