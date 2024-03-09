// Fill out your copyright notice in the Description page of Project Settings.


#include "ScrabbleDictionary.h"
#include "Kismet/KismetStringLibrary.h"

UScrabbleDictionary::UScrabbleDictionary()
{
	
}

UScrabbleDictionary* UScrabbleDictionary::ConstructScrabbleDictionary(FString FileName, int32 MinWordLength)
{
	UScrabbleDictionary* Dict = NewObject<UScrabbleDictionary>();
	FString ParsedText;
	FString Path = FPaths::ProjectContentDir() + FileName;
	const TCHAR* PathName = *(Path);

	// fails if the specified path name does not exist
	if (!FFileHelper::LoadFileToString(ParsedText, PathName))
	{
		
	}

	// create an array of words from the text
	TArray<FString> ParsedWords = UKismetStringLibrary::ParseIntoArray(ParsedText, "\n");

	for (FString& Word : ParsedWords)
	{
		if (Word.Len() < MinWordLength)
			continue;
	}

	return Dict;
}

bool UScrabbleDictionary::IsValidWord(FString Word)
{
	return true;
}

