#pragma once
#include "Token.h"

class LexicalException : public std::exception {
public:
	LexicalException(char ch, int line, int pos) {
		std::stringstream stm;
		stm << "Lexical error in line " << line << " char "<< pos<< ": \'" << ch << "\'";
		exception(stm.str().c_str());
	}
};

class LexicalAnalysis
{
	enum struct LexState {
		NORMAL = 0,
		OBJECT,
		KEYWORD,
		IDENTIFIER,
		SIGN,
		STRING,
		NUMBER,
		MATRIX,
		REGEX,
		SPACE,
		ANNOTATION,
	};

	int m_Line;
	int m_PosInLine;
	int m_Position;
	LexState m_State;
	std::vector<TokenPtr> m_Tokens;
	std::stringstream m_Buffer;
	void AddToken(Token::TokenType type) {
		TokenPtr token = std::make_unique<Token>(type, m_Buffer.str());
		m_Tokens.push_back(token);
		m_Buffer.str("");
	}
	bool Read(char ch) throw(LexicalException);

public:
	LexicalAnalysis(const char* start, const char* end);
	TokenPtr Next() throw (LexicalException);
	~LexicalAnalysis();
};

