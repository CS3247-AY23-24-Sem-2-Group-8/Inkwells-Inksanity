// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "ScrabbleDictionary.h"
#include "MCTS/EnemyAI.h"
#include "ScrabbleEnemyAI.generated.h"

USTRUCT(BlueprintType)
struct FBoardTile
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tile")
	FString Letter = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tile")
	int Point = 1;

	bool operator==(const FBoardTile& Other) const
	{
		return Letter == Other.Letter && Point == Other.Point;
	}
};

/**
 * 
 */
UCLASS(Blueprintable)
class SCRABBLEGAME_API UScrabbleEnemyAI: public UObject
{
	GENERATED_BODY()
public:
	UScrabbleEnemyAI();
	~UScrabbleEnemyAI();
	
	UFUNCTION(BlueprintCallable, Category = "Scrabble AI")
	void InitialiseAI(double StopThreshold, UScrabbleDictionary* DictionaryReference, double SearchTimeLimit);
	
	UFUNCTION(BlueprintCallable, Category = "Scrabble AI")
	void SetBoard(TArray<FBoardTile> Tiles);
	
	UFUNCTION(BlueprintCallable, Category = "Scrabble AI")
	void GetNextAction();

	UFUNCTION(BlueprintNativeEvent, Category = "Scrabble AI")
	void OnFinish(const FString& SelectedCharacter, const int& PointValue);
private:
	UScrabbleDictionary* Dictionary;
	double ThresholdToStop;
	double MaxTimeToSearch;
	EnemyAI* EnemyAI;
};
