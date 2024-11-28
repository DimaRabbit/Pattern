#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<memory>
#include<fstream>

using namespace std;

class Observer 
{
public:
	virtual ~Observer() = default;

	virtual void onWarning(const string& message) {}
	virtual void onError(const string& message) {}
	virtual void onFatalError(const string& message) {}
};

class Observable 
{
private:
	vector<Observer*>observers;
public:
	~Observable() {
		observers.clear();
	}
	void addObserver(Observer* observer) {
		if (find(observers.begin(), observers.end(), observer)==observers.end()) {
			observers.push_back(observer);
		}
	}
	void removeObserver(Observer* observer) {
		observers.erase(remove(observers.begin(), observers.end(), observer), observers.end());
	}
	void warning(const string& message)const 
	{
		for (auto observer : observers) 
		{
			if (observer)observer->onWarning(message);
		
		}
	}
	void error(const string& message)const 
	{
		for (auto observer : observers)
		{
			if (observer)observer->onError(message);

		}
	}
	void fatallError(const string& message)const
	{
		for (auto observer : observers)
		{
			if (observer)observer->onFatalError(message);

		}
	}

};
class WarningObserver : public Observer 
{
public:
	void onWarning(const string& message) override 
	{
		cout << "Warning: " << message << endl;
	}
};

class errorObserver :public Observer 
{
private:
	string filePath;
public:
	explicit errorObserver(const string& path) : filePath(path) {}
	void onError(const string& message) override 
	{
		ofstream outfile(filePath, ios::app);
		if (outfile.is_open()) 
		{
			outfile << "Error: " << message << endl;
			outfile.close();
		}
		else { 
			cerr << "Cannot open file: " << filePath << endl;
		
		}
	}

};

class FatalErrorObserver :public Observer 
{
private:
	string filePath;
public:
	explicit FatalErrorObserver(const string& path) : filePath(path) {}
	void onFatalError(const string& message)override 
	{
		cout << "Fatal error: " << message << endl;

		ofstream outfile(filePath, ios::app);
		if (outfile.is_open())
		{
			outfile << "Fatal Error: " << message << endl;
			outfile.close();
		}
		else {
			cerr << "Cannot open file: " << filePath << endl;

		}
	}
	
};

int main() 
{
	Observable logger;


	WarningObserver warningObs;
	errorObserver errorObs("error.log");
	FatalErrorObserver fatalErrorObs("fatal.log");

	logger.addObserver(&warningObs);
	logger.addObserver(&errorObs);
	logger.addObserver(&fatalErrorObs);

	logger.warning("This a warning message.");
	logger.error("This is an error message.");
	logger.fatallError("This is an Fatalerror message.");

	logger.removeObserver(&errorObs);

	return 0;
}