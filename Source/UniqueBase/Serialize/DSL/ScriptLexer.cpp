#include "Precompiled.h"
#include "ScriptLexer.h"

namespace Unique
{
    ScriptTokenListPtr ScriptLexer::tokenize(const String &str, const String &source)
    {
        // State enums
        enum{ READY = 0, COMMENT, MULTICOMMENT, WORD, QUOTE, VAR, POSSIBLECOMMENT };

        // Set up some constant characters of interest
        const wchar_t varopener = '$', quote = '\"', slash = '/', backslash = '\\', openbrace = '{', closebrace = '}', colon = ':', star = '*', cr = '\r', lf = '\n';
        char c = 0, lastc = 0;

        String lexeme;
        uint line = 1, state = READY, lastQuote = 0;
        ScriptTokenListPtr tokens(new ScriptTokenList());

        // Iterate over the input
        auto i = str.Begin(), end = str.End();
        while(i != end)
        {
            lastc = c;
            c = *i;

            if(c == quote)
                lastQuote = line;

            switch(state)
            {
            case READY:
                if(c == slash && lastc == slash)
                {
                    // Comment start, clear out the lexeme
                    lexeme = "";
                    state = COMMENT;
                }
                else if(c == star && lastc == slash)
                {
                    lexeme = "";
                    state = MULTICOMMENT;
                }
                else if(c == quote)
                {
                    // Clear out the lexeme ready to be filled with quotes!
                    lexeme = (String)c;
                    state = QUOTE;
                }
                else if(c == varopener)
                {
                    // Set up to read in a variable
                    lexeme = (String)c;
                    state = VAR;
                }
                else if(isNewline(c))
                {
                    lexeme = (String)c;
                    setToken(lexeme, line, source, tokens.get());
                }
                else if(!isWhitespace(c))
                {
                    lexeme = (String)c;
                    if(c == slash)
                        state = POSSIBLECOMMENT;
                    else
                        state = WORD;
                }
                break;
            case COMMENT:
                if(isNewline(c))
                {
                    lexeme = (String)c;
                    setToken(lexeme, line, source, tokens.get());
                    state = READY;
                }
                break;
            case MULTICOMMENT:
                if(c == slash && lastc == star)
                    state = READY;
                break;
            case POSSIBLECOMMENT:
                if(c == slash && lastc == slash)
                {
                    lexeme = "";
                    state = COMMENT;
                    break;  
                }
                else if(c == star && lastc == slash)
                {
                    lexeme = "";
                    state = MULTICOMMENT;
                    break;
                }
                else
                {
                    state = WORD;
                }
            case WORD:
                if(isNewline(c))
                {
                    setToken(lexeme, line, source, tokens.get());
                    lexeme = (String)c;
                    setToken(lexeme, line, source, tokens.get());
                    state = READY;
                }
                else if(isWhitespace(c))
                {
                    setToken(lexeme, line, source, tokens.get());
                    state = READY;
                }
                else if(c == openbrace || c == closebrace || c == colon)
                {
                    setToken(lexeme, line, source, tokens.get());
                    lexeme = (String)c;
                    setToken(lexeme, line, source, tokens.get());
                    state = READY;
                }
                else
                {
                    lexeme += c;
                }
                break;
            case QUOTE:
                if(c != backslash)
                {
                    // Allow embedded quotes with escaping
                    if(c == quote && lastc == backslash)
                    {
                        lexeme += c;
                    }
                    else if(c == quote)
                    {
                        lexeme += c;
                        setToken(lexeme, line, source, tokens.get());
                        state = READY;
                    }
                    else
                    {
                        // Backtrack here and allow a backslash normally within the quote
                        if(lastc == backslash)
                            lexeme = lexeme + "\\" + c;
                        else
                            lexeme += c;
                    }
                }
                break;
            case VAR:
                if(isNewline(c))
                {
                    setToken(lexeme, line, source, tokens.get());
                    lexeme = (String)c;
                    setToken(lexeme, line, source, tokens.get());
                    state = READY;
                }
                else if(isWhitespace(c))
                {
                    setToken(lexeme, line, source, tokens.get());
                    state = READY;
                }
                else if(c == openbrace || c == closebrace || c == colon)
                {
                    setToken(lexeme, line, source, tokens.get());
                    lexeme = (String)c;
                    setToken(lexeme, line, source, tokens.get());
                    state = READY;
                }
                else
                {
                    lexeme += c;
                }
                break;
            }

            // Separate check for newlines just to track line numbers
            if(c == cr || (c == lf && lastc != cr))
                line++;

            i++;
        }

        // Check for valid exit states
        if(state == WORD || state == VAR)
        {
            if(!lexeme.Empty())
                setToken(lexeme, line, source, tokens.get());
        }
        else
        {
            if(state == QUOTE)
            {
                UNIQUE_LOGERROR(String("no matching \" found for \" at line ") + 
                    String(lastQuote));
            }
        }

        return tokens;
    }

    void ScriptLexer::setToken(const String &lexeme, uint line, const String &source, ScriptTokenList *tokens)
    {
        const char openBracket = '{', closeBracket = '}', colon = ':',
            quote = '\"', var = '$';

        ScriptTokenPtr token(new ScriptToken());
        token->lexeme = lexeme;
        token->line = line;
        token->file = source;
        bool ignore = false;

        // Check the user token map first
        if(lexeme.Length() == 1 && isNewline(lexeme[0]))
        {
            token->type = TID_NEWLINE;
            if(!tokens->empty() && tokens->back()->type == TID_NEWLINE)
                ignore = true;
        }
        else if(lexeme.Length() == 1 && lexeme[0] == openBracket)
            token->type = TID_LBRACKET;
        else if(lexeme.Length() == 1 && lexeme[0] == closeBracket)
            token->type = TID_RBRACKET;
        else if(lexeme.Length() == 1 && lexeme[0] == colon)
            token->type = TID_COLON;
        else if(lexeme[0] == var)
            token->type = TID_VARIABLE;
        else
        {
            // This is either a non-zero length phrase or quoted phrase
            if(lexeme.Length() >= 2 && lexeme[0] == quote && lexeme[lexeme.Length() - 1] == quote)
            {
                token->type = TID_QUOTE;
            }
            else
            {
                token->type = TID_WORD;
            }
        }

        if(!ignore)
            tokens->push_back(token);
    }

    bool ScriptLexer::isWhitespace(char c)
    {
        return c == ' ' || c == '\r' || c == '\t';
    }

    bool ScriptLexer::isNewline(char c)
    {
        return c == '\n' || c == '\r';
    }

}

