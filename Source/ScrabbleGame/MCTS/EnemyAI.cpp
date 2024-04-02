#include "EnemyAI.h"

#include <chrono>

std::deque<BoardAction*>* ScrabbleGameBoard::Actions(BoardState* State)
{
	return State->Actions();
}

BoardState* ScrabbleGameBoard::Result(BoardState* State, BoardAction* Action)
{
	return State->Result(Action);
}

double ScrabbleGameBoard::Value(BoardState* State)
{
	return State->PointValue;
}

double ScrabbleGameBoard::Cost(BoardState* FromState, BoardAction* Action, BoardState* ToState)
{
	if (FromState->Result(Action) == nullptr || FromState->Result(Action) != ToState)
	{
		return std::numeric_limits<double>::infinity();
	}
	return 1;
}

bool ScrabbleGameBoard::IsGoal(const BoardState* State)
{
	return State->Letters.empty();
}

BoardAction* EnemyAI::Search(BoardState* State)
{
	if (IsTerminal(State)) return nullptr;
	if (Problem->IsGoal(State)) return nullptr;

	MCTSNode<BoardAction, BoardState>* Root = new MCTSNode<BoardAction, BoardState>(nullptr, nullptr, State, 0, 0);
	Expand(Root);

	const auto Start = std::chrono::steady_clock::now();
	auto Current = Start;
	while (std::chrono::duration_cast<std::chrono::seconds>(Current - Start).count() < MaxTimeForSearch)
	{
		MCTSNode<BoardAction, BoardState>* Leaf = this->Select(Root);
		MCTSNode<BoardAction, BoardState>* Child = this->Expand(Leaf);
		const double Value = this->Simulate(Child);
		Backprop(Child, Value);
		Current = std::chrono::steady_clock::now();
	}

	double MaxUtility = -std::numeric_limits<double>::infinity();
	const MCTSNode<BoardAction, BoardState>* BestChild = nullptr;
	for (const auto Child : *(Root->Children))
	{
		if (Child->Utility <= MaxUtility) continue;

		MaxUtility = Child->Utility;
		BestChild = Child;
	}

	if (BestChild == nullptr) return nullptr;

	return BestChild->Action;
}

double EnemyAI::Ucb(MCTSNode<BoardAction, BoardState>* Node, const double Confidence)
{
	double Utility = Node->Utility;
	if (Node->NumVisits == 0) Utility = std::numeric_limits<double>::infinity();
	const FString Word = Node->State->CurrentWord.c_str();
	Utility += Confidence * std::sqrt(std::log(Node->Parent->NumVisits) / Node->NumVisits);
	
	if (!Dictionary->IsValidPrefix(Word)) return -1;
	return Utility;
}

double EnemyAI::Utility(const BoardState* State) const
{
	double Utility = State->PointValue;
	const FString Word = State->CurrentWord.c_str();
	
	if (!Dictionary->IsValidPrefix(Word)) return -10000000;
	if (!Dictionary->IsValidWord(Word)) Utility /= 3;
	return Utility;
}

MCTSNode<BoardAction, BoardState>* EnemyAI::Select(MCTSNode<BoardAction, BoardState>* Node)
{
	if (Node->Children == nullptr || Node->Children->empty()) return Node;

	double Value = -1;
	MCTSNode<BoardAction, BoardState>* BestChild = Node;
		
	for (const auto Child : *(Node->Children))
	{
		const double CurrValue = this->Ucb(Child);
		if (Value > CurrValue) continue;
		Value = CurrValue;
		BestChild = Child;
	}

	return BestChild;
}

MCTSNode<BoardAction, BoardState>* EnemyAI::Expand(MCTSNode<BoardAction, BoardState>* Node)
{
	if (this->Problem->IsTerminal(Node->State) || Node->UntriedActions->size() == 0) return Node;

	while (Node->UntriedActions->size() > 0)
	{
		BoardAction* NextAction = Node->UntriedActions->front();
		Node->UntriedActions->pop_front();
		BoardState* NextState = Node->State->Result(NextAction);
		MCTSNode<BoardAction, BoardState>* NewNode = new MCTSNode(Node, NextAction, NextState, 0, 1);
		Node->Children->push_back(NewNode);
	}

	return this->Select(Node);
}

double EnemyAI::Simulate(MCTSNode<BoardAction, BoardState>* ChildNode)
{
	MCTSNode<BoardAction, BoardState>* Node = ChildNode;
	while (!IsTerminal(Node->State))
	{
		const int Length = Node->State->Actions()->size();
		if (Length < 1) break;
		
		std::uniform_int_distribution Distribution(0, Length - 1);

		// get random action valid prefix
		const int RandomChildIndex = Distribution(RNG);
		BoardAction* Action = Node->State->Actions()->at(RandomChildIndex);
		BoardState* State = Node->State->Result(Action);
		Node = new MCTSNode(Node, Action, State, 0, 1);
	}
	const double Value = this->Utility(Node->State);
	return Value;
}

void EnemyAI::Backprop(MCTSNode<BoardAction, BoardState>* Node, const double Utility)
{

	if (Utility > 0) Node->Utility += Utility;
	Node->NumVisits += 1;
	if (Node->Parent != nullptr) Backprop(Node->Parent, Utility);
}

bool EnemyAI::IsTerminal(const BoardState* State) const
{
	if (Dictionary == nullptr) return false;
	return Dictionary->IsValidWord(State->CurrentWord.c_str()) || State->IsTerminal();
}
