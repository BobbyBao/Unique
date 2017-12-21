
#ifndef __SCRIPTPARSER_H_
#define __SCRIPTPARSER_H_

#include "ScriptCompiler.h"
#include "ScriptLexer.h"

namespace Unique
{

    /** \addtogroup Core
    *  @{
    */
    /** \addtogroup General
    *  @{
    */

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
