#pragma once
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace MOW {
	class Token
	{
	public:
		enum struct Type {
			NONE = 0,
			KEYW = 0x5759454b,
			OBJB = 0x424a424f,
			OBJE = 0x454a424f,
			NUMB = 0x424d554e,
			IDEN = 0x4e454449,
			SIGN = 0x4e474953,
			ANNO = 0x4f4e4e41,
			STRI = 0x49525453,
			REGX = 0x58474552,
			SPAC = 0x43415053,
			NEWL = 0x4c57454e,
			ENDS = 0x53444e45,
			MATX = 0x5854414d,
		};

		Token(Type type, const std::string& value) : m_Type(type), m_Value(value) {};
		Type getType()const { return m_Type; }
		const std::string& getValue() const { return m_Value; }

	private:
		friend std::ostream& operator<<(std::ostream& stm, const Token& me) {
			char typeName[5] = { 0 };
			memcpy(typeName, &me.m_Type, 4);
			stm << /*"{" << */typeName << ": ";
			const char escaped[] = "\n\t\v\b\r\f\a";
			const char escape[] = "ntvbrfa";
			for (auto s = me.m_Value.begin(); s != me.m_Value.end(); s++) {
				const char* find = strchr(escaped, *s);
				if (find) {
					stm << '\\' << (escape + (find - escaped))[0];
				}
				else if (' ' == *s) {
					stm << "\\s";
				}
				else {
					stm << *s;
				}
			}
			//stm << "}";

			return stm;
		};

	private:
		Type m_Type;
		std::string m_Value;

		//std::string TypeedValue(const std::string& value, Type type);
	};

	using TokenPtr = std::shared_ptr<Token>;
	using TokenList = std::vector<Token>;
	using TokenStack = std::stack<TokenList::const_iterator>;
}