#ifndef TRIVIALBOT_HPP
# define TRIVIALBOT_HPP 

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

struct Question
{
    int         id;
    std::string text;
    std::vector<std::string> options;
    int correctAnswer;
};

class TrivialBot
{
private:
    std::vector<Question>   questions;
    std::string             questionsFile;
    Message                 message;
    std::vector<Question>   loadQuestionsFromFile(const std::string filename);
    Question                getRandomQuestion(const std::vector<int>& answeredQuestions);
    
public:
    /* PARAMETRIZED CONSTRUCTOR */
    TrivialBot(std::string questionsFile);

    /* DESTRUCTOR*/
    ~TrivialBot();

    /* METHODS */
    void start(Client& client);
    void evaluateAnswer(Client& client, const std::string& answer);
    void help(Client& client);
    void quit(Client& client);
    void askQuestion(Client& client);
};

#endif // TRIVIALBOT_HPP