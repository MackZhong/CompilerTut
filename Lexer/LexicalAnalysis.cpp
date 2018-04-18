#include "stdafx.h"
#include "LexicalAnalysis.h"

// https://zhuanlan.zhihu.com/moskize
// https://zhuanlan.zhihu.com/p/19879002

LexicalAnalysis::LexicalAnalysis(const char* start, const char* end)
	: m_Start(start)
	, m_End(end)
	, m_State(LexState::NORMAL)
	, m_Line(0)
	, m_PosInLine(0)
	, m_Position(0)
{
}


LexicalAnalysis::~LexicalAnalysis()
{
}

std::unique_ptr<Token> LexicalAnalysis::Next() {

}

void LexicalAnalysis::Read(char ch) throw(LexicalException)
{
	bool movenext = true;
	Token::TokenType type = Token::TokenType::NONE;

	if (m_Start + m_Position >= m_End) {
		type = Token::TokenType::ENDS;
		movenext = false;
		return;
	}

	if (!isprint(ch) || '\n' == ch) {
		return;
	}

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
			movenext = false;
		}
		else if (';' == ch) {
			m_State = LexState::ANNOTATION;
		}
		else if ('`' == ch) {
			m_State = LexState::REGEX;
			movenext = false;
		}
		else if ('\n' == ch) {
			type = Token::TokenType::NEWL;
			if (NextChar() == '\r') m_Position++;
			m_Line++;
			m_PosInLine = 0;
		}
		else if ('\r' == ch) {
			m_State = LexState::NORMAL;
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
	case LexState::OBJECT:
		break;
	case LexState::KEYWORD:
		break;
	case LexState::IDENTIFIER:
		if (isalnum(ch) || '_' == ch) {
			m_Buffer << ch;
		}
		else {
			movenext = false;
			m_State = LexState::NORMAL;
		}
		break;
	case LexState::SIGN:
		break;
	case LexState::STRING:
		if ('\r' == ch || '\n' == ch || '}' == ch) {
			throw LexicalException(ch, m_Line, m_PosInLine);
		}
		else {
			m_Buffer << ch;
			if (m_Buffer[1] == ch) {
				type = Token::TokenType::STRI;
				m_State = LexState::NORMAL;
			}
		}
		break;
	case LexState::NUMBER:
		break;
	case LexState::MATRIX:
		break;
	case LexState::REGEX:
		if ('\r' == ch || '\n' == ch || '}' == ch) {
			throw LexicalException(ch, m_Line, m_PosInLine);
		}
		else {
			m_Buffer << ch;
			if (m_Buffer[1] == ch) {
				type = Token::TokenType::STRI;
				m_State = LexState::NORMAL;
			}
		}
		break;
	case LexState::SPACE:
		if (isspace(ch)) {
			m_Buffer << ch;
		}
		else {
			type = Token::TokenType::SPAC;
			m_State = LexState::NORMAL;
			movenext = false;
		}
		break;
	case LexState::ANNOTATION:
		if ('\r' == ch || '\n' == ch) {
			type = Token::TokenType::ANNO;
			m_State = LexState::NORMAL;
			movenext = false;
		}
		else {
			m_Buffer << ch;
		}
		break;
	}

	if (Token::TokenType::NONE != type) {
		AddToken(type);
	}

	if (movenext) {
		m_Position++;
	}
}
