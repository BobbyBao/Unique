#pragma once
#include <functional>
#include <memory>

namespace Unique
{
	class AstNode;
	typedef UPtr<AstNode> AstNodePtr;
	using ChildMap = std::unordered_multimap<String, std::shared_ptr<AstNode>>;

	class AstNode
	{
	public:
		AstNode(const String& type);

		bool IsObject() const { return children_ != nullptr; }

		void AddChild(AstNode* node);
		AstNode* GetChild(const String& name);
		void VisitChild(const String& key, const std::function<void(AstNode*)>& fn);
		void Print(int depth);

		String type_;
		String value_;
	private:
		//AstNode* parent_ = nullptr;
		UPtr<ChildMap> children_;
	};


	class Tokenizer
	{
	public:
		Tokenizer();

		bool Parse(const String& buf);
		
	private:
		bool Tokenize(const String &str);
		void SetToken(const String &lexeme, uint line);
		void CreateNode(const String& type, uint line);
		AstNode* GetParent() { return parents_.empty() ? nullptr : parents_.back(); }
		String sourceFile_;
		Vector<UPtr<AstNode>> root_;
		Vector<AstNode*> parents_;
		AstNode* current_;
	
		friend class State;
	};

	


}