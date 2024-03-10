#pragma once
#include <cassert>
#include <cctype>
#include "iostream"

#define NUM_LETTERS 26

// don't ask why there is an F prefixed to the
// type; its simply the coding standard for unreal

// Represents a node in the prefix tree; Only allows the 26 alphabets.
typedef struct FPrefixTreeNode FPrefixTreeNode;

/**
 * Standard node for a prefix tree for english words,
 * using only the 26 alphabets. Each node contains a letter
 * and will denote if the current node is a word or not
 * to make checking simpler.
 */
struct FPrefixTreeNode
{
private:
	char Letter;
	// array of children, one for each letter
	FPrefixTreeNode* Children[NUM_LETTERS];
	bool IsWord = false;

public: 
	explicit FPrefixTreeNode(const char Letter)
	{
		assert(std::isalpha(Letter));
		this->Letter = std::tolower(Letter);

		for (int i = 0; i < NUM_LETTERS; i++) Children[i] = nullptr;
	}

	~FPrefixTreeNode()
	{
		for (const FPrefixTreeNode* Child : Children) delete Child;
	}

	void AddChildIfNull(const char Letter)
	{
		// get relative position of child
		if (const int Index = LetterToIndex(Letter); !Children[Index])
		{
			FPrefixTreeNode Child = FPrefixTreeNode(Letter);
			Children[Index] = &Child;
		}
	}

	FPrefixTreeNode* GetChild(const char Letter) const
	{
		const int Index = LetterToIndex(Letter);
		return Children[Index];
	}

	void PrintLetter() const
	{
		std::cout << Letter;
	}

	char GetLetter() const
	{
		return Letter;
	}

	void SetAsWord()
	{
		IsWord = true;
	}

	static int LetterToIndex(const char Letter)
	{
		return static_cast<int>(Letter) - 'a';
	}
};

struct FPrefixTree
{
	// represents the root of the tree, similar to how each
	// node has an array of children
	FPrefixTreeNode Root = FPrefixTreeNode('a');

public:
	void InsertWord(char* Word)
	{
		FPrefixTreeNode* Current = &Root;
		
		for (const char Letter : Word)
		{
			if (Letter == '\0') continue;
			Current->AddChildIfNull(Letter);
			Current = Current->GetChild(Letter);
		}
		Current->SetAsWord();
	}
};
