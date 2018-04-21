#pragma once
#include "Token.h"
#include <sstream>
#include <memory>
#include <iostream>

class SimpleReader {
private:
	const char* m_Buffer{ nullptr };
	unsigned long m_Length{ 0 };
	unsigned long m_Position{ 0 };
	unsigned long m_Lines{ 0 };
	unsigned short m_LinePos{ 0 };

public:
	SimpleReader(const char* buffer, unsigned long length) : m_Buffer(buffer), m_Length(length) {  }

	void moveNext() {
		m_Position++;
		if (m_Position > m_Length) {
			throw "Buffer overflow.";
		}
		m_LinePos++;
		if ('\n' == m_Buffer[m_Position]) {
			m_Lines++;
			m_LinePos = 0;
		}
	}
	unsigned short getPosition(unsigned long* line) const { *line = m_Lines; return m_LinePos; }
	char getChar() const {
		if (isEnd()) {
			return 0;
		}
		return m_Buffer[m_Position];
	}
	char prevChar() const {
		return m_Buffer[m_Position - 1];
	}
	char nextChar() const {
		return m_Buffer[m_Position + 1];
	}
	bool isEnd() const {
		return  m_Position == m_Length;
	}
	bool isLineEnd() const {
		return '\r' == m_Buffer[m_Position] || '\n' == m_Buffer[m_Position];
	}
};

class MowLexer
{
private:
	std::unique_ptr<SimpleReader> reader;

	std::stringstream m_Stream;
	TokenList m_Tokens;

	bool IsKeyword(const std::string& word) const {
		return false;
	}

	void AddToken(Token::Type type) {
		std::string value = m_Stream.str();
		if (value.compare("Matrix34") == 0) {
			//m_State = m_State;
		}
		if (Token::Type::IDEN == type && IsKeyword(value)) {
			type = Token::Type::KEYW;
			//else if (value.compare("Orientation") == 0) {
			//	m_State = ORIENTATION;
			//}
		}
		TokenPtr token = std::make_shared<Token>(type, value);
		m_Tokens.push_back(token);
		m_Stream.str("");
		std::cout << *token << std::endl;

		static int objLevel = 0;
		if (Token::Type::OBJB == type) {
			objLevel++;
		}
		else if (Token::Type::OBJE == type) {
			objLevel--;
			if (0 == objLevel) {
				objLevel = 0;
			}
		}
	}

	bool ReadString(const char quote) {
		char ch = reader->getChar();
		bool inString = true;
		while (inString) {
			if (reader->isLineEnd()) {
				throw "Invalid string.";
			}
			m_Stream << ch;

			if (quote == ch && '\\' != reader->prevChar())
			{
				inString = false;
			}
			reader->moveNext();
			ch = reader->getChar();
		};
		AddToken(Token::Type::STRI);

		return true;
	}

	bool ReadIdentity() {
		char ch = reader->getChar();
		while (iscsym(ch)) {
			m_Stream << ch;
			reader->moveNext();
			ch = reader->getChar();
		}
		AddToken(Token::Type::IDEN);

		return true;
	}

	bool ReadComment() {
		while (!reader->isLineEnd()) {
			m_Stream << reader->getChar();
			reader->moveNext();
		}
		AddToken(Token::Type::ANNO);

		return true;
	}

	bool ReadNumberic(const char first) {
		char ch = reader->getChar();
		bool isHex = ('0' == first) && ('x' == ch || 'X' == ch);
		bool hasDot = false;
		while ((isHex && isxdigit(ch))
			|| (!isHex && (isdigit(ch) || (!hasDot && '.' == ch) || (isdigit(reader->nextChar() && ('e' == ch || 'E' == ch)))))) {
			m_Stream << ch;
			reader->moveNext();
			ch = ch = reader->getChar();
			if ('f' == ch || 'F' == ch) {
				m_Stream << ch;
				reader->moveNext();
				break;
			}
		}
		AddToken(Token::Type::NUMB);

		return true;
	}

	bool ReadSpaces() {
		char ch = reader->getChar();
		while (isspace(ch)) {
			m_Stream << ch;
			reader->moveNext();
			ch = reader->getChar();
		}
		AddToken(Token::Type::SPAC);

		return true;
	}

	friend std::ostream& operator<<(std::ostream& stm, const MowLexer& me) {
		for (auto t = me.m_Tokens.begin(); t != me.m_Tokens.end(); t++) {
			auto tok = *t;
			stm << *tok << std::endl;
		}
		return stm;
	}

public:
	MowLexer(const char* start, unsigned long length) { reader = std::make_unique<SimpleReader>(start, length); };
	const TokenList& Tokens() const { return m_Tokens; }

	bool Process() {
		bool result = true;

		while (!reader->isEnd()) {
			unsigned long line;
			unsigned short linePos = reader->getPosition(&line);
			char ch = reader->getChar();
			reader->moveNext();
			m_Stream << ch;

			if ('{' == ch) {
				AddToken(Token::Type::OBJB);
			}
			else if ('}' == ch) {
				AddToken(Token::Type::OBJE);
			}
			else if ('\'' == ch || '\"' == ch) {
				result = ReadString(ch);
			}
			else if (';' == ch) {
				result = ReadComment();
			}
			else if (iscsymf(ch)) {
				result = ReadIdentity();
			}
			else if ('-' == ch || '+' == ch || isdigit(ch)) {
				result = ReadNumberic(ch);
			}
			else if (isspace(ch)) {
				result = ReadSpaces();
			}

			if (!result) {
				break;
			}
		}

		return true;
	};
};

