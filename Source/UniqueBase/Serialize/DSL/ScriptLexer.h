
#ifndef __ScriptLexer_H_
#define __ScriptLexer_H_

namespace Unique
{
    /** \addtogroup Core
    *  @{
    */
    /** \addtogroup General
    *  @{
    */
    /** These codes represent token IDs which are numerical translations of
        specific lexemes. Specific compilers using the lexer can register their
        own token IDs which are given precedence over these built-in ones.
    */
    enum
	{
        TID_LBRACKET = 0, // {
        TID_RBRACKET, // }
        TID_COLON, // :
        TID_VARIABLE, // $...
        TID_WORD, // *
        TID_QUOTE, // "*"
        TID_NEWLINE, // \n
        TID_UNKNOWN,
        TID_END
    };

    /** This struct represents a token, which is an ID'd lexeme from the
        parsing input stream.
    */
    struct ScriptToken
    {
        /// This is the lexeme for this token
		String lexeme;//, file;
        /// This is the id associated with the lexeme, which comes from a lexeme-token id mapping
        uint type;
        /// This holds the line number of the input stream where the token was found.
        uint line;
    };

    typedef Vector<ScriptToken> ScriptTokenList;
    typedef std::shared_ptr<ScriptTokenList> ScriptTokenListPtr;

    class UNIQUE_API ScriptLexer
    {
    public:
        /** Tokenizes the given input and returns the list of tokens found */
        static ScriptTokenListPtr tokenize(const String &str, const String &source);
    private: // Private utility operations
        static void setToken(const String &lexeme, uint line, const String &source, ScriptTokenList *tokens);
        static bool isWhitespace(char c);
        static bool isNewline(char c);
    };

    /** @} */
    /** @} */
}


#endif
