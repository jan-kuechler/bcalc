#include "stdafx.h"

#include "Diag.h"
using namespace tr::Diag;

std::wstring InvalidToken::Describe() const
{
	std::wstring res;
	if (!expr.empty()) {
		size_t pos = got.GetPos() + 7; // 7 == "Error: ".length()
		std::wstring point(pos, L' ');
		point += L'^';
	
		res = expr + L'\n' + point + L'\n';
	}
	res += L"Epxected " + exp.GetDesc() + L" instead of " + got.GetDesc();
	return res;
}   

std::wstring UnknownVariable::Describe() const
{
	return L"Unknown variable: " + name;
}

std::wstring UnknownFunction::Describe() const
{
	return L"Unknown function: " + name;
}

//std::wstring InvalidArguments::Describe() const
//{
//	return L"Invalid arguments for '" + func + L".\n" + L"Expected "
//}
