#include "stdafx.h"
#include "MtlStateMachine.h"

// http://www.cnblogs.com/welhzh/p/5940886.html
// https://github.com/bjornmp/Men-of-War-Blender-Importer

DFASTATE MtlStateMachine::STATE_INIT = new StateInit;
DFASTATE MtlStateMachine::STATE_OBJECT_BEGIN = new StateObjectBegin;

MtlStateMachine::MtlStateMachine()
{
}


MtlStateMachine::~MtlStateMachine()
{
}

bool MtlStateMachine::Process(IReader* reader)
{
	m_State = m_State->Process(this, reader);

	return false;
}

IState* MtlStateMachine::StateInit::Process(IStateMachine * machine, IReader* reader)
{
	char ch = reader->Read();
	if ('{' == ch) {
		return MtlStateMachine::STATE_OBJECT_BEGIN;
	}

	return false;
}

IState* MtlStateMachine::StateObjectBegin::Process(IStateMachine * machine, IReader* reader)
{
	return false;
}

char MemoryReader::NextChar()
{
	return m_Start[m_Pos+1];
}

char MemoryReader::PrevChar()
{
	return m_Start[m_Pos-1];
}

char MemoryReader::Read()
{
	return m_Start[m_Pos++];
}

int MemoryReader::MoveNext()
{
	m_Pos++;
	return m_Pos;
}

int MemoryReader::MovePrev()
{
	m_Pos--;
	return m_Pos;
}
