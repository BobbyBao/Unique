
#ifndef __SCRIPTPARSER_H_
#define __SCRIPTPARSER_H_

#include "ScriptLexer.h"

namespace Unique
{
	/** These enums hold the types of the concrete parsed nodes */
	enum ConcreteNodeType
	{
		CNT_VARIABLE,
		CNT_VARIABLE_ASSIGN,
		CNT_WORD,
		CNT_IMPORT,
		CNT_QUOTE,
		CNT_LBRACE,
		CNT_RBRACE,
		CNT_COLON
	};

	/** The ConcreteNode is the struct that holds an un-conditioned sub-tree of parsed input */
	struct ConcreteNode;
	typedef std::shared_ptr<ConcreteNode> ConcreteNodePtr;
	typedef List<ConcreteNodePtr> ConcreteNodeList;
	typedef std::shared_ptr<ConcreteNodeList> ConcreteNodeListPtr;
	struct ConcreteNode
	{
		String token, file;
		unsigned int line;
		ConcreteNodeType type;
		ConcreteNodeList children;
		ConcreteNode *parent;
	};

    class UNIQUE_API ScriptParser
    {
    public:
        static ConcreteNodeListPtr parse(const ScriptTokenListPtr &tokens);
        static ConcreteNodeListPtr parseChunk(const ScriptTokenListPtr &tokens);
    private:
        static ScriptToken *getToken(ScriptTokenList::iterator i, ScriptTokenList::iterator end, int offset);
        static ScriptTokenList::iterator skipNewlines(ScriptTokenList::iterator i, ScriptTokenList::iterator end);
    };
    
    /** @} */
    /** @} */
}


#endif
