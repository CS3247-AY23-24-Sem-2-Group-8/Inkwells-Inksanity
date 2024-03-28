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

void UScrabbleEnemyAI::InitialiseAI(double StopThreshold, UScrabbleDictionary* DictionaryReference, double SearchTimeLimit)
{
	ThresholdToStop = StopThreshold;
	Dictionary = DictionaryReference;
	MaxTimeToSearch = SearchTimeLimit;
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

void UScrabbleEnemyAI::GetNextAction()
{
	FString SelectedCharacter = "";
	int PointValue = -1;
	if (Dictionary == nullptr || EnemyAI == nullptr) return OnFinish(SelectedCharacter, PointValue);

	AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [this] () {
		/* Work on the TaskGraph */
		if (this == nullptr) return;
		FString SelectedCharacter = "";
		int PointValue = -1;
		const BoardAction* Action = EnemyAI->getNextAction();
		if (Action == nullptr) return OnFinish(SelectedCharacter, PointValue);
		
		char String[2] = "\0";
		String[0] = Action->selectedLetter;
		SelectedCharacter = String;
		PointValue = Action->pointValue;
		OnFinish(SelectedCharacter, PointValue);
	});
}

void UScrabbleEnemyAI::OnFinish_Implementation(const FString& SelectedCharacter, const int& PointValue)
{
}

UScrabbleEnemyAI::~UScrabbleEnemyAI()
{
	delete EnemyAI;
}
