// Fill out your copyright notice in the Description page of Project Settings.

#include "ScrabbleEnemyAI.h"
#include "MCTS/EnemyAI.h"

UScrabbleEnemyAI::UScrabbleEnemyAI()
{
	ThresholdToStop = 0;
	MaxTimeToSearch = 0;
	Dictionary = nullptr;
	EnemyAI = nullptr;
}

UScrabbleEnemyAI* UScrabbleEnemyAI::InitialiseAI(double StopThreshold, UScrabbleDictionary* DictionaryReference, double SearchTimeLimit)
{
	UScrabbleEnemyAI* Ai = NewObject<UScrabbleEnemyAI>();
	Ai->ThresholdToStop = StopThreshold;
	Ai->Dictionary = DictionaryReference;
	Ai->MaxTimeToSearch = SearchTimeLimit;
	return Ai;
}

void UScrabbleEnemyAI::SetBoard(TArray<FBoardTile> Tiles)
{
	std::vector<std::pair<char, int>*> tiles;
	for (auto Tile: Tiles)
	{
		if (Tile.Letter.Len() < 1) return;
		std::pair<char, int>* ConvertedTile = new std::pair<char, int>(Tile.Letter[0], Tile.Point);
		tiles.push_back(ConvertedTile);
	}
	BoardState* State = new BoardState("", tiles);
	ScrabbleGameBoard* Board = new ScrabbleGameBoard(State);
	EnemyAI = new class EnemyAI(Board, ThresholdToStop, Dictionary, MaxTimeToSearch);
}

void UScrabbleEnemyAI::GetNextAction(FString& SelectedCharacter, int& PointValue)
{
	const BoardAction* Action = EnemyAI->getNextAction();
	if (Action == nullptr)
	{
		SelectedCharacter = "";
		PointValue = -1;
		return;
	}
	char String[2] = "\0";
	String[0] = Action->selectedLetter;
	SelectedCharacter = String;
	PointValue = Action->pointValue;
}

UScrabbleEnemyAI::~UScrabbleEnemyAI()
{
	delete EnemyAI;
}
