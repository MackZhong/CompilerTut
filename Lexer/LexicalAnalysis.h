#pragma once
#include "Token.h"

class LexicalException : public std::exception {
public:
	LexicalException(char ch, int line, int pos) {
		std::stringstream stm;
		stm << "Lexical error in line " << line << " char " << pos << ": \'" << ch << "\'";
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

	const char* m_Start;
	const char* m_End;
	int m_Line;
	int m_PosInLine;
	int m_Position;
	LexState m_State;
	std::vector<TokenPtr> m_Tokens;
	std::stringstream m_Buffer;

	void AddToken(Token::Type type) {
		AddToken(type, m_Buffer.str());
		m_Buffer.str("");
	}
	void AddToken(Token::Type type, const std::string& value) {
		TokenPtr token = std::make_unique<Token>(type, value);
		m_Tokens.push_back(token);
	}
	void Read(char ch) throw(...);
	char NextChar() const {
		if (m_Start + m_Position + 1 < m_End)
			return m_Start[m_Position + 1];
		else
			throw "Position can't move after the buffer.";
	}
	char PrevChar() const {
		if (m_Position > 0)
			return m_Start[m_Position - 1];
		else
			throw "Position can't move ahead the buffer.";
	}

public:
	LexicalAnalysis(const char* start, const char* end);
	TokenPtr Next() throw ();
	~LexicalAnalysis();
};

