#include "Precompiled.h"
#include "AstParser.h"
#include <cassert>
#include <iostream>

namespace Unique
{
	static bool isWhitespace(char c)
	{
		return c == ' ' || c == '\r' || c == '\t';
	}

	static bool isNewline(char c)
	{
		return c == '\n' || c == '\r';
	}

	AstNode::AstNode(const String& type)
		: type_(type)
	{
	}

	void AstNode::AddChild(AstNode* node)
	{
		if (children_ == nullptr)
		{
			children_.reset(new ChildMap);
		}

		children_->insert(std::make_pair(node->type_, node));
	//	node->parent_ = this;
	}

	AstNode* AstNode::GetChild(const String& name)
	{
		auto it = children_->find(name);
		if (it != children_->end())
		{
			return it->second.get();
		}

		return nullptr;
	}

	void AstNode::VisitChild(const String& key, const std::function<void(AstNode*)>& fn)
	{
		auto range = children_->equal_range(key);
		for (auto it = range.first; it != range.second; ++it) 
		{
			fn(it->second.get());
		}

	}

	void AstNode::Print(int depth)
	{
		std::cout << type_ << " " << value_ << std::endl;
		
		if (IsObject())
		{
			std::cout << "{" << std::endl;

			for (auto& n : *children_)
			{
				n.second->Print(depth + 1);
			}

			std::cout << "}" << std::endl;
		}

	}

	AstParser::AstParser()
	{
	}

	bool AstParser::Parse(const String& str)
	{
		return Parse(str.CString(), str.Length());
	}

	void AstParser::Print()
	{
		for (auto& n : root_)
		{
			n->Print(0);
		}
	}

	bool AstParser::Parse(const char* buf, size_t size)
	{
		if (!Tokenize(buf, size))
		{
			return false;
		}

		return !root_.empty();
	}

	bool AstParser::Tokenize(const char* buf, size_t size)
	{
		// State enums
		enum { READY = 0, COMMENT, MULTICOMMENT, WORD, QUOTE, POSSIBLECOMMENT };

		// Set up some constant characters of interest
		const wchar_t quote = '\"', slash = '/', backslash = '\\', openbrace = '{', closebrace = '}', colon = ':', star = '*', cr = '\r', lf = '\n';
		char c = 0, lastc = 0;

		String lexeme;
		uint line = 1, state = READY, lastQuote = 0;

		// Iterate over the input
		auto i = buf, end = buf + size;
		while (i != end)
		{
			lastc = c;
			c = *i;

			if (c == quote)
				lastQuote = line;

			switch (state)
			{
			case READY:
				if (c == slash && lastc == slash)
				{
					// Comment start, clear out the lexeme
					lexeme = "";
					state = COMMENT;
				}
				else if (c == star && lastc == slash)
				{
					lexeme = "";
					state = MULTICOMMENT;
				}
				else if (c == quote)
				{
					// Clear out the lexeme ready to be filled with quotes!
					lexeme = (String)c;
					state = QUOTE;
				}
				else if (isNewline(c))
				{
					lexeme = (String)c;
					SetToken(lexeme, line);
				}
				else if (!isWhitespace(c))
				{
					lexeme = (String)c;
					if (c == slash)
						state = POSSIBLECOMMENT;
					else
						state = WORD;
				}
				break;
			case COMMENT:
				if (isNewline(c))
				{
					lexeme = (String)c;
					SetToken(lexeme, line);
					state = READY;
				}
				break;
			case MULTICOMMENT:
				if (c == slash && lastc == star)
					state = READY;
				break;
			case POSSIBLECOMMENT:
				if (c == slash && lastc == slash)
				{
					lexeme = "";
					state = COMMENT;
					break;
				}
				else if (c == star && lastc == slash)
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
				if (isNewline(c))
				{
					SetToken(lexeme, line);
					lexeme = (String)c;
					SetToken(lexeme, line);
					state = READY;
				}
				else if (isWhitespace(c))
				{
					SetToken(lexeme, line);
					state = READY;
				}
				else if (c == openbrace || c == closebrace || c == colon)
				{
					SetToken(lexeme, line);
					lexeme = (String)c;
					SetToken(lexeme, line);
					state = READY;
				}
				else
				{
					lexeme += c;
				}
				break;
			case QUOTE:
				if (c != backslash)
				{
					// Allow embedded quotes with escaping
					if (c == quote && lastc == backslash)
					{
						lexeme += c;
					}
					else if (c == quote)
					{
						lexeme += c;
						SetToken(lexeme, line);
						state = READY;
					}
					else
					{
						// Backtrack here and allow a backslash normally within the quote
						if (lastc == backslash)
							lexeme = lexeme + "\\" + c;
						else
							lexeme += c;
					}
				}
				break;
			}

			// Separate check for newlines just to track line numbers
			if (c == cr || (c == lf && lastc != cr))
				line++;

			i++;
		}

		// Check for valid exit states
		if (state == WORD)
		{
			if (!lexeme.Empty())
				SetToken(lexeme, line);
		}
		else
		{
			if (state == QUOTE)
			{
				UNIQUE_LOGERROR(String("no matching \" found for \" at line ") +
					String(lastQuote));
				return false;
			}
		}

		return true;
	}

	void AstParser::SetToken(const String &lexeme, uint line)
	{
		const char openBracket = '{', closeBracket = '}', quote = '\"';

		// Check the user token map first
		if (lexeme.Length() == 1 && isNewline(lexeme[0]))
		{
			//tokenType = TID_NEWLINE;
		}
		else if (lexeme.Length() == 1 && lexeme[0] == openBracket)
		{
			//tokenType = TID_LBRACKET;
			assert(current_ != nullptr);
			parents_.push_back(current_);
		}
		else if (lexeme.Length() == 1 && lexeme[0] == closeBracket)
		{
			//tokenType = TID_RBRACKET;
			parents_.pop_back();
		}
		else
		{
			bool isQuote = false;
			// This is either a non-zero length phrase or quoted phrase
			if (lexeme.Length() >= 2 && lexeme[0] == quote && lexeme[lexeme.Length() - 1] == quote)
			{
				//tokenType = TID_QUOTE;
				isQuote = true;
			}
			else
			{
				//tokenType = TID_WORD;
			}

			if (current_)
			{
				if (!current_->value_.Empty())
				{
					CreateNode(isQuote ? lexeme.Substring(1, lexeme.Length() - 2) : lexeme, line);
				}
				else
				{
					current_->value_ = isQuote ? lexeme.Substring(1, lexeme.Length() - 2) : lexeme;
				}
			}
			else
			{
				CreateNode(isQuote ? lexeme.Substring(1, lexeme.Length() - 2) : lexeme, line);
			}
		}


	}

	void AstParser::CreateNode(const String& type, uint line)
	{
		current_ = new AstNode(type);
		AstNode* parent = GetParent();
		if (parent)
		{
			parent->AddChild(current_);
		}
		else
		{
			root_.push_back(AstNodePtr(current_));
		}
	}
}

