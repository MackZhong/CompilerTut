#pragma once
class Token
{
public:
	enum struct Type {
		NONE = 0,
		KEYW = 0x4b455957,
		NUMB = 0x4e554d42,
		IDEN = 0x4944454e,
		SIGN = 0x5349474e,
		ANNO = 0x414e4e4f,
		STRI = 0x53545249,
		REGX = 0x52454758,
		SPAC = 0x53504143,
		NEWL = 0x4e45574c,
		ENDS = 0x454e4453,
		MATX = 0x4d415458,
	};

	Token(Type type, const std::string& value) : m_Type(type), m_Value(value) {};
	~Token() {};

private:
	friend std::ostream& operator<<(std::ostream& stm, const Token& me) {
		char typeName[5] = { 0 };
		memcpy(typeName, &me.m_Type, 4);
		stm << "State: " << typeName << "; Value: " << me.m_Value.c_str() << "." << std::endl;
		return stm;
	};

private:
	Type m_Type;
	std::string m_Value;

	//std::string TypeedValue(const std::string& value, Type type);
};

using TokenPtr = std::shared_ptr<Token>;
using TokenStack = std::stack<TokenPtr>;
using TokenList = std::vector<TokenPtr>;
