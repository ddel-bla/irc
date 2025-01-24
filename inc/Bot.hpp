#ifndef BOT_HPP
# define BOT_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <csignal>
#include <map>
#include <algorithm>
#include "../inc/Utils.hpp"
#include "../inc/Logger.hpp"
#include "../inc/Macros.hpp"

enum Command {
	TRIVIAL,
    START,
    ANSWER,
    T_QUIT,
    HELP,
    UNKNOWN 
};

struct Question
{
    int         id;
    std::string text;
    std::vector<std::string> options;
    int correctAnswer;
};

class Player {
private:
        std::string name;
        int         currentQuestion;
        int         score;
        std::vector<int>	answerQuestions;
public:
    /* PARAMETRIZED CONSTRUCTOR */
    Player(std::string name);

    /* DESTRUCTOR */
    ~Player();

    /* METHODS */
    void	addAnswerQuestion(int questionId);

    /* GETTERS */
    const std::string&  getName() const;
    int                 getCurrentQuestion() const;
    int                 getScore() const;
    std::vector<int>    getAnswerQuestions() const;

    /* SETTERS */
    void setName(const std::string& name);
    void setCurrentQuestion(const int currentQuestion);
	void setScore(const int score);
};

class Bot {

#define NICK "T-Bot"
#define INVITE_MSG(nick) " INVITE " + nick
#define PRIVMSG_MSG(nick) " PRIVMSG " + nick

private:
    std::string         name;               // Bot name
    int                 sock;               // Socket
    struct sockaddr_in  serverAddr;         // Server addr
    std::string         serverPassword;     // Server password
    Logger              logger;             // Logging class
    std::vector<Question>           questions;      // Questions
    std::string                     questionsFile;  // Question file
    std::vector<Player>             players;        // Players
    std::map<std::string, Command> 	commandMap; 	// Switch cmd map

    /* GENERAL METHODS */
    void            sendMessage(const std::string& message);
    std::string     receiveMessage(void);
    void            joinChannel(const std::string& serverMessage);
    void            replyPrivmsg(const std::string& serverMessage);

    /* CMD METHODS */
    void    hanndleCommand(const std::string& sender, const std::string& command);
    void    start(const std::string& player);
    void    evaluateAnswer(const std::string& sender, const std::string& answer);
    void    help(const std::string& sender);
    void    quit(const std::string& sender);
    void    askQuestion(Player& player);
    
    Player*                 getPlayerByName(const std::string& playerName);
    int                     findPlayerIndex(const std::string& playerName);
    bool                    addPlayer(const std::string& playerName);
    bool                    removePlayer(const std::string& playerName);
    bool                    isPlaying(const std::string& playerName);
    std::vector<Question>   loadQuestionsFromFile(const std::string filename);
    Question                getRandomQuestion(const std::vector<int>& answeredQuestions);
    std::string             extractKeyword(const std::string& keyword, const std::string& message);
    std::string             extractUsername(const std::string& message);
    std::map<std::string, Command>  createCommandMap();
    Command                         commandToInt(const std::string& command);

public:
    /* PARAMETRIZED CONSTRUCTOR */
    Bot(const std::string& serverIp, int serverPort, std::string serverPassword);

    /* DESTRUCTOR */
    ~Bot(void);

    /* RUN METHOD */
    void run();

};
#endif // BOT_HPP 