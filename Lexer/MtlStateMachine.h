#pragma once

__interface IReader {
public:
	virtual char NextChar() = 0;
	virtual char PrevChar() = 0;
	virtual char Read() = 0;
	virtual int MoveNext() = 0;
	virtual int MovePrev() = 0;
};

class MemoryReader : public IReader {
	int m_Pos;
	const char* m_Start;
	const char* m_End;

public:
	MemoryReader(const char* start, const char* end)
		: m_Pos(0)
		, m_Start(start)
		, m_End(end) {};


	// 通过 IReader 继承
	virtual char NextChar() override;

	virtual char PrevChar() override;

	virtual char Read() override;

	virtual int MoveNext() override;

	virtual int MovePrev() override;

};

__interface IStateMachine {
public:
	virtual bool Process(IReader* reader) = 0;
};

__interface IState {
public:
	virtual IState* Process(IStateMachine* machine, IReader* reader) = 0;
};
using DFASTATE = IState * ;

class MtlStateMachine : public IStateMachine
{
	class StateInit :public IState {
		virtual IState* Process(IStateMachine * machine, IReader* reader) override;
	};
	static DFASTATE STATE_INIT = new StateInit;

	class StateObjectBegin :public IState {
		virtual IState* Process(IStateMachine * machine, IReader* reader) override;
	};
	static DFASTATE STATE_OBJECT_BEGIN;

	IState* m_State = STATE_INIT;

	std::stringstream m_Stream;

public:
	MtlStateMachine();
	~MtlStateMachine();

	// 通过 IStateMachine 继承
	virtual bool Process(IReader* reader) override;
};

