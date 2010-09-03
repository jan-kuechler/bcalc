// bcalc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <boost/date_time.hpp>

#include "Parser.h"
#include "Env.h"

#include "Commands.h"

Result do_quit(const std::wstring&)
{
	return std::make_tuple(QUIT, Empty());
}

#define OUT(x) do {std::wcout << x; log << x; } while (0)

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
		boost::trim(buffer);
		auto split = buffer.find(L' ');
		std::wstring cmd = buffer.substr(0, split);

		bool hasArgs = split != buffer.npos;
		std::wstring args = hasArgs ? buffer.substr(split+1) : L"";

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
					break;
				}
			case RESULT:
				{
					auto data = std::get<1>(res);
					auto res = boost::get<double>(data);
					OUT(res << std::endl);
					break;
				}
			case MESSAGE:
				{
					auto data = std::get<1>(res);
					auto res = boost::get<std::wstring>(data);
					OUT(res << std::endl);
					break;
				}
			case QUIT:
				exit = true;
				break;
			default:
				assert(false);
			}
		}
		else {
			OUT(L"Error: Unknown command '" << cmd << L"'" << std::endl);
		}
		if (exit)
			break;
		OUT(L"> ");
	}
	log << std::endl;

	return 0;
}

