#pragma once
#include <random>

#include "MCTS.h"
#include "../ScrabbleDictionary.h"

struct BoardAction : Action
{
	char selectedLetter;
	int pointValue;

	BoardAction(char selectedLetter, int pointValue)
	{
		this->selectedLetter = selectedLetter;
		this->pointValue = pointValue;
	}

	bool operator ==(const Action& other) const override
	{
		const BoardAction& otherAction = (const BoardAction&) other;
		
		return selectedLetter == otherAction.selectedLetter && pointValue == otherAction.pointValue;
	}
};

struct BoardState : State
{
	std::string currentWord;
	std::vector<std::pair<char, int>*> letters;
	int pointValue;

	BoardState(std::string currentWord, const std::vector<std::pair<char, int>*>& letters, int pointValue = 0)
	{
		this->currentWord = currentWord;
		this->letters = letters;
		this->pointValue = pointValue;
	}

	BoardState* result(const BoardAction* action)
	{
		if (letters.empty()) return nullptr; 

		std::pair<char, int>* Pair = nullptr;
		for (auto CurrentPair: letters)
		{
			if (action != nullptr && CurrentPair->first != action->selectedLetter) continue;
			
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
		std::string NewWord = currentWord;

		for (auto CurrentPair: letters)
		{
			if (CurrentPair == Pair) continue;
			NewLetters.push_back(CurrentPair);
		}

		BoardState* newState = new BoardState(currentWord + Pair->first, NewLetters, pointValue + Pair->second);
		return newState;
	}

	std::deque<BoardAction*>* Actions()
	{
		std::deque<BoardAction*>* Actions = new std::deque<BoardAction*>();

		for (const auto CurrentPair : letters) {
			BoardAction* Action = new BoardAction(CurrentPair->first, CurrentPair->second);
			Actions->push_back(Action);
		}

		return Actions;
	}

	bool isTerminal() const
	{
		return this->letters.empty();
	}
};

class ScrabbleGameBoard : public Problem<BoardAction, BoardState>
{
public:
	ScrabbleGameBoard(BoardState* initialState, BoardState* goalState = nullptr) : Problem(initialState, goalState) {}

	std::deque<BoardAction*>* actions(BoardState* State) override;

	BoardState* result(BoardState* state, BoardAction* Action) override;

	double value(BoardState* State) override;

	double cost(BoardState* FromState, BoardAction* Action, BoardState* ToState) override;

	bool isGoal(const BoardState* State);
};


class EnemyAI: public MCTSAgent<BoardAction, BoardState>
{
public:
	EnemyAI(ScrabbleGameBoard* problem, double thresholdToStop, UScrabbleDictionary* dictionary, double maxTimeForSearch) : MCTSAgent(problem)
	{
		std::random_device rd;
		this->rng = std::mt19937(rd());
		this->problem = problem;
		this->thresholdToStop = thresholdToStop;
		this->dictionary = dictionary;
		this->maxTimeForSearch = maxTimeForSearch;
		this->currentState = problem->initialState;
	}

	BoardAction* search(BoardState* state) override;

	double ucb(MCTSNode<BoardAction, BoardState>* Node, double cost = 1.4) override;
	
	double Utility(const BoardState* State) const;

	MCTSNode<BoardAction, BoardState>* select(MCTSNode<BoardAction, BoardState>* node) override;

	MCTSNode<BoardAction, BoardState>* expand(MCTSNode<BoardAction, BoardState>* node) override;

	double simulate(MCTSNode<BoardAction, BoardState>* childNode) override;

	void backprop(MCTSNode<BoardAction, BoardState>* node, double utility) override;

	bool isTerminal(BoardState* node);

private:
	ScrabbleGameBoard* problem;
	double thresholdToStop;
	UScrabbleDictionary* dictionary;
	double maxTimeForSearch;
	std::mt19937 rng;
};

