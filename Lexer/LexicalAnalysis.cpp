#include "stdafx.h"
#include "LexicalAnalysis.h"

// https://zhuanlan.zhihu.com/moskize
// https://zhuanlan.zhihu.com/p/19879002

LexicalAnalysis::LexicalAnalysis(const char* start, const char* end)
{
}


LexicalAnalysis::~LexicalAnalysis()
{
}

std::unique_ptr<Token> LexicalAnalysis::Next() {

}

bool LexicalAnalysis::Read(char ch) throw(LexicalException)
{
	bool next = true;
	Token::TokenType type = Token::TokenType::NONE;

	switch (m_State) {
	case LexState::NORMAL:
		if ('{' == ch) {
			m_State = LexState::OBJECT;
		}
		else if ('}' == ch) {
			m_State = LexState::NORMAL;
		}
		else if ('\'' == ch || '\"' == ch) {
			m_State = LexState::STRING;
			next = false;
		}
		else if (';' == ch) {
			m_State = LexState::ANNOTATION;
		}
		else if ('`' == ch) {
			m_State = LexState::REGEX;
		}
		else if ('\n' == ch) {
			type = Token::TokenType::NEWL;
			m_Line++;
			m_PosInLine = 0;
		}
		else if (0 == ch) {
			type = Token::TokenType::ENDS;
		}
		else if (isalpha(ch) || '_' == ch) {
			m_State = LexState::IDENTIFIER;
		}
		else if (isdigit(ch)) {
			m_State = LexState::NUMBER;
		}
		else if (isspace(ch)) {
			m_State = LexState::SPACE;
		}
		else {
			throw LexicalException(ch, m_Line, m_PosInLine);
		}
		break;
	case LexState::OBJECT :
		break;
	case LexState::KEYWORD :
		break;
	case LexState::IDENTIFIER :
		break;
	case LexState::SIGN :
		break;
	case LexState::STRING :
		break;
	case LexState::NUMBER :
		break;
	case LexState::MATRIX :
		break;
	case LexState::REGEX :
		break;
	case LexState::SPACE :
		break;
	case LexState::ANNOTATION :
		break;
	}

	if (Token::TokenType::NONE != type) {
		AddToken(type);
	}

	if (next) {
		m_Position++;
	}

	return next;
}
