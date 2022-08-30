#include "common.h"
#include "factor.h"

class argument_list
{
	list<string> args;
public:
	argument_list(int argc, char *argv[], const string& self)
	{
		copy_n(argv, argc, back_inserter(args));
		if (!args.empty() && (*args.cbegin()).ends_with(self)) {
			args.pop_front();
		}
		if (args.empty()) {
			args.emplace_back("show-help");
		}
	}

	string take_string()
	{
		if (args.empty()) {
			throw invalid_argument("Unexpected end of input");
		}
		auto arg = *args.begin();
		args.pop_front();
		return arg;
	}

	integer take_integer()
	{
		return stoll(take_string());
	}

	void end()
	{
		if (!args.empty()) {
			throw invalid_argument("Unexpected extra argument");
		}
	}
};

class command_definition
{
public:
	const string long_name;
	const string short_name;
	const string description;
	const vector<pair<string, string>> signature;

	command_definition(string long_name, string short_name, string description, vector<pair<string, string>> signature) :
		long_name(long_name),
		short_name(short_name),
		description(description),
		signature(signature) { }
	virtual ~command_definition() = default;
	virtual void operator () (argument_list& args) = 0;
};

struct command_find_primes : command_definition
{
	command_find_primes() :
		command_definition(
			"find-primes",
			"primes",
			"Finds all prime numbers up to and including the target",
			{
				{ "until", "Upper-bound (not required to be prime)" },
			}) { }
	virtual void operator() (argument_list& args)
	{
		auto until = args.take_integer();
		args.end();
		auto result = find_primes(until);
		for (auto prime : result) {
			cout << prime << endl;
		}
	}
};

struct command_find_factors : command_definition
{
	command_find_factors() :
		command_definition(
			"find-factors",
			"factorise",
			"Finds all factors of a number, and their degrees",
			{
				{ "value", "the value to factorise" },
			}) { }
	virtual void operator() (argument_list& args)
	{
		auto value = args.take_integer();
		args.end();
		auto result = find_factors(value);
		for (auto [factor, count] : result) {
			cout << factor << "^" << count << endl;
		}
	}
};

struct command_find_lowest_common_factor : command_definition
{
	command_find_lowest_common_factor() :
		command_definition(
			"find-lowest-common-factors",
			"lcf",
			"Finds the lowest common factor of two numbers, i.e. the smallest number which divides into both of them",
			{
				{ "a", "value to find a common factor for" },
				{ "b", "value to find a common factor for" },
			}) { }
	virtual void operator() (argument_list& args)
	{
		auto a = args.take_integer();
		auto b = args.take_integer();
		args.end();
		auto result = find_lowest_common_factor(a, b);
		cout << result << endl;
	}
};

struct command_find_greatest_common_factor : command_definition
{
	command_find_greatest_common_factor() :
		command_definition(
			"find-greatest-common-factors",
			"gcf",
			"Finds the lowest common factor of two numbers, i.e. the greatest number which divides into both of them",
			{
				{ "a", "value to find a common factor for" },
				{ "b", "value to find a common factor for" },
			}) { }
	virtual void operator() (argument_list& args)
	{
		auto a = args.take_integer();
		auto b = args.take_integer();
		args.end();
		auto result = find_greatest_common_factor(a, b);
		cout << result << endl;
	}
};

struct command_find_lowest_common_multiple : command_definition
{
	command_find_lowest_common_multiple() :
		command_definition(
			"find-lowest-common-multiples",
			"lcm",
			"Finds the lowest common multiple of two numbers, i.e. the lowest number which divides by both of them",
			{
				{ "a", "value to find a common multiple for" },
				{ "b", "value to find a common multiple for" },
			}) { }
	virtual void operator() (argument_list& args)
	{
		auto a = args.take_integer();
		auto b = args.take_integer();
		args.end();
		auto result = find_lowest_common_multiple(a, b);
		cout << result << endl;
	}
};

class command_table;

struct command_show_help : command_definition
{
	command_table& table;

	command_show_help(command_table &table) :
		command_definition(
			"show-help",
			"help",
			"Show a list of commands and their signatures",
			{ }),
		table(table)
		{ }
	virtual void operator() (argument_list& args);
};

class command_table
{
	map<string, shared_ptr<command_definition>> table;
public:
	template <typename CommandDefinition, typename... Args>
	void add(Args&&... args)
	{
		auto ptr = make_shared<CommandDefinition>(forward<Args>(args)...);
		table[ptr->long_name] = ptr;
		table[ptr->short_name] = ptr;
	}

	void show_help()
	{
		cout << setw(0);
		cout << "Command table" << endl;
		cout << endl;
		struct command_compare
		{
			bool operator () (const command_definition *l, const command_definition *r) const
			{
				return lexicographical_compare(l->long_name.cbegin(), l->long_name.cend(), r->long_name.cbegin(), r->long_name.cend());
			}
		};
		set<command_definition *, command_compare> commands;
		for (const auto& [name, cmd] : table) {
			commands.emplace(cmd.get());
		}
		for (const auto& cmd : commands) {
			cout << cmd->short_name << ": " << cmd->description << endl;
			for (const auto& [name, description] : cmd->signature) {
				cout << "  " << setw(10) << name << setw(0) << " : " << description << endl;
			}
			cout << endl;
		}
		cout << endl;
		for (const auto& cmd : commands) {
			cout << setw(20) << cmd->short_name << setw(0) << " <=> " << cmd->long_name << endl;
		}
		cout << endl;
		cout << setw(0);
	}

	void execute(argument_list args)
	{
		auto command = table.find(args.take_string());
		if (command == table.cend()) {
			throw invalid_argument("Invalid command");
		}
		auto& handler = *command->second;
		handler(args);
	}
};

void command_show_help::operator() (argument_list& args)
{
	args.end();
	table.show_help();
}

int main(int argc, char *argv[])
{
	command_table commands;
	commands.add<command_find_primes>();
	commands.add<command_find_factors>();
	commands.add<command_find_lowest_common_factor>();
	commands.add<command_find_greatest_common_factor>();
	commands.add<command_find_lowest_common_multiple>();
	commands.add<command_show_help>(commands);

	commands.execute({ argc, argv, "numutils" });

	return 0;
}
