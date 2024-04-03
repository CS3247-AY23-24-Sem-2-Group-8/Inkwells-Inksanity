#include "EnemyAI.h"

#include <chrono>

std::deque<FBoardAction*>* FScrabbleGameBoard::Actions(FBoardState* State)
{
	return State->Actions();
}

FBoardState* FScrabbleGameBoard::Result(FBoardState* State, FBoardAction* Action)
{
	return State->Result(Action);
}

double FScrabbleGameBoard::Value(FBoardState* State)
{
	return State->PointValue;
}

double FScrabbleGameBoard::Cost(FBoardState* FromState, FBoardAction* Action, FBoardState* ToState)
{
	if (FromState->Result(Action) == nullptr || FromState->Result(Action) != ToState)
	{
		return std::numeric_limits<double>::infinity();
	}
	return 1;
}

bool FScrabbleGameBoard::IsGoal(const FBoardState* State)
{
	return State->Letters.empty();
}

FBoardAction* FEnemyAI::Search(FBoardState* State)
{
	if (IsTerminal(State)) return nullptr;
	if (Problem->IsGoal(State)) return nullptr;

	MCTSNode<FBoardAction, FBoardState>* Root = new MCTSNode<FBoardAction, FBoardState>(nullptr, nullptr, State, 0, 0);
	Expand(Root);

	const auto Start = std::chrono::steady_clock::now();
	auto Current = Start;
	while (std::chrono::duration_cast<std::chrono::milliseconds>(Current - Start).count() < MaxTimeForSearch * 1000)
	{
		MCTSNode<FBoardAction, FBoardState>* Leaf = this->Select(Root);
		MCTSNode<FBoardAction, FBoardState>* Child = this->Expand(Leaf);
		const double Value = this->Simulate(Child);
		Backprop(Child, Value);
		Current = std::chrono::steady_clock::now();
	}

	double MaxUtility = -std::numeric_limits<double>::infinity();
	const MCTSNode<FBoardAction, FBoardState>* BestChild = nullptr;
	for (const auto Child : *(Root->Children))
	{
		if (Child->Utility <= MaxUtility) continue;

		MaxUtility = Child->Utility;
		BestChild = Child;
	}

	if (BestChild == nullptr) return nullptr;

	return BestChild->Action;
}

double FEnemyAI::Ucb(MCTSNode<FBoardAction, FBoardState>* Node, const double Confidence)
{
	double Utility = Node->Utility;
	if (Node->NumVisits == 0) Utility = std::numeric_limits<double>::infinity();
	const FString Word = Node->State->CurrentWord.c_str();
	Utility += Confidence * std::sqrt(std::log(Node->Parent->NumVisits) / Node->NumVisits);
	
	if (!Dictionary->IsValidPrefix(Word)) return -1;
	return Utility;
}

double FEnemyAI::Utility(const FBoardState* State) const
{
	double Utility = State->PointValue;
	const FString Word = State->CurrentWord.c_str();
	
	if (!Dictionary->IsValidPrefix(Word)) return -10000000;
	if (!Dictionary->IsValidWord(Word)) Utility /= 3;
	return Utility;
}

MCTSNode<FBoardAction, FBoardState>* FEnemyAI::Select(MCTSNode<FBoardAction, FBoardState>* Node)
{
	if (Node->Children == nullptr || Node->Children->empty()) return Node;

	double Value = -1;
	MCTSNode<FBoardAction, FBoardState>* BestChild = Node;
		
	for (const auto Child : *(Node->Children))
	{
		const double CurrValue = this->Ucb(Child);
		if (Value > CurrValue) continue;
		Value = CurrValue;
		BestChild = Child;
	}

	return BestChild;
}

MCTSNode<FBoardAction, FBoardState>* FEnemyAI::Expand(MCTSNode<FBoardAction, FBoardState>* Node)
{
	if (this->Problem->IsTerminal(Node->State) || Node->UntriedActions->size() == 0) return Node;

	while (Node->UntriedActions->size() > 0)
	{
		FBoardAction* NextAction = Node->UntriedActions->front();
		Node->UntriedActions->pop_front();
		FBoardState* NextState = Node->State->Result(NextAction);
		MCTSNode<FBoardAction, FBoardState>* NewNode = new MCTSNode(Node, NextAction, NextState, 0, 1);
		Node->Children->push_back(NewNode);
	}

	return this->Select(Node);
}

double FEnemyAI::Simulate(MCTSNode<FBoardAction, FBoardState>* ChildNode)
{
	MCTSNode<FBoardAction, FBoardState>* Node = ChildNode;
	while (!IsTerminal(Node->State))
	{
		const int Length = Node->State->Actions()->size();
		if (Length < 1) break;
		
		std::uniform_int_distribution Distribution(0, Length - 1);

		// get random action valid prefix
		const int RandomChildIndex = Distribution(RNG);
		FBoardAction* Action = Node->State->Actions()->at(RandomChildIndex);
		FBoardState* State = Node->State->Result(Action);
		Node = new MCTSNode(Node, Action, State, 0, 1);
	}
	const double Value = this->Utility(Node->State);
	return Value;
}

void FEnemyAI::Backprop(MCTSNode<FBoardAction, FBoardState>* Node, const double Utility)
{

	if (Utility > 0) Node->Utility += Utility;
	Node->NumVisits += 1;
	if (Node->Parent != nullptr) Backprop(Node->Parent, Utility);
}

bool FEnemyAI::IsTerminal(const FBoardState* State) const
{
	if (Dictionary == nullptr) return false;
	return Dictionary->IsValidWord(State->CurrentWord.c_str()) || State->IsTerminal();
}
