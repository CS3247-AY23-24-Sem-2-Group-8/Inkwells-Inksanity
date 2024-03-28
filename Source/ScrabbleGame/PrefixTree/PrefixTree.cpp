#include "PrefixTree.h"
#include <cassert>
#include <cctype>
#include "iostream"
#include "stack"

// FPrefixTreeNode functions
FPrefixTreeNode::FPrefixTreeNode(const char Letter)
{
	assert(std::isalpha(Letter));
	this->Letter = std::tolower(Letter);
}

FPrefixTreeNode::~FPrefixTreeNode()
{
	for (const FPrefixTreeNode* Child : Children) delete Child;
}

void FPrefixTreeNode::AddChildIfNull(const char ChildLetter)
{
	// get relative position of child
	if (const int Index = LetterToIndex(ChildLetter); !Children[Index])
	{
		if (!std::isalpha(ChildLetter)) return;
		FPrefixTreeNode* Child = new FPrefixTreeNode(ChildLetter);
		Children[Index] = Child;
	}
}

FPrefixTreeNode* FPrefixTreeNode::GetChild(const char ChildLetter) const
{
	const int Index = LetterToIndex(ChildLetter);
	return Children[Index];
}

void FPrefixTreeNode::PrintLetter() const
{
	std::cout << Letter;
}

char FPrefixTreeNode::GetLetter() const
{
	return Letter;
}

void FPrefixTreeNode::DeleteChild(const char ChildLetter)
{
	if (const FPrefixTreeNode* Child = Children[LetterToIndex(ChildLetter)])
	{
		delete Child;
		Children[LetterToIndex(ChildLetter)] = nullptr;
	}
}

bool FPrefixTreeNode::IsPrefixTo(const char ChildLetter) const 
{
	return Children[LetterToIndex(ChildLetter)] != NULL;
}

// a leaf is a node that has no additional children
bool FPrefixTreeNode::IsLeafNode() const
{
	for (const FPrefixTreeNode* Child : Children)
	{
		if (Child) return false;
	}

	return true;
}

int FPrefixTreeNode::LetterToIndex(const char Letter)
{
	return std::tolower(Letter) - 'a';
}

// FPrefixTree functions
FPrefixTree::FPrefixTree()
{
	
}

void FPrefixTree::InsertWord(const std::string& Word)
{
	FPrefixTreeNode* Current = &Root;
	
	for (const char Letter : Word)
	{
		if (!std::isalpha(Letter)) continue;

		Current->AddChildIfNull(Letter);
		Current = Current->GetChild(Letter);
	}
	Current->IsWord = true;
}

// untested might be buggy
void FPrefixTree::DeleteWord(const std::string& Word)
{
	std::stack<FPrefixTreeNode*> Stack;
	FPrefixTreeNode* Current = &Root;
	
	for (const char Letter : Word)
	{
		if (Letter == '\0') continue;
		if (!Current->IsPrefixTo(Letter)) break;

		Stack.push(Current->GetChild(Letter));
		Current = Current->GetChild(Letter);
	}

	if (!Current->IsWord) return;
	Current->IsWord = false;

	// recursively delete the rest of the letters that are not words
	while (!Stack.empty())
	{
		const FPrefixTreeNode* Node = Stack.top();
		
		if (Node->IsWord || !Node->IsLeafNode()) return;
		Stack.pop();

		FPrefixTreeNode* Parent = Stack.top();
		Parent->DeleteChild(Node->GetLetter());
	}
}

bool FPrefixTree::Contains(const std::string& Word) const
{
	const FPrefixTreeNode* Current = &Root;
	
	for (const char Letter : Word)
	{
		if (Letter == '\0') continue;
		if (!Current->IsPrefixTo(Letter)) return false;
		
		Current = Current->GetChild(Letter);
	}
	return Current->IsWord;
}

bool FPrefixTree::ContainsPrefix(const std::string& Prefix) const
{
	const FPrefixTreeNode* Current = &Root;
	
	for (const char Letter : Prefix)
	{
		if (Letter == '\0') continue;
		if (!Current->IsPrefixTo(Letter)) return false;
		
		Current = Current->GetChild(Letter);
	}
	return true;
}