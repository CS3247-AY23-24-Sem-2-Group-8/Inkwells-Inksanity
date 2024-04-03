#pragma once
#include <random>

#include "MCTS.h"
#include "../ScrabbleDictionary.h"

struct FBoardAction final : Action
{
	char SelectedLetter;
	int PointValue;

	FBoardAction(const char SelectedLetter, const int PointValue)
	{
		this->SelectedLetter = SelectedLetter;
		this->PointValue = PointValue;
	}

	virtual bool operator ==(const Action& Other) const override
	{
		const FBoardAction& OtherAction = static_cast<const FBoardAction&>(Other);
		
		return SelectedLetter == OtherAction.SelectedLetter && PointValue == OtherAction.PointValue;
	}
};

struct FBoardState final : State
{
	std::string CurrentWord;
	std::vector<std::pair<char, int>*> Letters;
	int PointValue;

	FBoardState(const std::string& CurrentWord, const std::vector<std::pair<char, int>*>& Letters, const int PointValue = 0)
	{
		this->CurrentWord = CurrentWord;
		this->Letters = Letters;
		this->PointValue = PointValue;
	}

	FBoardState* Result(const FBoardAction* Action) const
	{
		if (Letters.empty()) return nullptr;

		const std::pair<char, int>* Pair = nullptr;
		for (const auto CurrentPair: Letters)
		{
			if (Action != nullptr && CurrentPair->first != Action->SelectedLetter) continue;
			
			if (Pair == nullptr || CurrentPair->second > Pair->second)
			{
				Pair = CurrentPair;
			}
		}
		if (Pair == nullptr)
		{
			return nullptr;
		}
		
		std::vector<std::pair<char, int>*> NewLetters;
		std::string NewWord = CurrentWord;

		for (auto CurrentPair: Letters)
		{
			if (CurrentPair == Pair) continue;
			NewLetters.push_back(CurrentPair);
		}

		FBoardState* NewState = new FBoardState(CurrentWord + Pair->first, NewLetters, PointValue + Pair->second);
		return NewState;
	}

	std::deque<FBoardAction*>* Actions()
	{
		std::deque<FBoardAction*>* Actions = new std::deque<FBoardAction*>();

		for (const auto CurrentPair : Letters) {
			FBoardAction* Action = new FBoardAction(CurrentPair->first, CurrentPair->second);
			Actions->push_back(Action);
		}

		return Actions;
	}

	bool IsTerminal() const
	{
		return this->Letters.empty();
	}
};

class FScrabbleGameBoard final : public Problem<FBoardAction, FBoardState>
{
public:
	explicit FScrabbleGameBoard(FBoardState* InitialState, FBoardState* GoalState = nullptr) : Problem(InitialState, GoalState) {}

	std::deque<FBoardAction*>* Actions(FBoardState* State) override;

	FBoardState* Result(FBoardState* State, FBoardAction* Action) override;

	double Value(FBoardState* State) override;

	double Cost(FBoardState* FromState, FBoardAction* Action, FBoardState* ToState) override;

	bool IsGoal(const FBoardState* State);
};


class FEnemyAI final : public MCTSAgent<FBoardAction, FBoardState>
{
public:
	FEnemyAI(FScrabbleGameBoard* Problem, const double ThresholdToStop, UScrabbleDictionary* Dictionary,
		const double MaxTimeForSearch): MCTSAgent(Problem)
	{
		std::random_device Rd;
		this->RNG = std::mt19937(Rd());
		this->Problem = Problem;
		this->ThresholdToStop = ThresholdToStop;
		this->Dictionary = Dictionary;
		this->MaxTimeForSearch = MaxTimeForSearch;
		this->CurrentState = Problem->InitialState;
	}

	FBoardAction* Search(FBoardState* State) override;

	double Ucb(MCTSNode<FBoardAction, FBoardState>* Node, double Confidence = 1.4) override;
	
	double Utility(const FBoardState* State) const;

	MCTSNode<FBoardAction, FBoardState>* Select(MCTSNode<FBoardAction, FBoardState>* Node) override;

	MCTSNode<FBoardAction, FBoardState>* Expand(MCTSNode<FBoardAction, FBoardState>* Node) override;

	double Simulate(MCTSNode<FBoardAction, FBoardState>* ChildNode) override;

	void Backprop(MCTSNode<FBoardAction, FBoardState>* Node, double Utility) override;

	bool IsTerminal(const FBoardState* Node) const;

private:
	FScrabbleGameBoard* Problem;
	double ThresholdToStop;
	UScrabbleDictionary* Dictionary;
	double MaxTimeForSearch;
	std::mt19937 RNG;
};

