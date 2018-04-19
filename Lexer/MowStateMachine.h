#pragma once
#include <ctype.h>
#include <sstream>
#include <memory>

#include "Token.h"

// http://www.cnblogs.com/welhzh/p/5940886.html


__interface IDfaReader {
public:
	virtual char Read() = 0;
	virtual int Forward() = 0;
	virtual int Back() = 0;
	virtual char NextChar() const = 0;
	virtual char PrevChar() const = 0;
	virtual bool End() const = 0;
	virtual bool Endline() const = 0;
};

class MemoryReader : public IDfaReader {
	int m_Pos;
	const char* m_Start;
	size_t m_Length;

	bool Overflow() const {
		return m_Pos < 0 || m_Pos >= m_Length;
	}

public:
	MemoryReader(const char* start, size_t length)
		: m_Pos(0)
		, m_Start(start)
		, m_Length(length) {
		if (this->Overflow()) {
			throw "Memory overflow";
		}
	};

	// 通过 IReader 继承
	virtual char Read() override {
		return m_Start[m_Pos++];
	};
	virtual int Forward() override {
		m_Pos++;
		return m_Pos;
	};
	virtual int Back() override {
		m_Pos--;
		return m_Pos;
	};
	virtual char NextChar() const override {
		return m_Start[m_Pos + 1];
	};
	virtual char PrevChar() const override {
		return m_Start[m_Pos - 1];
	};
	virtual bool End() const override {
		return  m_Pos == m_Length;
	}
	virtual bool Endline() const override {
		return '\r' == m_Start[m_Pos] || '\n' == m_Start[m_Pos];
	}
};

__interface IStateMachine {
public:
	virtual void AddChar(char ch) = 0;
	virtual void AddToken(Token::Type type) = 0;
	virtual bool Process(IDfaReader* reader) = 0;
};

__interface IDfaState {
public:
	virtual const char* Name() = 0;
	virtual IDfaState* Process(IStateMachine* machine, IDfaReader* reader) = 0;
};
using DFASTATE = IDfaState * ;

//#define DELCLARE_DFASTATE(state_name) \
//class DfaState##state_name : public IDfaState{ \
//	virtual const char* Name() override { return #state_name; } \
//	virtual IDfaState* Process(IStateMachine* machine, IReader* reader) override; \
//}; \
//static IDfaState* state_name;
//
//#define IMPLEMENT_DFASTATE(state_name, classname) \
//IDfaState* className::state_name = new DfaState##state_name;
//
//DELCLARE_DFASTATE(MATRIX);

class MowStateMachine : public IStateMachine
{
	class StateInit :public IDfaState {
		virtual const char* Name() override { return "StateInit"; }
		virtual IDfaState* Process(IStateMachine * machine, IDfaReader* reader) override {
			char ch = reader->Read();
			if ('{' == ch) {
				return MowStateMachine::OBJECT_BEGIN;
			}
			else if ('}' == ch) {
				return MowStateMachine::OBJECT_END;
			}
			else if ('\'' == ch || '\"' == ch) {
				return MowStateMachine::STRING;
			}
			else if (';' == ch) {
				return MowStateMachine::COMMENT;
			}
			else if (iscsymf(ch)) {
				reader->Back();
				return MowStateMachine::IDENTITY;
			}
			else if (isdigit(ch)) {
				reader->Back();
				return MowStateMachine::NUMERIC;
			}
			else if (isspace(ch)) {
				return MowStateMachine::SPACE;
			}

			return nullptr;
		};
	};
	static DFASTATE STATE_INIT;

	class StateObjectBegin :public IDfaState {
		virtual const char* Name() override { return "ObjectBegin"; }
		virtual IDfaState* Process(IStateMachine * machine, IDfaReader* reader) override {
			return MowStateMachine::STATE_INIT;
		};
	};
	static DFASTATE OBJECT_BEGIN;

	class StateObjectEnd :public IDfaState {
		virtual const char* Name() override { return "ObjectEnd"; }
		virtual IDfaState* Process(IStateMachine * machine, IDfaReader* reader) override {
			if (reader->End()) {
				return MowStateMachine::STATE_END;
			}

			return MowStateMachine::STATE_INIT;
		};
	};
	static DFASTATE OBJECT_END;

	class StateSpace :public IDfaState {
		virtual const char* Name() override { return "Space"; }
		virtual IDfaState* Process(IStateMachine * machine, IDfaReader* reader) override {
			while (isspace(reader->Read()));
			reader->Back();

			return MowStateMachine::STATE_INIT;
		};
	};
	static DFASTATE SPACE;

	class StateIdentity :public IDfaState {
		virtual const char* Name() override { return "Identity"; }
		virtual IDfaState* Process(IStateMachine * machine, IDfaReader* reader) override {
			char ch = reader->Read();
			while (iscsymf(ch)) {
				machine->AddChar(ch);
				ch = reader->Read();
			}
			machine->AddToken(Token::Type::IDEN);

			if ('}' == ch) {
				return MowStateMachine::OBJECT_END;
			}
			return MowStateMachine::STATE_INIT;
		};
	};
	static DFASTATE IDENTITY;

	class StateMatrix3x4 :public StateIdentity {
		virtual const char* Name() override { return "Matrix3x4"; }
		virtual IDfaState* Process(IStateMachine * machine, IDfaReader* reader) override {
			return nullptr;
		};
	};
	static DFASTATE MATRIX3x4;

	class StateString :public IDfaState {
		virtual const char* Name() override { return "String"; }
		virtual IDfaState* Process(IStateMachine * machine, IDfaReader* reader) override {
			char quote = reader->PrevChar();
			char ch = reader->Read();
			while (ch != quote && !reader->Endline()) {
				machine->AddChar(ch);
				ch = reader->Read();
			}
			machine->AddToken(Token::Type::STRI);

			return MowStateMachine::STATE_INIT;
		};
	};
	static DFASTATE STRING;

	class StateComment :public IDfaState {
		virtual const char* Name() override { return "Comment"; }
		virtual IDfaState* Process(IStateMachine * machine, IDfaReader* reader) override {
			char ch = reader->Read();
			while (!reader->Endline()) {
				machine->AddChar(ch);
				ch = reader->Read();
			}
			machine->AddToken(Token::Type::ANNO);

			return MowStateMachine::STATE_INIT;
		};
	};
	static DFASTATE COMMENT;

	class StateNumeric :public IDfaState {
		virtual const char* Name() override { return "Numeric"; }
		virtual IDfaState* Process(IStateMachine * machine, IDfaReader* reader) override {
			char ch = reader->Read();
			bool isHex = ('0' == ch) && ('x' == reader->NextChar() || 'X' == reader->NextChar());
			bool hasDot = false;
			while ((isHex && isxdigit(ch))
				|| (!isHex && (isdigit(ch) || (!hasDot && '.' == ch) || (isdigit(reader->NextChar() && ('e' == ch || 'E' == ch)))))) {
				machine->AddChar(ch);
				ch = reader->Read();
				if ('f' == ch || 'F' == ch) {
					ch = reader->Read();
					break;
				}
			}
			machine->AddToken(Token::Type::NUMB);

			return MowStateMachine::STATE_INIT;
		};
	};
	static DFASTATE NUMERIC;

	class StateEnd :public IDfaState {
		virtual const char* Name() override { return "StateEnd"; }
		virtual IDfaState* Process(IStateMachine * machine, IDfaReader* reader) override {
			return nullptr;
		};
	};
	static DFASTATE STATE_END;

	IDfaState* m_State = STATE_INIT;

	std::stringstream m_Stream;
	TokenList	m_Tokens;

	void AddChar(char ch) { m_Stream << ch; }

	void AddToken(Token::Type type) {
		std::string value = m_Stream.str();
		if (Token::Type::IDEN == type && IsKeyword(value)) {
			type = Token::Type::KEYW;
			if (value.compare("Matrix3x4") == 0) {
				//m_State =
			}
		}
		TokenPtr token = std::make_shared<Token>(type, value);
		m_Tokens.push_back(token);
		m_Stream.str("");
	}

	bool IsKeyword(const std::string& word) {
		return false;
	}

public:
	MowStateMachine() {
		if (!STATE_INIT) STATE_INIT = new StateInit;
		if (!STATE_END) STATE_END = new StateEnd;
		if (!OBJECT_BEGIN) OBJECT_BEGIN = new StateObjectBegin;
		if (!OBJECT_END) OBJECT_END = new StateObjectEnd;
		if (!IDENTITY) IDENTITY = new StateIdentity;
		if (!MATRIX3x4) MATRIX3x4 = new StateMatrix3x4;
		if (!STRING) STRING = new StateString;
		if (!COMMENT) COMMENT = new StateComment;
		if (!NUMERIC) NUMERIC = new StateNumeric;
		if (!SPACE) SPACE = new StateSpace;
	};
	~MowStateMachine() {
		if (STATE_INIT) delete STATE_INIT; STATE_INIT = nullptr;
		if (STATE_END) delete STATE_END; STATE_END = nullptr;
		if (OBJECT_BEGIN) delete OBJECT_BEGIN; OBJECT_BEGIN = nullptr;
		if (OBJECT_END) delete OBJECT_END; OBJECT_END = nullptr;
		if (IDENTITY) delete IDENTITY; IDENTITY = nullptr;
		if (MATRIX3x4) delete MATRIX3x4; MATRIX3x4 = nullptr;
		if (STRING) delete STRING; STRING = nullptr;
		if (COMMENT) delete COMMENT; COMMENT = nullptr;
		if (NUMERIC) delete NUMERIC; NUMERIC = nullptr;
		if (SPACE) delete SPACE; SPACE = nullptr;
	};

	// 通过 IStateMachine 继承
	virtual bool Process(IDfaReader* reader) override {
		m_State = STATE_INIT;
		m_Stream.str("");
		m_Tokens.clear();

		while (MowStateMachine::STATE_END != m_State) {
			std::string name = m_State->Name();
			std::cout << name.c_str() << std::endl;
			m_State = m_State->Process(this, reader);
			if (!m_State) {
				throw "Lexical error.";
			}
		}

		return false;
	};

private:
	friend std::ostream& operator<<(std::ostream& stm, const MowStateMachine& me) {
		for (auto t = me.m_Tokens.begin(); t != me.m_Tokens.end(); t++) {
			auto tok = *t;
			stm << *tok << std::endl;
		}
		return stm;
	}
};

#ifndef DFASTATE_DEFINE
#define DFASTATE_DEFINE
DFASTATE MowStateMachine::STATE_INIT = nullptr;
DFASTATE MowStateMachine::STATE_END = nullptr;
DFASTATE MowStateMachine::OBJECT_BEGIN = nullptr;
DFASTATE MowStateMachine::OBJECT_END = nullptr;
DFASTATE MowStateMachine::IDENTITY = nullptr;
DFASTATE MowStateMachine::MATRIX3x4 = nullptr;
DFASTATE MowStateMachine::STRING = nullptr;
DFASTATE MowStateMachine::COMMENT = nullptr;
DFASTATE MowStateMachine::NUMERIC = nullptr;
DFASTATE MowStateMachine::SPACE = nullptr;
#endif