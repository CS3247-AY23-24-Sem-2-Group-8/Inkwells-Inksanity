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

void UScrabbleEnemyAI::InitialiseAI(const double StopThreshold, UScrabbleDictionary* DictionaryReference, const double SearchTimeLimit)
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
	const FString SelectedCharacter = "";
	constexpr int PointValue = -1;
	if (Dictionary == nullptr || EnemyAI == nullptr) return OnFinish(SelectedCharacter, PointValue);

	AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [this] () {
		/* Work on the TaskGraph */
		if (this == nullptr) return;
		FString Character = "";
		int Value = -1;
		const BoardAction* Action = EnemyAI->GetNextAction();
		if (Action == nullptr) return OnFinish(Character, Value);
		
		char String[2] = "\0";
		String[0] = Action->SelectedLetter;
		Character = String;
		Value = Action->PointValue;
		OnFinish(Character, Value);
	});
}

void UScrabbleEnemyAI::OnFinish_Implementation(const FString& SelectedCharacter, const int& PointValue)
{
}

UScrabbleEnemyAI::~UScrabbleEnemyAI()
{
	delete EnemyAI;
}
