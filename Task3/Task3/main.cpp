#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>


enum class MessageType {
    Warning,
    Error,
    FatalError,
    Unknown
};


class LogMessage {
public:
    LogMessage(MessageType type, const std::string& message)
        : type_(type), message_(message) {
    }

    MessageType type() const { return type_; }
    const std::string& message() const { return message_; }

private:
    MessageType type_;
    std::string message_;
};


class LogHandler {
public:
    virtual ~LogHandler() = default;

    void receiveMessage(const LogMessage& msg) {
       
        if (canHandle() == msg.type()) {
            handleMessage(msg);
        }
       
        else if (next_) {
            next_->receiveMessage(msg);
        }
        
        else {
            throw std::runtime_error("Error: no handler for this message was found!");
        }
    }

    
    void setNextHandler(LogHandler* next) { next_ = next; }

protected:
    virtual void handleMessage(const LogMessage& msg) = 0; 
    virtual MessageType canHandle() const = 0;            

private:
    LogHandler* next_ = nullptr; 
};


class WarningHandler : public LogHandler {
protected:
    void handleMessage(const LogMessage& msg) override {
        std::cout << "Warning: " << msg.message() << std::endl;
    }

    MessageType canHandle() const override {
        return MessageType::Warning;
    }
};


class ErrorHandler : public LogHandler {
public:
    ErrorHandler(const std::string& filePath) : filePath_(filePath) {}

protected:
    void handleMessage(const LogMessage& msg) override {
        std::ofstream outFile(filePath_, std::ios::app);
        if (!outFile) {
            throw std::runtime_error("Failed to open log file: " + filePath_);
        }
        outFile << "Error: " << msg.message() << std::endl;
    }

    MessageType canHandle() const override {
        return MessageType::Error;
    }

private:
    std::string filePath_;
};

class FatalErrorHandler : public LogHandler {
protected:
    void handleMessage(const LogMessage& msg) override {
        throw std::runtime_error("Fatal error: " + msg.message());
    }

    MessageType canHandle() const override {
        return MessageType::FatalError;
    }
};

int main() {
    try {
        
        WarningHandler warningHandler;
        ErrorHandler errorHandler("errors.log");
        FatalErrorHandler fatalErrorHandler;

        
        warningHandler.setNextHandler(&errorHandler);
        errorHandler.setNextHandler(&fatalErrorHandler);

        
        LogMessage warningMessage(MessageType::Warning, "This is a warning.");
        LogMessage errorMessage(MessageType::Error, "This is an error.");
        LogMessage fatalMessage(MessageType::FatalError, "This is a fatal error.");
        LogMessage unknownMessage(MessageType::Unknown, "This is an unknown message.");

        
        warningHandler.receiveMessage(warningMessage); 
        warningHandler.receiveMessage(errorMessage);   
        warningHandler.receiveMessage(fatalMessage);   
        warningHandler.receiveMessage(unknownMessage); 
    }
    catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
    }

    return 0;
}