#pragma once
#include <cctype>

#define NUM_LETTERS 26

// don't ask why there is an F prefixed to the
// type; its simply the coding standard for unreal

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
	FPrefixTreeNode* Children[NUM_LETTERS] = {nullptr};
	
public:
	bool IsWord = false;
	
	explicit FPrefixTreeNode(const char Letter);

	~FPrefixTreeNode();

	void AddChildIfNull(const char ChildLetter);

	FPrefixTreeNode* GetChild(const char ChildLetter) const;

	void PrintLetter() const;

	char GetLetter() const;

	void DeleteChild(const char ChildLetter);

	bool IsPrefixTo(const char ChildLetter) const;

	bool IsLeafNode() const;

	static int LetterToIndex(const char Letter);
};

struct FPrefixTree
{
	// represents the root of the tree; initialised with A as placeholder
	FPrefixTreeNode Root = FPrefixTreeNode('a');
	
	FPrefixTree();
	
	void InsertWord(const std::string& Word);

	void DeleteWord(const std::string& Word);

	bool Contains(const std::string& Word) const;
};
