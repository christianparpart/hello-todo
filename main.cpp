#include <iostream>
#include <fstream>
#include <cstdlib>

#include <vector>
#include <string>

/*
 * XXX WAT XXX
 *
 * Write a todo list program that manages a todo list :-D
 *
 * - [x] add a task
 * - [x] list all tasks
 * - [x] mark a task as complete (or incomplete)
 * - [x] delete
 * - [x] update
 * - [x] save it to a file
 * - [x] load it from a file
 */

auto constexpr BOLD = "\033[1m";
auto constexpr STRIKEOUT = "\033[9m";
auto constexpr BLUE = "\033[34m";
auto constexpr RESET = "\033[0m";

struct Task {
	std::string description;
	bool completed;
};

using TaskList = std::vector<Task>;

/// @returns the task description as filled in by the user from @p _input.
Task requestTaskFromUser(std::ostream& _output, std::istream& _input)
{
	_output << "Task description: ";
	_output.flush();

	std::string taskDescription;
	std::getline(_input, taskDescription);

	return Task{taskDescription, false};
}

/// Adds the given task @p _task to the task list @p _tasks.
void addTask(TaskList& _tasks, Task _task)
{
	_tasks.emplace_back(std::move(_task));
}

/// Prints all tasks to an output stream.
///
/// @param _tasks the task list to be printed.
/// @param _output the target stream to print out the list of tasks.
void printAllTasks(TaskList const& _tasks, std::ostream& _output)
{
	_output << _tasks.size() << " tasks:\n"
			<< "========================\n\n";

	for (size_t i = 1; i <= _tasks.size(); ++i) // iterate over i from 1 to _tasks.size()
	{
		Task const& task = _tasks[i - 1];

		_output << i << ". ";

		if (task.completed)
			_output << "✅";
		else
			_output << "  ";

        _output << ' ';

        if (task.completed)
            _output << STRIKEOUT;

		_output << task.description;
        _output << RESET;
        _output << '\n';
	}

	_output << std::endl; // prints '\n' AND forces a flush
}

size_t requestTaskNumber(std::ostream& _out, std::istream& _in, size_t _taskCount)
{
	while (true)
	{
		_out << "Task number: ";
		_out.flush();

		std::string input;
		try
		{
			std::getline(_in, input);
			auto const number = std::stoi(input);

			if (number >= 1 && number <= _taskCount)
				return number;

			_out << "Invalid input. Please try again.\n";
		}
		catch (std::invalid_argument const& e)
		{
			_out << "Invalid input. " << e.what() << " Please try again.\n";
		}
	}
}

/// Toggles a task at given task number @p _taskNumber in task list @p _tasks.
void toggleTaskCompleted(TaskList& _tasks, size_t _taskNumber)
{
	Task& task = _tasks[_taskNumber - 1];
	task.completed = !task.completed;
}

/// Deletes a task at given task number @p _taskNumber in task list @p _tasks.
void updateTask(TaskList& _tasks, std::istream& _in, size_t _taskNumber)
{
	Task& task = _tasks[_taskNumber - 1];
	std::getline(_in, task.description);
}

/// Deletes a task at given task number @p _taskNumber in task list @p _tasks.
void deleteTask(TaskList& _tasks, size_t _taskNumber)
{
	_tasks.erase(next(begin(_tasks), _taskNumber - 1));
}

enum class Command { Invalid, Help, Add, Update, Toggle, Delete, List, Save, Quit };

Command getCommandInput()
{
	std::cout << BOLD << BLUE << "Command (type help for showing help): " << RESET;
	std::cout.flush();

	std::string input;
	std::getline(std::cin, input);

	// TODO: refactor this to look more sexy & concise ;-)

	if (input == "h" || input == "help")
		return Command::Help;

	if (input == "a" || input == "add")
		return Command::Add;

	if (input == "u" || input == "update")
		return Command::Update;

	if (input == "t" || input == "toggle")
		return Command::Toggle;

	if (input == "d" || input == "delete")
		return Command::Delete;

	if (input == "l" || input == "list")
		return Command::List;

	if (input == "q" || input == "quit")
		return Command::Quit;

	if (input == "s" || input == "save")
		return Command::Save;

	return Command::Invalid;
}

void printCommandHelpOutput()
{
	std::cout << "Available commands:\n"
			     "\n"
				 " (h)elp     - prints this help\n"
				 " (a)dd      - adds a new task\n"
				 " (u)pdate   - updates an existing task\n"
				 " (t)oggle   - toggles the completion state of a task\n"
				 " (d)elete   - delete a task\n"
				 " (l)ist     - lists all tasks\n"
				 " (s)ave     - saves the task list to disk (automatic save at program exit, too)\n"
				 " (q)uit     - quits this program\n"
				 "\n";
}

/// Loads the task list from the given test file @p _fileName and returns them as TaskList.
TaskList loadTaskList(std::string const& _fileName)
{
	TaskList tasks;
	std::ifstream input(_fileName);

	while (input.good())
	{
		Task task{};

		input >> task.completed;

		// skipping any leading whitespace
		while (input.peek() == ' ')
			input.get();

		std::getline(input, task.description); // rest of the line is the task description

		if (!task.description.empty())
			tasks.emplace_back(std::move(task));
	}

	return tasks;
}

/// Saves the task list @p _tasks into the file named @p _fileName.
void saveTaskList(TaskList const& _tasks, std::string const& _fileName)
{
	std::cout << "Saving TODO items to: " << _fileName << '\n';

	std::ofstream output(_fileName, std::ios::trunc);

	for (size_t i = 0; i < _tasks.size(); ++i)
	{
		Task const& task = _tasks[i];
		output << task.completed << ' ' << task.description << std::endl;
	}
}

int main(int argc, char const* argv[])
{
	std::string const taskListFilename = "todos.txt";

	TaskList tasks = loadTaskList(taskListFilename);

	printCommandHelpOutput();

	bool done = false;
	while (!done)
	{
		switch (getCommandInput())
		{
			case Command::Quit:
				std::cout << "Quitting. I'm done!\n";
				done = !done;
				break;
			case Command::Invalid:
				std::cout << "Invalid input!. Plese type help for getting list of commands.\n";
				break;
			case Command::Help:
				printCommandHelpOutput();
				break;
			case Command::Add:
				addTask(tasks, requestTaskFromUser(std::cout, std::cin));
				break;
			case Command::Update:
				updateTask(tasks, std::cin, requestTaskNumber(std::cout, std::cin, tasks.size()));
				break;
			case Command::List:
				printAllTasks(tasks, std::cout);
				break;
			case Command::Toggle:
				toggleTaskCompleted(tasks, requestTaskNumber(std::cout, std::cin, tasks.size()));
				break;
			case Command::Delete:
				deleteTask(tasks, requestTaskNumber(std::cout, std::cin, tasks.size()));
				break;
			case Command::Save:
				saveTaskList(tasks, taskListFilename);
				break;
		}
	}

	saveTaskList(tasks, taskListFilename);

	return EXIT_SUCCESS;
}
