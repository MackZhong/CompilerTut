#include "stdafx.h"
#include "MowLexer.h"
#include "MowParser.h"
#include "Skeleton.h"

namespace MOW {

	IMowParser::ParserMap IMowParser::m_ParserMap;
	SkeletonParser instance;

	bool _cdecl ExtensionFunc(IModel * pModel, const std::string & value)
	{
		auto skeleton = reinterpret_cast<Skeleton*>(pModel);
		return skeleton->Extension(value);
	}

	//ParseFunc extf = ExtensionFunc;
}