#include "IRCServer.hpp"

void IRCServer::trivial(std::string& command, Client& client)
{
    logger.info("[TRIVAL]:: User " + client.getNickname() + " wants to interact with trivial...");

    // 1. Split command
    std::vector<std::string> split_command = Utils::splitBySpaces(command);
    int command_len = split_command.size();
    
    if (command_len < 2) {
        trivialBot.help(client);
        return;
    }

    // 2. Handle commands using a "switch"
    Command cmd = commandToInt(Utils::toUpper(split_command[1]));
    switch (cmd) {
        case START:
            trivialBot.start(client);
            break;

        case ANSWER:
            if (!client.isPlaying())
            {
                message.sendToClient(client.getFd(), BOT_ERR_NOTPLAYING(client.getNickname()));
                return;
            }
            else if (split_command.size() < 3 && client.isPlaying()) {
                message.sendToClient(client.getFd(), BOT_ERR_NOANSWERGIVEN(client.getNickname()));
            }
            else
                trivialBot.evaluateAnswer(client, split_command[2]);
            break;

        case T_QUIT:
            trivialBot.quit(client);
            break;

        case HELP:
        case UNKNOWN:
        default:
            trivialBot.help(client);
            break;
    }
}

/* PARAMETRIZED CONSTRUCTOR */
TrivialBot::TrivialBot(std::string questionsFile) : questionsFile(questionsFile)
{
    questions = loadQuestionsFromFile(questionsFile);
}

/* DESTRUCTOR */
TrivialBot::~TrivialBot() {}

/* METHODS */
void TrivialBot::start(Client& client)
{
    // 1. Check if client is playing
    if (client.isPlaying())
    {
        message.sendToClient(client.getFd(), BOT_ERR_ALREDYPLAYING(client.getNickname()));
        return;
    }

    // 2. Init game
    client.setPlaying(true);
    client.setCurrentQuestion(0);
    client.setScore(0);

    // 4. Send welcome message
    message.sendToClient(client.getFd(), BOT_WELCOME(client.getNickname()));

    // 5. Ask first question
    askQuestion(client);
}

void TrivialBot::evaluateAnswer(Client& client, const std::string& answer)
{
    // 1. Client no playing
    if (!client.isPlaying()) {
        message.sendToClient(client.getFd(), BOT_ERR_NOTPLAYING(client.getNickname()));
        return;
    }

    int answerIndex = atoi(answer.c_str()) - 1;
    int currentIndex = client.getCurrentQuestion();
    const Question& currentQuestion = questions[currentIndex];
    
    // 2. Check answer
    if (answerIndex == currentQuestion.correctAnswer)
    {
        client.setScore(client.getScore() + 1);
        message.sendToClient(client.getFd(), BOT_CORRECTANSWER(client.getNickname(), client.getScore()));
    }
    else
        message.sendToClient(client.getFd(), BOT_INCORRECTANSWER(client.getNickname(), currentQuestion.options[currentQuestion.correctAnswer]));

    // 3. Next question
    int answeredQuestions = client.getAnswerQuestions().size();
    if (answeredQuestions < NUM_QUESTIONS && answeredQuestions < (int) questions.size())
    {
        client.setCurrentQuestion(currentIndex + 1);
        askQuestion(client);
    } else {
        message.sendToClient(client.getFd(), BOT_ENDGAME(client.getNickname(), client.getScore()));
        client.setPlaying(false);
    }
}

void TrivialBot::askQuestion(Client& client)
{
    // 1. Questions not empty
    if (!questions.empty())
    {   
        // 2. Get random question
        Question question = getRandomQuestion(client.getAnswerQuestions());
        
        std::string msg = "Question: " + question.text + "\n";
        for (size_t i = 0; i < question.options.size(); ++i) {
            msg += Utils::intToString(i + 1) + ". " + question.options[i] + "\n";
        }
        message.sendToClient(client.getFd(), msg);
        
        // 3. Set question as answered
        client.setCurrentQuestion(question.id);
        client.addAnswerQuestion(question.id);

        
    } else {
        message.sendToClient(client.getFd(), BOT_ERR_QUESTIONSNOTFOUND(client.getNickname()));
        client.setPlaying(false);
    }
}

void TrivialBot::quit(Client& client)
{
    if (!client.isPlaying()) {
        message.sendToClient(client.getFd(), BOT_ERR_NOTPLAYING(client.getNickname()));
        return;
    }

    client.setPlaying(false);
    message.sendToClient(client.getFd(), BOT_QUITGAME(client.getNickname()));
}

void TrivialBot::help(Client& client)
{
    std::string helpMessage = 
        "Available commands for Trivial:\n"
        "!trivial start - Start a new game.\n"
        "!trivial answer X - Answer with option X (1-4).\n"
        "!trivial quit - Exits the current game.\n"
        "!trivial help - Displays this help message\n";
    message.sendToClient(client.getFd(), helpMessage);
}

Question TrivialBot::getRandomQuestion(const std::vector<int>& answeredQuestions)
{
    if (questions.empty()) {
        std::cout << "No hay preguntas disponibles." << std::endl;
    }

    static bool seeded = false;
    if (!seeded) {
        std::srand(std::time(0)); // Seed based on actual time
        seeded = true;
    }

    // Calculate how many questions have not been answered yet
    int remainingQuestions = questions.size() - answeredQuestions.size();
    if (remainingQuestions == 0) {
        std::cout << "Todas las preguntas han sido respondidas." << std::endl;
    }

    // Random NOT answered questions
    int randomIndex;
    do {
        randomIndex = std::rand() % questions.size();
    } while (std::find(answeredQuestions.begin(), answeredQuestions.end(), randomIndex) != answeredQuestions.end());

    return questions[randomIndex];
}

std::vector<Question> TrivialBot::loadQuestionsFromFile(const std::string filename) {
    std::ifstream file(filename.c_str());

    if (!file.is_open()) {
        //logger.error("[TRIVIAL]:: No se pudo abrir el archivo de preguntas.");
        return questions;
    }

    std::string line;
    int id = 0;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        Question q;

        // Leer pregunta
        std::getline(ss, q.text, '|');

        // Leer opciones
        std::string option;
        
        while (std::getline(ss, option, '|') && q.options.size() < 4) {
            q.options.push_back(option);
        }
    
        q.correctAnswer = atoi(option.c_str()) - 1;

        // Set id
        q.id = id;

        questions.push_back(q);
        id++;
    }

    file.close();
    return questions;
}

