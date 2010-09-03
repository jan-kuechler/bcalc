#include "stdafx.h"

#include "Diag.h"
using namespace tr::Diag;

std::wstring InvalidToken::Describe() const
{
	size_t pos = got.GetPos();
	std::wstring point(pos, L' ');
	point += L'^';
	
	std::wstring res = point + L'\n';
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
