#include<iostream>
#include<fstream>
#include<stdexcept>
#include<string>

using namespace std;

enum class Type
{
	Warning,
	Error,
	FatalError,
	Unknown
};

class LogMessage 
{
private:
	Type msgType;
	string msgContent;
public:
	LogMessage(Type type,const string& message) : msgType(type), msgContent(message) {}
	Type type()const 
	{
		return msgType;
	}
	const string& message()const 
	{
		return msgContent;
	}
};
class Handler {
protected:
	Handler* nextHandler = nullptr; 

public:
	virtual ~Handler() = default;

	
	void setNext(Handler* handler) {
		nextHandler = handler;
	}


	virtual void handle(const LogMessage& message) {
		if (nextHandler) {
			nextHandler->handle(message);
		}
		else {
			
			throw std::runtime_error("No handler found for message type!");
		}
	}
};

class FatalErrorHandler : public Handler {
public:
	void handle(const LogMessage& message) override {
		if (message.type() == Type::FatalError) {
			throw std::runtime_error("Fatal error: " + message.message());
		}
		
		if (nextHandler) {
			nextHandler->handle(message);
		}
	}
};

class ErrorHandler : public Handler 
{
private:
	string filePath;
public:
	explicit ErrorHandler(const string& path) : filePath(path) {}

	void handle(const LogMessage& message)override 
	{
		if (message.type() == Type::Error) 
		{
			ofstream outfile(filePath, ios::app);
			if (outfile.is_open()) 
			{
				outfile << "Error: " << message.message() << endl;
				outfile.close();
			
			}
			else 
			{
				throw runtime_error("Cannot open file " + filePath);
			
			}
		
		}
		else if (nextHandler) {
			nextHandler->handle(message);
		}
	}
};

class WarningHandler :public Handler 
{
public:
	void handle(const LogMessage& message)override
	{
		if (message.type() == Type::Warning)
		{
			cout << "Warning: " << message.message() << endl;
		}
		else if(nextHandler)
		{
			nextHandler->handle(message);

		}
	}
};

class UnknownHandler : public Handler {
public:
	void handle(const LogMessage& message) override {
		if (message.type() == Type::Unknown) {
			throw std::runtime_error("Unknown: " + message.message());
		}

		if (nextHandler) {
			nextHandler->handle(message);
		}
	}
};

int main() 
{
	try {
		FatalErrorHandler fatalerror;
		ErrorHandler errorHandler("errors.log");
		WarningHandler warningH;
		UnknownHandler unknownH;

		warningH.setNext(&errorHandler);
		errorHandler.setNext(&fatalerror);
		fatalerror.setNext(&unknownH);

		LogMessage warningMessage(Type::Warning, "This is a Warning!");
		LogMessage errorMessage(Type::Error, "This is an Error!");
		LogMessage fatalErrorMessage(Type::FatalError, "This is a FatalError!");
		LogMessage unknownMessage(Type::Unknown, "This is an Unknown message!");

		warningH.handle(warningMessage);
		warningH.handle(errorMessage);

		warningH.handle(fatalErrorMessage);

		warningH.handle(unknownMessage);

	}
	catch (const exception& ex) 
	{
		cerr << "Exception: " << ex.what() << endl;
	
	}


	return 0;
}