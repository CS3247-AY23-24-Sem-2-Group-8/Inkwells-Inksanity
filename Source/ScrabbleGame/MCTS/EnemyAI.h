#pragma once
#include <random>

#include "MCTS.h"
#include "../ScrabbleDictionary.h"

struct BoardAction : Action
{
	char SelectedLetter;
	int PointValue;

	BoardAction(const char SelectedLetter, const int PointValue)
	{
		this->SelectedLetter = SelectedLetter;
		this->PointValue = PointValue;
	}

	virtual bool operator ==(const Action& Other) const override
	{
		const BoardAction& OtherAction = static_cast<const BoardAction&>(Other);
		
		return SelectedLetter == OtherAction.SelectedLetter && PointValue == OtherAction.PointValue;
	}
};

struct BoardState : State
{
	std::string CurrentWord;
	std::vector<std::pair<char, int>*> Letters;
	int PointValue;

	BoardState(const std::string& CurrentWord, const std::vector<std::pair<char, int>*>& Letters, const int PointValue = 0)
	{
		this->CurrentWord = CurrentWord;
		this->Letters = Letters;
		this->PointValue = PointValue;
	}

	BoardState* Result(const BoardAction* Action) const
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

		BoardState* NewState = new BoardState(CurrentWord + Pair->first, NewLetters, PointValue + Pair->second);
		return NewState;
	}

	std::deque<BoardAction*>* Actions()
	{
		std::deque<BoardAction*>* Actions = new std::deque<BoardAction*>();

		for (const auto CurrentPair : Letters) {
			BoardAction* Action = new BoardAction(CurrentPair->first, CurrentPair->second);
			Actions->push_back(Action);
		}

		return Actions;
	}

	bool IsTerminal() const
	{
		return this->Letters.empty();
	}
};

class ScrabbleGameBoard : public Problem<BoardAction, BoardState>
{
public:
	ScrabbleGameBoard(BoardState* InitialState, BoardState* GoalState = nullptr) : Problem(InitialState, GoalState) {}

	std::deque<BoardAction*>* Actions(BoardState* State) override;

	BoardState* Result(BoardState* State, BoardAction* Action) override;

	double Value(BoardState* State) override;

	double Cost(BoardState* FromState, BoardAction* Action, BoardState* ToState) override;

	bool IsGoal(const BoardState* State);
};


class EnemyAI: public MCTSAgent<BoardAction, BoardState>
{
public:
	EnemyAI(ScrabbleGameBoard* Problem, const double ThresholdToStop, UScrabbleDictionary* Dictionary,
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

	BoardAction* Search(BoardState* State) override;

	double Ucb(MCTSNode<BoardAction, BoardState>* Node, double Confidence = 1.4) override;
	
	double Utility(const BoardState* State) const;

	MCTSNode<BoardAction, BoardState>* Select(MCTSNode<BoardAction, BoardState>* Node) override;

	MCTSNode<BoardAction, BoardState>* Expand(MCTSNode<BoardAction, BoardState>* Node) override;

	double Simulate(MCTSNode<BoardAction, BoardState>* ChildNode) override;

	void Backprop(MCTSNode<BoardAction, BoardState>* Node, double Utility) override;

	bool IsTerminal(const BoardState* Node) const;

private:
	ScrabbleGameBoard* Problem;
	double ThresholdToStop;
	UScrabbleDictionary* Dictionary;
	double MaxTimeForSearch;
	std::mt19937 RNG;
};

