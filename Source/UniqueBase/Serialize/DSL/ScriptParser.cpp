#include "Precompiled.h"
#include "ScriptParser.h"
#include "ScriptCompiler.h"

namespace Unique
{
    ConcreteNodeListPtr ScriptParser::parse(const ScriptTokenListPtr &tokens)
    {
        // MEMCATEGORY_GENERAL because SharedPtr can only free using that category
        ConcreteNodeListPtr nodes(new ConcreteNodeList());

        enum{READY, OBJECT};
        uint state = READY;

        ConcreteNode *parent = 0;
        ConcreteNodePtr node;
        ScriptToken *token = 0;
        ScriptTokenList::iterator i = tokens->begin(), end = tokens->end();
        while(i != end)
        {
            token = (*i).get();

            switch(state)
            {
            case READY:
                if(token->type == TID_WORD)
                {
                    if(token->lexeme == "import")
                    {
                        node = ConcreteNodePtr(new ConcreteNode());
                        node->token = token->lexeme;
                        node->file = token->file;
                        node->line = token->line;
                        node->type = CNT_IMPORT;

                        // The next token is the target
                        ++i;
                        if(i == end || ((*i)->type != TID_WORD && (*i)->type != TID_QUOTE))
							UNIQUE_LOGERRORF("expected import target at line %d", node->line);

                        ConcreteNodePtr temp(new ConcreteNode());
                        temp->parent = node.get();
                        temp->file = (*i)->file;
                        temp->line = (*i)->line;
                        temp->type = (*i)->type == TID_WORD ? CNT_WORD : CNT_QUOTE;
                        if(temp->type == CNT_QUOTE)
                            temp->token = (*i)->lexeme.Substring(1, token->lexeme.Length() - 2);
                        else
                            temp->token = (*i)->lexeme;
                        node->children.push_back(temp);

                        // The second-next token is the source
                        ++i;
                        ++i;
                        if(i == end || ((*i)->type != TID_WORD && (*i)->type != TID_QUOTE))
                            UNIQUE_LOGERRORF("expected import source at line %d", node->line);
                        temp = ConcreteNodePtr(new ConcreteNode());
                        temp->parent = node.get();
                        temp->file = (*i)->file;
                        temp->line = (*i)->line;
                        temp->type = (*i)->type == TID_WORD ? CNT_WORD : CNT_QUOTE;
                        if(temp->type == CNT_QUOTE)
                            temp->token = (*i)->lexeme.Substring(1, (*i)->lexeme.Length() - 2);
                        else
                            temp->token = (*i)->lexeme;
                        node->children.push_back(temp);

                        // Consume all the newlines
                        i = skipNewlines(i, end);

                        // Insert the node
                        if(parent)
                        {
                            node->parent = parent;
                            parent->children.push_back(node);
                        }
                        else
                        {
                            node->parent = 0;
                            nodes->push_back(node);
                        }
                        node = ConcreteNodePtr();
                    }
                    else if(token->lexeme == "set")
                    {
                        node = ConcreteNodePtr(new ConcreteNode());
                        node->token = token->lexeme;
                        node->file = token->file;
                        node->line = token->line;
                        node->type = CNT_VARIABLE_ASSIGN;

                        // The next token is the variable
                        ++i;
                        if(i == end || (*i)->type != TID_VARIABLE)
                            UNIQUE_LOGERROR(String("expected variable name at line ") + String(node->line));
                        ConcreteNodePtr temp(new ConcreteNode());
                        temp->parent = node.get();
                        temp->file = (*i)->file;
                        temp->line = (*i)->line;
                        temp->type = CNT_VARIABLE;
                        temp->token = (*i)->lexeme;
                        node->children.push_back(temp);

                        // The next token is the assignment
                        ++i;
                        if(i == end || ((*i)->type != TID_WORD && (*i)->type != TID_QUOTE))
							UNIQUE_LOGERROR(String("expected variable value at line ") + String(node->line));
                        temp = ConcreteNodePtr(new ConcreteNode());
                        temp->parent = node.get();
                        temp->file = (*i)->file;
                        temp->line = (*i)->line;
                        temp->type = (*i)->type == TID_WORD ? CNT_WORD : CNT_QUOTE;
                        if(temp->type == CNT_QUOTE)
                            temp->token = (*i)->lexeme.Substring(1, (*i)->lexeme.Length() - 2);
                        else
                            temp->token = (*i)->lexeme;
                        node->children.push_back(temp);

                        // Consume all the newlines
                        i = skipNewlines(i, end);

                        // Insert the node
                        if(parent)
                        {
                            node->parent = parent;
                            parent->children.push_back(node);
                        }
                        else
                        {
                            node->parent = 0;
                            nodes->push_back(node);
                        }
                        node = ConcreteNodePtr();
                    }
                    else
                    {
                        node = ConcreteNodePtr(new ConcreteNode());
                        node->file = token->file;
                        node->line = token->line;
                        node->type = token->type == TID_WORD ? CNT_WORD : CNT_QUOTE;
                        if(node->type == CNT_QUOTE)
                            node->token = token->lexeme.Substring(1, token->lexeme.Length() - 2);
                        else
                            node->token = token->lexeme;

                        // Insert the node
                        if(parent)
                        {
                            node->parent = parent;
                            parent->children.push_back(node);
                        }
                        else
                        {
                            node->parent = 0;
                            nodes->push_back(node);
                        }

                        // Set the parent
                        parent = node.get();

                        // Switch states
                        state = OBJECT;

                        node = ConcreteNodePtr();
                    }
                }
                else if(token->type == TID_RBRACKET)
                {
                    // Go up one level if we can
                    if(parent)
                        parent = parent->parent;

                    node = ConcreteNodePtr(new ConcreteNode());
                    node->token = token->lexeme;
                    node->file = token->file;
                    node->line = token->line;
                    node->type = CNT_RBRACE;

                    // Consume all the newlines
                    i = skipNewlines(i, end);

                    // Insert the node
                    if(parent)
                    {
                        node->parent = parent;
                        parent->children.push_back(node);
                    }
                    else
                    {
                        node->parent = 0;
                        nodes->push_back(node);
                    }

                    // Move up another level
                    if(parent)
                        parent = parent->parent;

                    node = ConcreteNodePtr();
                }
                break;
            case OBJECT:
                if(token->type == TID_NEWLINE)
                {
                    // Look ahead to the next non-newline token and if it isn't an {, this was a property
                    ScriptTokenList::iterator next = skipNewlines(i, end);
                    if(next == end || (*next)->type != TID_LBRACKET)
                    {
                        // Ended a property here
                        if(parent)
                            parent = parent->parent;
                        state = READY;
                    }
                }
                else if(token->type == TID_COLON)
                {
                    node = ConcreteNodePtr(new ConcreteNode());
                    node->token = token->lexeme;
                    node->file = token->file;
                    node->line = token->line;
                    node->type = CNT_COLON;

                    // The following token are the parent objects (base classes).
                    // Require at least one of them.

                    ScriptTokenList::iterator j = i + 1;
                    j = skipNewlines(j, end);
                    if(j == end || ((*j)->type != TID_WORD && (*j)->type != TID_QUOTE)) {
						UNIQUE_LOGERROR(String("expected object identifier at line ") + 
                                    String(node->line));
                    }

                    while(j != end && ((*j)->type == TID_WORD || (*j)->type == TID_QUOTE))
                    {
                        ConcreteNodePtr tempNode = ConcreteNodePtr(new ConcreteNode());
                        tempNode->token = (*j)->lexeme;
                        tempNode->file = (*j)->file;
                        tempNode->line = (*j)->line;
                        tempNode->type = (*j)->type == TID_WORD ? CNT_WORD : CNT_QUOTE;
                        tempNode->parent = node.get();
                        node->children.push_back(tempNode);
                        ++j;
                    }

                    // Move it backwards once, since the end of the loop moves it forwards again anyway
                    i = --j;

                    // Insert the node
                    if(parent)
                    {
                        node->parent = parent;
                        parent->children.push_back(node);
                    }
                    else
                    {
                        node->parent = 0;
                        nodes->push_back(node);
                    }
                    node = ConcreteNodePtr();
                }
                else if(token->type == TID_LBRACKET)
                {
                    node = ConcreteNodePtr(new ConcreteNode());
                    node->token = token->lexeme;
                    node->file = token->file;
                    node->line = token->line;
                    node->type = CNT_LBRACE;

                    // Consume all the newlines
                    i = skipNewlines(i, end);

                    // Insert the node
                    if(parent)
                    {
                        node->parent = parent;
                        parent->children.push_back(node);
                    }
                    else
                    {
                        node->parent = 0;
                        nodes->push_back(node);
                    }

                    // Set the parent
                    parent = node.get();

                    // Change the state
                    state = READY;

                    node = ConcreteNodePtr();
                }
                else if(token->type == TID_RBRACKET)
                {
                    // Go up one level if we can
                    if(parent)
                        parent = parent->parent;

                    // If the parent is currently a { then go up again
                    if(parent && parent->type == CNT_LBRACE && parent->parent)
                        parent = parent->parent;

                    node = ConcreteNodePtr(new ConcreteNode());
                    node->token = token->lexeme;
                    node->file = token->file;
                    node->line = token->line;
                    node->type = CNT_RBRACE;

                    // Consume all the newlines
                    i = skipNewlines(i, end);

                    // Insert the node
                    if(parent)
                    {
                        node->parent = parent;
                        parent->children.push_back(node);
                    }
                    else
                    {
                        node->parent = 0;
                        nodes->push_back(node);
                    }

                    // Move up another level
                    if(parent)
                        parent = parent->parent;

                    node = ConcreteNodePtr();
                    state = READY;
                }
                else if(token->type == TID_VARIABLE)
                {
                    node = ConcreteNodePtr(new ConcreteNode());
                    node->token = token->lexeme;
                    node->file = token->file;
                    node->line = token->line;
                    node->type = CNT_VARIABLE;

                    // Insert the node
                    if(parent)
                    {
                        node->parent = parent;
                        parent->children.push_back(node);
                    }
                    else
                    {
                        node->parent = 0;
                        nodes->push_back(node);
                    }
                    node = ConcreteNodePtr();
                }
                else if(token->type == TID_QUOTE)
                {
                    node = ConcreteNodePtr(new ConcreteNode());
                    node->token = token->lexeme.Substring(1, token->lexeme.Length() - 2);
                    node->file = token->file;
                    node->line = token->line;
                    node->type = CNT_QUOTE;

                    // Insert the node
                    if(parent)
                    {
                        node->parent = parent;
                        parent->children.push_back(node);
                    }
                    else
                    {
                        node->parent = 0;
                        nodes->push_back(node);
                    }
                    node = ConcreteNodePtr();
                }
                else if(token->type == TID_WORD)
                {
                    node = ConcreteNodePtr(new ConcreteNode());
                    node->token = token->lexeme;
                    node->file = token->file;
                    node->line = token->line;
                    node->type = CNT_WORD;

                    // Insert the node
                    if(parent)
                    {
                        node->parent = parent;
                        parent->children.push_back(node);
                    }
                    else
                    {
                        node->parent = 0;
                        nodes->push_back(node);
                    }
                    node = ConcreteNodePtr();
                }
                break;
            }

            ++i;
        }

        return nodes;
    }

    ConcreteNodeListPtr ScriptParser::parseChunk(const ScriptTokenListPtr &tokens)
    {
        // MEMCATEGORY_GENERAL because SharedPtr can only free using that category
        ConcreteNodeListPtr nodes(new ConcreteNodeList());

        ConcreteNodePtr node;
        ScriptToken *token = 0;
        for(ScriptTokenList::const_iterator i = tokens->begin(); i != tokens->end(); ++i)
        {
            token = (*i).get();

            switch(token->type)
            {
            case TID_VARIABLE:
                node = ConcreteNodePtr(new ConcreteNode());
                node->file = token->file;
                node->line = token->line;
                node->parent = 0;
                node->token = token->lexeme;
                node->type = CNT_VARIABLE;
                break;
            case TID_WORD:
                node = ConcreteNodePtr(new ConcreteNode());
                node->file = token->file;
                node->line = token->line;
                node->parent = 0;
                node->token = token->lexeme;
                node->type = CNT_WORD;
                break;
            case TID_QUOTE:
                node = ConcreteNodePtr(new ConcreteNode());
                node->file = token->file;
                node->line = token->line;
                node->parent = 0;
                node->token = token->lexeme.Substring(1, token->lexeme.Length() - 2);
                node->type = CNT_QUOTE;
                break;
            default:
				UNIQUE_LOGERROR(String("unexpected token") + token->lexeme + " at line " + 
                        String(token->line));
            }

            if(node)
                nodes->push_back(node);
        }

        return nodes;
    }

    ScriptToken *ScriptParser::getToken(ScriptTokenList::iterator i, ScriptTokenList::iterator end, int offset)
    {
        ScriptToken *token = 0;
        ScriptTokenList::iterator iter = i + offset;
        if(iter != end)
            token = (*i).get();
        return token;
    }

    ScriptTokenList::iterator ScriptParser::skipNewlines(ScriptTokenList::iterator i, ScriptTokenList::iterator end)
    {
        while(i != end && (*i)->type == TID_NEWLINE)
            ++i;
        return i;
    }

}
