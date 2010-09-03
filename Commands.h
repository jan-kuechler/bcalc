#ifndef COMMANDS_H
#define COMMANDS_H

enum Status
{
	OK,
	ERROR,
	RESULT,
	MESSAGE,
	QUIT,
};

struct Empty { };
typedef boost::variant<double, std::wstring, Empty> ExtraResult;
typedef std::tuple<Status, ExtraResult> Result;
typedef std::function<Result (const std::wstring&)> Command;

static inline Result ERROR_MSG(const std::wstring& msg)
{
	return std::make_tuple(ERROR, msg);
}

namespace Cmd
{

struct Def
{
	Result operator()(const std::wstring& args) 
	{
		try {
			tr::Parser p(args);
			env.Eval(std::shared_ptr<tr::Node>(p.Parse()));
			return std::make_tuple(OK, Empty());
		}
		catch (tr::Diagnostics& dg) {
			return std::make_tuple(ERROR, dg.Describe());
		}
		catch (std::exception& ex) {
			std::string msg(ex.what());
			return std::make_tuple(ERROR, std::wstring(msg.begin(), msg.end()));
		}
	}

	Def(tr::Env& e)
	: env(e)
	{ }

private:
	tr::Env& env;
};

struct Calc
{
	Result operator()(const std::wstring& args) 
	{
		try {
			tr::Parser p(args);
			double res = env.Eval(std::shared_ptr<tr::Node>(p.Parse()));
			return std::make_tuple(RESULT, res);
		}
		catch (tr::Diagnostics& dg) {
			return std::make_tuple(ERROR, dg.Describe());
		}
		catch (std::exception& ex) {
			std::string msg(ex.what());
			return std::make_tuple(ERROR, std::wstring(msg.begin(), msg.end()));
		}
	}

	Calc(tr::Env& e)
	: env(e)
	{ }

private:
	tr::Env& env;
};

#define USE_FIXED_FORMAT 0

struct Run
{
	Result operator()(const std::wstring& args);

	Run(tr::Env& e)
	: env(e)
	{ }

private:
	typedef std::deque<double> VarSet;

	tr::Env& env;

	std::deque<std::shared_ptr<tr::Node>> functions;
	std::deque<std::wstring> varNames, funcNames;
	std::deque<VarSet> varSets;
	std::deque<VarSet> resultSets;
	std::wstring result;

#if USE_FIXED_FORMAT
	std::deque<std::deque<std::wstring>> resultStrings;
#else
	boost::wformat resFmt;
#endif

	bool LoadData(std::wistream& in);
	bool CallFor(const VarSet& set);
	void Apply(const VarSet& vars);

#if USE_FIXED_FORMAT
	void GetMinSizes(std::deque<size_t>& ms);
	size_t AddStringRow(const std::deque<std::wstring>& strings, const std::deque<size_t>& minSize);
#else
	void BuildFormat();
#endif
};

};

#endif //COMMANDS_H
