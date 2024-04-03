#pragma once
#include <deque>

struct Action
{
	virtual ~Action() = default;
	virtual bool operator ==(const Action& Other) const = 0;
};

struct State
{
	virtual ~State() = default;
	template <class T, class U, class = std::enable_if_t<std::is_base_of_v<Action, T>>,
		 class = std::enable_if_t<std::is_base_of_v<State, U>>>
	U* Result(T* action);

	template <class T, class U, class = std::enable_if_t<std::is_base_of_v<Action, T>>,
		class = std::enable_if_t<std::is_base_of_v<State, U>>>
	std::queue<T*>* UntriedActions() const;

	virtual bool IsTerminal() const = 0;
};

template <class T, class U, class = std::enable_if_t<std::is_base_of_v<Action, T>>,
	class = std::enable_if_t<std::is_base_of_v<State, U>>>
class Problem
{
public:
	U* InitialState;
	U* GoalState;
	
	Problem(U* InitialState, U* GoalState)
	{
		this->InitialState = InitialState;
		this->GoalState = GoalState;
	};

	virtual ~Problem() = default;

	virtual std::deque<T*>* Actions(U* State) = 0;
	virtual U* Result(U* State, T* Action) = 0;
	virtual double Value(U* State) = 0;
	virtual double Cost(U* FromState, T* Action, U* ToState) = 0;

	bool IsGoal(U* State) {
		return State == GoalState;
	};

	double PathCost(double CurrentCost, U* State1, T* Action, U* State2)
	{
		return CurrentCost + 1;
	};

	bool IsTerminal(U* State)
	{
		return IsGoal(State) || this->Actions(State)->empty();
	}
};

template <class T, class U, class = std::enable_if_t<std::is_base_of_v<Action, T>>,
	class = std::enable_if_t<std::is_base_of_v<State, U>>>
struct MCTSNode
{
	MCTSNode* Parent;
	T* Action;
	U* State;
	double Utility;
	int NumVisits;
	std::vector<MCTSNode*>* Children;
	std::deque<T*>* UntriedActions;

	MCTSNode(MCTSNode* Parent = nullptr, T* Action = nullptr, U* State = nullptr, double Utility = 0, int NumVisits = 0)
	{
		this->Parent = Parent;
		this->Action = Action;
		this->State = State;
		this->Utility = Utility;
		this->NumVisits = NumVisits;
		this->Children = new std::vector<MCTSNode*>();
		this->UntriedActions = State->Actions();
	};

	~MCTSNode()
	{
		delete State;
		delete Action;
		for (auto* Child : Children) {
			delete Child;
		}
		delete Children;
		while (!UntriedActions->empty()) {
			delete UntriedActions->front();
			UntriedActions->pop();
		}
		delete UntriedActions();
	}
};

template <class T, class U, class = std::enable_if_t<std::is_base_of_v<Action, T>>,
	class = std::enable_if_t<std::is_base_of_v<State, U>>>
class MCTSAgent
{
public:
	MCTSAgent(Problem<T, U>* Problem)
	{
		this->Problem = Problem;
		this->CurrentState = NULL;
		this->CurrentAction = NULL;
	};

	virtual ~MCTSAgent() = default;

	T* GetNextAction()
	{
		T* Action = this->Search(CurrentState);
		this->CurrentState = CurrentState->Result(Action);
		this->CurrentAction = Action;
		return Action;
	};

	virtual T* Search(U* State) = 0;
	virtual double Ucb(MCTSNode<T, U>* Node, double Cost) = 0;
	virtual MCTSNode<T, U>* Select(MCTSNode<T, U>* Node) = 0;
	virtual MCTSNode<T, U>* Expand(MCTSNode<T, U>* Node) = 0;
	virtual double Simulate(MCTSNode<T, U>* ChildNode) = 0;
	virtual void Backprop(MCTSNode<T, U>* Node, double Utility) = 0;

protected:
	Problem<T, U>* Problem;
	U* CurrentState;
	T* CurrentAction;
};