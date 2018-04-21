#pragma once
#include <stack>
#include "MowParser.h"

namespace MOW {
	class Skeleton : public IModel {
	public:
		virtual bool Process(const std::string& keyword, const std::string& value) override {
			if (keyword.compare("extension") == 0) {
				extf(this, value);
			}
			return true;
		}

	protected:
		bool Extension(const std::string& value) {
			return true;
		}
		friend bool _cdecl ExtensionFunc(IModel* pModel, const std::string& value); /*{
			auto skeleton = reinterpret_cast<Skeleton*>(pModel);
			return skeleton->Extension(value);
		}*/

		ParseFunc extf = ExtensionFunc;
	};

	class SkeletonParser : public IMowParser
	{
	public:
		SkeletonParser() {
			while (m_ModelStack.size()) m_ModelStack.pop();
			while (m_TokenStack.size()) m_TokenStack.pop();
		};

		IModel * Process(const TokenList& tokens) override {
			while (m_ModelStack.size()) m_ModelStack.pop();
			while (m_TokenStack.size()) m_TokenStack.pop();
			IModel* model = nullptr;

			for (auto t = tokens.begin(); t != tokens.end(); t++) {
				Token::Type type = t->getType();
				if (Token::Type::SPAC == type ||
					Token::Type::ANNO == type) {
					continue;
				}
				m_TokenStack.push(t);

				if (Token::Type::IDEN == type
					&& m_TokenStack.top()->getType() == Token::Type::OBJB) {
					if (t->getValue().compare("Skeleton") == 0) {
						model = new Skeleton;
						m_ModelStack.push(model);
					}
					else if (t->getValue().compare("extension") == 0) {
						model = m_ModelStack.top();
						model->Process("extension", t->getValue());
					}
				}
			}

			model = m_ModelStack.top();
			m_ModelStack.pop();
			if (m_ModelStack.size() != 0) {
				throw "Parser stack error.";
			}

			return model;
		}

	protected:
		// Í¨¹ý IMowParser ¼Ì³Ð
		virtual const char * getName() override {
			return "mdl";
		}
	};
};
