#include "stdafx.h"
#include "Parser.h"
#include "Env.h"
#include "Commands.h"

namespace 
{
	bool readline(std::wistream& in, std::wstring& line, size_t& ln)
	{
		while (std::getline(in, line)) {
			ln++;
			boost::trim(line);
			if (boost::starts_with(line, L"#"))
				continue;
			return true;
		}
		return false;
	}

	const size_t MIN_SIZE = 5;
};

bool Cmd::Run::LoadData(std::wistream& in)
{
	std::wstring line;
	size_t lineNumber = 0;

	if (!readline(in, line, lineNumber)) {
		result = L"Error: Empty data file";
		return false;
	}
	varNames.clear();
	varSets.clear();

	boost::split(varNames, line, boost::is_space());
	auto n = varNames.size();

	while (readline(in, line, lineNumber)) {
		if (line.empty())
			continue;
		std::deque<std::wstring> numbers;
		boost::split(numbers, line, boost::is_space());
		if (numbers.size() != n) {
			result = L"Incorrect number of values in line " + boost::lexical_cast<std::wstring>(lineNumber);
			return false;
		}

		VarSet set;
		for (auto i=numbers.begin(); i!=numbers.end(); ++i) {
			try {
				set.push_back(boost::lexical_cast<double>(*i));
			}
			catch (boost::bad_lexical_cast) {
				result = L"'" + *i + L"' is not a number. Line: " + boost::lexical_cast<std::wstring>(lineNumber);
				return false;
			}
		}
		varSets.push_back(set);
	}

	return true;
}

Result Cmd::Run::operator()(const std::wstring& args)
{
	boost::tokenizer<
		boost::escaped_list_separator<wchar_t>,
		std::wstring::const_iterator,
		std::wstring
	> tok(args.begin(), args.end(), boost::escaped_list_separator<wchar_t>(L'\\', L' ', L'"'));
	std::deque<std::wstring> parts(tok.begin(), tok.end());

	if (parts.size() < 2)
		return ERROR_MSG(L"Usage: run <file> <func> [func...]");

	std::wifstream in(parts[0]);
	if (!in.is_open())
		return ERROR_MSG(L"Could not open '" + parts[0] + L"'");

	functions.clear();
	for (auto i=parts.begin()+1; i!=parts.end(); ++i) {
		try {
			tr::Parser p(*i);
			functions.push_back(std::shared_ptr<tr::Node>(p.Parse()));
		}
		catch (tr::Diagnostics& dg) {
			return ERROR_MSG(dg.Describe());
		}
	}
	funcNames.assign(parts.begin() + 1, parts.end());

	result.clear();
	if (!LoadData(in))
		return ERROR_MSG(result);

#if !USE_FIXED_FORMAT
	BuildFormat();

	size_t len = 0;
	for (size_t i=0; i < funcNames.size(); ++i) {
		size_t c = funcNames[i].length();

		if (c < MIN_SIZE)
			result += std::wstring(MIN_SIZE - c, L' ');
		result += L" " + funcNames[i] + L" ";

		len += std::max(funcNames[i].length(), MIN_SIZE) + 2;
		if (i != funcNames.size() - 1)  {
			result += L"|";
			len++;
		}
	}
	result += L'\n';
	result += std::wstring(len, L'-');
	result += L'\n';
#endif

	resultSets.clear();
	for (auto it = varSets.begin(); it != varSets.end(); ++it) {
		CallFor(*it);
	}

#if USE_FIXED_FORMAT
	std::deque<size_t> minSize;
	resultStrings.clear();
	GetMinSizes(minSize);

	size_t len = AddStringRow(funcNames, minSize);
	result += std::wstring(len, L'-');
	result += L'\n';
	for (size_t s=0; s < resultSets.size(); ++s) {
		AddStringRow(resultStrings[s], minSize);
	}
#endif

	return std::make_tuple(MESSAGE, result);
}

void Cmd::Run::Apply(const VarSet& set)
{
	for (size_t i=0; i < varNames.size(); ++i) {
		env.SetVar(varNames[i], set[i]);
	}
}

bool Cmd::Run::CallFor(const VarSet& set)
{
	Apply(set);
	std::deque<double> res;
	for (auto fn = functions.begin(); fn != functions.end(); ++fn) {
		try {
			res.push_back(env.Eval(*fn));
		}
		catch (tr::Diagnostics& dg) {
			result.clear();
			result = dg.Describe();
			return false;
		}
		catch (std::exception& ex) {
			std::string m = ex.what();
			result.assign(m.begin(), m.end());
			return false;
		}
	}
#if USE_FIXED_FORMAT
	resultSets.push_back(res);
	return true;
#else
	resFmt.clear_binds();
	for (size_t i=0; i < res.size(); ++i) {
		resFmt.bind_arg(i+1, res[i]);
	}
	result += resFmt.str();
	return true;
#endif
}

#if USE_FIXED_FORMAT
size_t Cmd::Run::AddStringRow(const std::deque<std::wstring>& strings, const std::deque<size_t>& minSize)
{
	size_t count = 0;
	for (size_t i=0; i < strings.size(); ++i) {
		size_t len = strings[i].length();
		if (len < minSize[i]) {
			result += std::wstring(minSize[i] - len, L' ');
		}
		result += L" " + strings[i] + L" ";
		if (i != strings.size() - 1) {
			result += L'|';
			count++;
		}
		count += std::max(minSize[i], strings[i].length());
		count += 2;
	}
	result += L'\n';
	return count;
}

void Cmd::Run::GetMinSizes(std::deque<size_t>& ms)
{
	ms.resize(funcNames.size());
	resultStrings.resize(resultSets.size());
	for (size_t set = 0; set != resultSets.size(); ++set) {
		for (size_t i=0; i < resultSets[set].size(); ++i) {
			auto s = boost::lexical_cast<std::wstring>(resultSets[set][i]);
			resultStrings[set].push_back(s);
			ms[i] = std::max(std::max(ms[i], s.length()), MIN_SIZE);
		}
	}
	for (size_t i=0; i < funcNames.size(); ++i) {
		ms[i] = std::max(std::max(ms[i], funcNames[i].length()), MIN_SIZE);
	}
}
#else
void Cmd::Run::BuildFormat()
{
	std::wstring fmt;
	for (size_t i=0; i < funcNames.size(); ++i) {
		auto l = std::max(MIN_SIZE, funcNames[i].length());
		auto w = boost::lexical_cast<std::wstring>(l);
		auto prec = boost::lexical_cast<std::wstring>(l - 2);
		fmt += L" %|" + w + L"." + prec + L"|";
		if (i != funcNames.size() - 1) 
			fmt += L" |";
	}
	fmt += L'\n';
	resFmt.parse(fmt);
}
#endif
