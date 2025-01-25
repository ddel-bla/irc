#include "Bot.hpp"

/* MAIN */
int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        std::cerr << "Usage: ./trivialBot <serverIp> <port> <password>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string ip = argv[1];
    int port = std::atoi(argv[2]);
    std::string password = argv[3];

    try {
        Bot bot(ip, port, password);
        bot.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}

/* PARAMETRIZED CONSTRUCTOR */
Bot::Bot(const std::string& serverIp, int serverPort, std::string serverPassword) : name(NICK), serverPassword(serverPassword), logger("bot.log", false), questionsFile(F_QUESTIONS)
{
    // 1. Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        throw std::runtime_error("Failed to connect to the IRC server.");
    }

    // 2. Config server ip
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr) <= 0) {
        throw std::runtime_error("Invalid IP address.");
    }

    // 3. Connect to server
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        throw std::runtime_error("Failed to create the socket.");
    }
    
    logger.info("Connection established with IRC Server: " + serverIp + ":" + Utils::intToString(serverPort));

    // 4. Load questions
    questions = loadQuestionsFromFile(questionsFile);
    commandMap = createCommandMap();
}

/* DESTRUCTOR */
Bot::~Bot()
{
    close(sock);
}

/* METHODS */
void Bot::run(void)
{
    // 1. Authenticate 
    sendMessage("PASS " + this->serverPassword);
    sendMessage("NICK " + this->name);
    sendMessage("USER " + this->name + " 0 * :" + this->name);
    logger.info("Authenticated!");

    // 2. Read server messages
    while (true)
    {
        std::string serverMessage = receiveMessage();
        std::cout << serverMessage << std::endl;
        logger.info("Server sends: " + serverMessage);

        // Joins channel if invited
        joinChannel(serverMessage);

        // Reply privmsg if needed
        replyPrivmsg(serverMessage);
    }
}
void Bot::joinChannel(const std::string& serverMessage)
{
    if (serverMessage.find(INVITE_MSG(this->name)) != std::string::npos)
    {
        std::string channel = extractKeyword(INVITE_MSG(this->name), serverMessage);
        sendMessage("JOIN " + channel);
    }
}

void Bot::replyPrivmsg(const std::string& serverMessage)
{
    if (serverMessage.find(" PRIVMSG ") != std::string::npos)
    {
        std::string complete_cmd = extractKeyword("PRIVMSG", serverMessage);
        
        // 1. Split command 
        std::vector<std::string> split_command = Utils::splitBySpaces(complete_cmd);
        
        if (split_command.size() >= 2)
        {
            std::string sender = split_command[0];
            std::string cmd = Utils::getMessageWithoutPrefixes(complete_cmd, 1);
            cmd = Utils::removeLeadingChar(cmd, ':');

            // 2. Sender is a person
            if (sender[0] != '#')
                sender = extractUsername(serverMessage);

            // 3. Hanndle command
            if (!sender.empty())
                hanndleCommand(sender, cmd);
        }
        else
            std::cout << "Error" << std::endl;
    }
}

void Bot::hanndleCommand(const std::string& sender, const std::string& command)
{
    logger.info("Handling command...");

    // 1. Split command
    std::vector<std::string> split_command = Utils::splitBySpaces(command);
    int command_len = split_command.size();
    
    // 2. Check that !trivial cmd is present
    if ((command_len > 0 && split_command[0] != "!trivial"))
        return;
    else if (command_len < 2 && (command_len > 0 && split_command[0] == "!trivial")) {
        help(sender);
        return;
    }

    // 3. Handle commands using a "switch"
    Command cmd = commandToInt(Utils::toUpper(split_command[1]));
    switch (cmd) {
        case START:
            std::cout << "Enters" << command << std::endl;
            start(sender);
            break;

        case ANSWER:
            if (!isPlaying(sender))
            {
                sendMessage("PRIVMSG " + sender + BOT_ERR_NOTPLAYING);
                return;
            }
            else if (split_command.size() < 3 && isPlaying(sender))
                sendMessage("PRIVMSG " + sender + BOT_ERR_NOANSWERGIVEN);
            else
                evaluateAnswer(sender, split_command[2]);
            break;

        case T_QUIT:
            quit(sender);
            break;

        case HELP:
        case UNKNOWN:
        default:
            help(sender);
            break;
    }
}

void Bot::start(const std::string& sender)
{
    std::string msg;

    // 1. Check if client is playing
    if (isPlaying(sender))
    {
        sendMessage("PRIVMSG " + sender + BOT_ERR_ALREDYPLAYING);
        return;
    }

    // 2. Init game
    addPlayer(sender);

    // 3. Get player
    Player* player = getPlayerByName(sender);

    // 4. Send welcome message
    sendMessage("PRIVMSG " + player->getName() + BOT_WELCOME);

    // 5. Ask first question
    askQuestion(*player);
}

void Bot::evaluateAnswer(const std::string& sender, const std::string& answer)
{
    // 1. Client no playing
    if (!isPlaying(sender))
    {
        sendMessage("PRIVMSG " + sender + BOT_ERR_NOTPLAYING);
        return;
    }

    int answerIndex = atoi(answer.c_str()) - 1;
    Player* player = getPlayerByName(sender);
    int currentIndex = player->getCurrentQuestion();
    const Question& currentQuestion = questions[currentIndex];
    
    // 2. Check answer
    if (answerIndex == currentQuestion.correctAnswer)
    {
        player->setScore(player->getScore() + 1);
        sendMessage("PRIVMSG " + sender + BOT_CORRECTANSWER(player->getScore()));
    }
    else
        sendMessage("PRIVMSG " + sender + BOT_INCORRECTANSWER(currentQuestion.options[currentQuestion.correctAnswer]));

    // 3. Next question
    int answeredQuestions = player->getAnswerQuestions().size();
    if (answeredQuestions < NUM_QUESTIONS && answeredQuestions < (int) questions.size())
    {
        player->setCurrentQuestion(currentIndex + 1);
        askQuestion(*player);
    } else {
        sendMessage("PRIVMSG " + sender + BOT_ENDGAME(player->getScore()));
        removePlayer(player->getName());
    }
}

void Bot::askQuestion(Player& player)
{
    // 1. Questions not empty
    if (!questions.empty())
    {   
        // 2. Get random question
        Question question = getRandomQuestion(player.getAnswerQuestions());
        
        std::string msg = " Question: " + question.text + "\n";
            sendMessage("PRIVMSG " + player.getName() + msg);
        for (size_t i = 0; i < question.options.size(); ++i) {
            sendMessage("PRIVMSG " + player.getName() + " " + Utils::intToString(i + 1) + ". " + question.options[i]);
        }
        
        // 3. Set question as answered
        player.setCurrentQuestion(question.id);
        player.addAnswerQuestion(question.id);
        
    } else {
        sendMessage("PRIVMSG " + player.getName() + BOT_ERR_QUESTIONSNOTFOUND);
        removePlayer(player.getName());
    }
}

void Bot::quit(const std::string& sender)
{
    if (!isPlaying(sender))
    {
        sendMessage("PRIVMSG " + sender + BOT_ERR_NOTPLAYING);
        return;
    }

    removePlayer(sender);
    sendMessage("PRIVMSG " + sender + BOT_QUITGAME);
}

void Bot::help(const std::string& sender)
{
    sendMessage("PRIVMSG " + sender + " :Available commands for Trivial:");
    sendMessage("PRIVMSG " + sender + " !trivial start - Start a new game.");
    sendMessage("PRIVMSG " + sender + " !trivial answer X - Answer with option X (1-4)");
    sendMessage("PRIVMSG " + sender + " !trivial quit - Exits the current game.");
    sendMessage("PRIVMSG " + sender + " !trivial help - Displays this help message");
}

void Bot::sendMessage(const std::string& message)
{
    std::string msg = message + CRLF;
    send(sock, msg.c_str(), msg.size(), 0);
}

std::string Bot::receiveMessage(void)
{
    char buffer[512];
    memset(buffer, 0, sizeof(buffer));

    int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived == 0)
        throw std::runtime_error("El servidor cerró la conexión.");
    else if (bytesReceived < 0)
        throw std::runtime_error("Error al recibir datos del servidor.");


    return std::string(buffer);
}

Player* Bot::getPlayerByName(const std::string& playerName)
{
    for (size_t i = 0; i < players.size(); ++i) {
        if (players[i].getName() == playerName) {
            return &players[i];
        }
    }
    return NULL;
}

bool Bot::isPlaying(const std::string& playerName)
{
    for (size_t i = 0; i < players.size(); ++i) {
        if (players[i].getName() == playerName)
            return true;
    }
    return false;
}

int Bot::findPlayerIndex(const std::string& playerName)
{
    for (size_t i = 0; i < players.size(); ++i) {
        if (players[i].getName() == playerName)
            return i;
    }
    return -1; // No encontrado
}

bool Bot::addPlayer(const std::string& playerName)
{
    if (findPlayerIndex(playerName) != -1) {
        return false;
    }

    players.push_back(Player(playerName));
    return true;
}

bool Bot::removePlayer(const std::string& playerName)
{
    int index = findPlayerIndex(playerName);
    if (index == -1) {
        return false;
    }

    players.erase(players.begin() + index);
    return true;
}

Question Bot::getRandomQuestion(const std::vector<int>& answeredQuestions)
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

std::vector<Question> Bot::loadQuestionsFromFile(const std::string filename) {
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

std::string Bot::extractKeyword(const std::string& keyword, const std::string& message)
{
    size_t pos = message.find(keyword);
    if (pos != std::string::npos)
        return message.substr(pos + keyword.length());
    return "";
}

std::string Bot::extractUsername(const std::string& message)
{
    std::vector<std::string> split_command = Utils::splitBySpaces(message);


    std::cout << "Extracted command " << message << std::endl;
    if (split_command.size() > 2)
    {
        std::string sender = split_command[1];
        std::cout << "Sender obtained " << sender << std::endl;
        
        std::vector<std::string> sender_split = split_command = Utils::split(sender, "!");

        if (sender_split.size() > 1)
            sender = Utils::removeLeadingChar(sender_split[0], ':');
        else
            std::cout << "This should not happen! .-." << std::endl;

        return sender;
    }
    return NULL;
}

std::map<std::string, Command> Bot::createCommandMap()
{
	commandMap["!TRIVIAL"] = TRIVIAL;
    commandMap["START"] = START;
    commandMap["ANSWER"] = ANSWER;
    commandMap["QUIT"] = T_QUIT;
    commandMap["HELP"] = HELP;
    return commandMap;
}

Command Bot::commandToInt(const std::string& command)
{
    std::map<std::string, Command>::const_iterator it = commandMap.find(command);
    if (it != commandMap.end()) {
        return it->second;
    }
    return UNKNOWN;
}