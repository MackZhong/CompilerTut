#pragma once
#include <map>
#include <memory>
#include <stack>

#include "Token.h"

namespace MOW {
	class IModel {
	public:
		virtual bool Process(const std::string& keyword, const std::string& value) = 0;
	};

	typedef bool(_cdecl *ParseFunc)(IModel* pModel, const std::string& value);
	//using ParseFunc = bool(IModel* pModel, const std::string& value);
	using ModelStack = std::stack<IModel*>;

	class IMowParser {
	protected:
		using ParserPtr = std::unique_ptr<IMowParser>;
		using ParserMap = std::map<std::string, ParserPtr>;
		static ParserMap m_ParserMap;
		TokenStack m_TokenStack;
		ModelStack m_ModelStack;

		virtual const char* getName() = 0;

		void RegisterMe() {
			if (m_ParserMap.find(getName()) == m_ParserMap.end()) {
				m_ParserMap.try_emplace(getName(), this);
			}
		}

		IMowParser() {
			RegisterMe();
		}

	public:
		static IMowParser* Get(const char* name) {
			if (m_ParserMap.find(name) == m_ParserMap.end()) {
				return m_ParserMap[name].get();
			}
			return nullptr;
		}

		virtual IModel* Process(const TokenList& tokens) = 0;
	};
}
