#include<iostream>
#include<string>
#include<sstream>
#include<fstream>

using namespace std;

class LogCommand {
public:
	virtual ~LogCommand()noexcept = default;

	virtual void print(const string& message) = 0;
};

class ConsoleLogCommand : public LogCommand 
{
public:
	~ConsoleLogCommand()noexcept override = default;
	void print(const string& message)override 
	{
		cout << "Console: " << message << endl;
	
	}
};

class FileLogCommand : public LogCommand
{
private:
	string filePath;
public:
	~FileLogCommand()noexcept override = default;
	explicit FileLogCommand(const string& path) : filePath(path) {};

	void print(const string& message)override 
	{
		ofstream outFile(filePath, ios::app);
		if (outFile.is_open()) 
		{
			outFile << message << endl;
		}
		else {
		
			cerr << "Error, connot open file: " << filePath << endl;
		}
	}
};

void print(LogCommand& command) 
{
	string message = "This is a test log message!";
	command.print(message);
}

int main() 
{
	ConsoleLogCommand consoleLoger;
	FileLogCommand fileLoger("log.txt");

	print(consoleLoger);
	print(fileLoger);

	return 0;
}