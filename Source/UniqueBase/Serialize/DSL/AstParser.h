#pragma once
#include <functional>
#include <memory>
#include <map>

namespace Unique
{
	class AstNode;
	typedef UPtr<AstNode> AstNodePtr;
	using ChildMap = std::multimap<String, std::shared_ptr<AstNode>>;

	class AstNode
	{
	public:
		AstNode(const String& type);

		bool IsObject() const { return children_ != nullptr; }

		void AddChild(AstNode* node);
		AstNode* GetChild(const String& name);
		size_t GetChildCount() const { return children_->size(); }
		size_t GetChildCount(const String& key) { return children_->count(key); }
		ChildMap::_Pairii GetIterator(const String& key) { return children_->equal_range(key); }
		void VisitChild(const String& key, const std::function<void(AstNode*)>& fn);
		void Print(int depth);

		String token_;
		String value_;
		bool isQuote_ = false;
	private:
		UPtr<ChildMap> children_;
	};


	class ASTParser
	{
	public:
		ASTParser();

		bool Parse(const String& str);
		bool Parse(const char* buf, size_t size);
		void Print();
		Vector<UPtr<AstNode>> root_;
	private:
		bool Tokenize(const char* buf, size_t size);
		void SetToken(const String &lexeme, uint line);
		void CreateNode(const String& type, uint line);
		AstNode* GetParent() { return parents_.empty() ? nullptr : parents_.back(); }
		String sourceFile_;
		Vector<AstNode*> parents_;
		AstNode* current_;	
	};

	


}