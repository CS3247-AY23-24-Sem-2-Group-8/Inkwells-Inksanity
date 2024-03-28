#include "EnemyAI.h"

#include <chrono>

std::deque<BoardAction*>* ScrabbleGameBoard::actions(BoardState* State)
{
	return State->Actions();
}

BoardState* ScrabbleGameBoard::result(BoardState* state, BoardAction* Action)
{
	return state->result(Action);
}

double ScrabbleGameBoard::value(BoardState* State)
{
	return State->pointValue;
}

double ScrabbleGameBoard::cost(BoardState* FromState, BoardAction* Action, BoardState* ToState)
{
	if (FromState->result(Action) == nullptr || FromState->result(Action) != ToState)
	{
		return std::numeric_limits<double>::infinity();
	}
	return 1;
}

bool ScrabbleGameBoard::isGoal(const BoardState* State)
{
	return State->letters.empty();
}

BoardAction* EnemyAI::search(BoardState* state)
{
	if (isTerminal(state)) return nullptr;
	if (problem->isGoal(state)) return nullptr;

	MCTSNode<BoardAction, BoardState>* Root = new MCTSNode<BoardAction, BoardState>(nullptr, nullptr, state, 0, 0);
	expand(Root);

	const auto Start = std::chrono::steady_clock::now();
	auto Current = Start;
	while (std::chrono::duration_cast<std::chrono::seconds>(Current - Start).count() < maxTimeForSearch)
	{
		MCTSNode<BoardAction, BoardState>* Leaf = this->select(Root);
		MCTSNode<BoardAction, BoardState>* Child = this->expand(Leaf);
		double Value = this->simulate(Child);
		backprop(Child, Value);
		Current = std::chrono::steady_clock::now();
	}

	double maxUtility = -std::numeric_limits<double>::infinity();
	MCTSNode<BoardAction, BoardState>* bestChild = nullptr;
	for (auto child : *(Root->children))
	{
		if (child->utility <= maxUtility) continue;

		maxUtility = child->utility;
		bestChild = child;
	}

	if (bestChild == nullptr) return nullptr;

	return bestChild->action;
}

double EnemyAI::ucb(MCTSNode<BoardAction, BoardState>* Node, const double Confidence)
{
	double Utility = Node->utility;
	if (Node->numVisits == 0) Utility = std::numeric_limits<double>::infinity();
	const FString Word = Node->state->currentWord.c_str();
	Utility += Confidence * std::sqrt(std::log(Node->parent->numVisits) / Node->numVisits);
	
	if (!dictionary->IsValidPrefix(Word)) return -1;
	return Utility;
}

double EnemyAI::Utility(const BoardState* State) const
{
	double Utility = State->pointValue;
	const FString Word = State->currentWord.c_str();
	
	if (!dictionary->IsValidPrefix(Word)) return -10000000;
	if (!dictionary->IsValidWord(Word)) Utility /= 3;
	return Utility;
}

MCTSNode<BoardAction, BoardState>* EnemyAI::select(MCTSNode<BoardAction, BoardState>* node)
{
	if (node->children == nullptr || node->children->empty()) return node;

	double value = -1;
	MCTSNode<BoardAction, BoardState>* bestChild = node;
		
	for (auto child : *(node->children))
	{	
		double currValue = this->ucb(child);
		if (value > currValue) continue;
		value = currValue;
		bestChild = child;
	}

	return bestChild;
}

MCTSNode<BoardAction, BoardState>* EnemyAI::expand(MCTSNode<BoardAction, BoardState>* node)
{
	if (this->problem->isTerminal(node->state) || node->untriedActions->size() == 0) return node;

	while (node->untriedActions->size() > 0)
	{
		BoardAction* nextAction = node->untriedActions->front();
		node->untriedActions->pop_front();
		BoardState* nextState = node->state->result(nextAction);
		MCTSNode<BoardAction, BoardState>* newNode = new MCTSNode<BoardAction, BoardState>(node, nextAction, nextState, 0, 1);
		node->children->push_back(newNode);
	}

	return this->select(node);
}

double EnemyAI::simulate(MCTSNode<BoardAction, BoardState>* childNode)
{
	MCTSNode<BoardAction, BoardState>* node = childNode;
	while (!isTerminal(node->state))
	{
		int length = node->state->Actions()->size();
		if (length < 1) break;
		
		std::uniform_int_distribution<int> distribution(0, length - 1);

		// get random action valid prefix
		int randomChildIndex = distribution(rng);
		BoardAction* action = node->state->Actions()->at(randomChildIndex);
		BoardState* state = node->state->result(action);
		node = new MCTSNode<BoardAction, BoardState>(node, action, state, 0, 1);
	}
	double value = this->Utility(node->state);
	return value;
}

void EnemyAI::backprop(MCTSNode<BoardAction, BoardState>* node, double utility)
{

	if (utility > 0) node->utility += utility;
	node->numVisits += 1;
	if (node->parent != nullptr) backprop(node->parent, utility);
}

bool EnemyAI::isTerminal(BoardState* state)
{
	if (dictionary == nullptr) return false;
	return dictionary->IsValidWord(state->currentWord.c_str()) || state->isTerminal();
}
