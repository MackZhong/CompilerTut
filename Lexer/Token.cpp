#include "stdafx.h"
#include "Token.h"


Token::Token(TokenType type, const std::string& value)
	:m_Type(type)
	, m_Value(TypeedValue(value, type))
{
}

std::string Token::TypeedValue(const std::string& value, TokenType type) {
	switch (type)
	{
	case TokenType::ANNO:
		return value.substr(1);
	case TokenType::STRI:
	case TokenType::REGX:
		return value.substr(1, value.length()-1);
	case TokenType::ENDS:
		return nullptr;
	default:
		return value;
	}
}

Token::~Token()
{
}
