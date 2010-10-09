// bcalc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <boost/date_time.hpp>

#include "Parser.h"
#include "Env.h"

#include "Commands.h"

#include <boost/logic/tribool.hpp>

#define OUT(x) do {std::wcout << x; log << x; } while (0)

typedef std::map<std::wstring, Command> CommandMap;

using namespace boost::logic;

tribool Execute(std::wstring& line, CommandMap& commands, std::wostream& log)
{
	boost::trim(line);
	if (line.empty())
		return true; // nothing to do
	auto split = line.find(L' ');
	std::wstring cmd = line.substr(0, split);

	bool hasArgs = split != line.npos;
	std::wstring args = hasArgs ? line.substr(split+1) : L"";

	bool exit = false;
	if (commands.find(cmd) != commands.end()) {
		auto res = commands[cmd](args);
		switch (std::get<0>(res)) {
		case OK:
			// Silent ok
			break;
		case ERROR:
			{
				std::wstring msg = boost::get<std::wstring>(std::get<1>(res));
				OUT(L"Error: " << msg << std::endl);
				return indeterminate;
			}
			break;
		case RESULT:
			{
				auto data = std::get<1>(res);
				auto res = boost::get<double>(data);
				OUT(res << std::endl);	
			}
			break;
		case MESSAGE:
			{
				auto data = std::get<1>(res);
				auto res = boost::get<std::wstring>(data);
				OUT(res << std::endl);
			}
			break;
		case QUIT:
			return false;
		default:
			assert(false);
		}
	}
	else {
		OUT(L"Error: Unknown command '" << cmd << L"'" << std::endl);
		return indeterminate;
	}
	return true;
}

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

Result do_quit(const std::wstring&)
{
	return std::make_tuple(QUIT, Empty());
}

struct Exec
{
	Result operator()(const std::wstring& args)
	{
		std::wifstream in(args);
		if (!in.is_open()) {
			return ERROR_MSG(L"Could not open '" + args + L"'");
		}

		std::wstring line;
		size_t ln = 0;
		while (readline(in, line, ln)) {
			tribool res = Execute(line, cmds, log);
			if (res == false)
				break;
			else if (indeterminate(res)) {
				return ERROR_MSG(L"In line " + boost::lexical_cast<std::wstring>(ln));
			}
		}
		return std::make_tuple(OK, Empty());
	}

	Exec(CommandMap& c, std::wostream& l)
		: cmds(c), log(l)
	{ }
private:
	CommandMap& cmds;
	std::wostream& log;
};

struct Helper
{
	Result operator()(const std::wstring& arg)
	{
		if (helpStrings.find(arg) != helpStrings.end()) {
			OUT(arg << helpStrings[arg] << std::endl);
		}
		else {
			OUT(L"\t\tbcalc - A batch calculator" << std::endl << std::endl);
			for (auto i=helpStrings.begin(); i!=helpStrings.end(); ++i)
				OUT(i->first << i->second << std::endl);
		}
		return std::make_tuple(OK, Empty());
	}

	Helper(std::wostream& l)
	: log(l)
	{
		helpStrings[L"quit"] = L"                         - Quit bcalc";
		helpStrings[L"exit"] = L"                         - Quit bcalc";
		helpStrings[L"def"]  = L"  <expression>            - Define a function";
		helpStrings[L"set"]  = L"  <expression>            - Set a variable";
		helpStrings[L"calc"] = L" <expression>            - Evaluate an expression and show the result";
		helpStrings[L"run"]  = L"  <file> <expr> [expr...] - Run expressions on the dataset in file";
		helpStrings[L"exec"] = L" <file>                  - Execute a script file";
		helpStrings[L"help"] = L" [cmd]                   - Show help for one or all commands";
	}
private:
	std::wostream& log;
	std::map<std::wstring, std::wstring> helpStrings;
};

int main(int argc, char* argv[])
{
	std::map<std::wstring, Command> commands;	
	std::wofstream log(L"bcalc.log", std::ios::out | std::ios::app | std::ios::ate);
	tr::Env env;
	
	commands[L"quit"] = do_quit;
	commands[L"exit"] = do_quit;
	commands[L"def"]  = Cmd::Def(env);
	commands[L"set"]  = Cmd::Def(env);
	commands[L"calc"] = Cmd::Calc(env);
	commands[L"run"]  = Cmd::Run(env);
	commands[L"exec"] = Exec(commands, log);
	commands[L"help"] = Helper(log);

	{
		boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
		boost::posix_time::wtime_facet *fmt(new boost::posix_time::wtime_facet(L"%d.%m.%Y %H:%M:%S"));
		log.imbue(std::locale(log.getloc(), fmt));
		log << L"****** bcalc - " << now << L" ******" << std::endl;
	}

	OUT(L"> ");
	std::wstring buffer;
	while (std::getline(std::wcin, buffer)) {
		log << buffer << std::endl;

		if (Execute(buffer, commands, log) == false)
			break;
		OUT(L"> ");
	}
	log << std::endl;

	return 0;
}

