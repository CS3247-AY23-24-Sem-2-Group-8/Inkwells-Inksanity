#pragma once

struct Action
{
	virtual ~Action() = default;
	virtual bool operator ==(const Action& other) const = 0;
};

struct State
{
	virtual ~State() = default;
	template <class T, class U, class = typename std::enable_if<std::is_base_of_v<Action, T>>::type,
		 class = typename std::enable_if<std::is_base_of_v<State, U>>::type>
	U* result(T* action);

	template <class T, class U, class = typename std::enable_if<std::is_base_of_v<Action, T>>::type,
		class = typename std::enable_if<std::is_base_of_v<State, U>>::type>
	std::queue<T*>* untriedActions() const;

	virtual bool isTerminal() const = 0;
};

template <class T, class U, class = typename std::enable_if<std::is_base_of_v<Action, T>>::type,
	class = typename std::enable_if<std::is_base_of_v<State, U>>::type>
class Problem
{
public:
	U* initialState;
	U* goalState;
	
	Problem(U* initialState, U* goalState)
	{
		this->initialState = initialState;
		this->goalState = goalState;
	};

	virtual std::deque<T*>* actions(U* state) = 0;
	virtual U* result(U* state, T* action) = 0;
	virtual double value(U* state) = 0;
	virtual double cost(U* fromState, T* action, U* toState) = 0;

	bool isGoal(U* state) {
		return state == goalState;
	};

	double pathCost(double currentCost, U* state1, T* action, U* state2)
	{
		return currentCost + 1;
	};

	bool isTerminal(U* state)
	{
		return isGoal(state) || this->actions(state)->empty();
	}
};

template <class T, class U, class = typename std::enable_if<std::is_base_of_v<Action, T>>::type,
	class = typename std::enable_if<std::is_base_of_v<State, U>>::type>
struct MCTSNode
{
	MCTSNode* parent;
	T* action;
	U* state;
	double utility;
	int numVisits;
	std::vector<MCTSNode*>* children;
	std::deque<T*>* untriedActions;

	MCTSNode(MCTSNode* parent = nullptr, T* action = nullptr, U* state = nullptr, double utility = 0, int numVisits = 0)
	{
		this->parent = parent;
		this->action = action;
		this->state = state;
		this->utility = utility;
		this->numVisits = numVisits;
		this->children = new std::vector<MCTSNode*>();
		this->untriedActions = state->Actions();
	};

	~MCTSNode()
	{
		delete state;
		delete action;
		for (auto* child : children) {
			delete child;
		}
		delete children;
		while (!untriedActions->empty()) {
			delete untriedActions->front();
			untriedActions->pop();
		}
		delete untriedActions();
	}
};

template <class T, class U, class = typename std::enable_if<std::is_base_of_v<Action, T>>::type,
	class = typename std::enable_if<std::is_base_of_v<State, U>>::type>
class MCTSAgent
{
public:
	MCTSAgent(Problem<T, U>* problem)
	{
		this->problem = problem;
		this->currentState = NULL;
		this->currentAction = NULL;
	};

	T* getNextAction()
	{
		T* action = this->search(currentState);
		this->currentState = currentState->result(action);
		this->currentAction = action;
		return action;
	};

	virtual T* search(U* state) = 0;
	virtual double ucb(MCTSNode<T, U>* node, double cost) = 0;
	virtual MCTSNode<T, U>* select(MCTSNode<T, U>* node) = 0;
	virtual MCTSNode<T, U>* expand(MCTSNode<T, U>* node) = 0;
	virtual double simulate(MCTSNode<T, U>* childNode) = 0;
	virtual void backprop(MCTSNode<T, U>* node, double utility) = 0;

protected:
	Problem<T, U>* problem;
	U* currentState;
	T* currentAction;
};