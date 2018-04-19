#pragma once
class Token
{
	friend class LexicalAnalysis;
	enum struct TokenType {
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

	enum TokenType m_Type;
	std::string m_Value;

	std::string TypeedValue(const std::string& value, TokenType type);

public:
	Token( TokenType type, const std::string& value);
	~Token();
};

using TokenPtr = std::shared_ptr<Token>;
