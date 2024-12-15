#include "Bot.hpp"

/* PARAMETRIZED CONSTRUCTOR */
Player::Player(std::string name): name(name), currentQuestion(0), score(0) {}

/* DESTRUCTOR */
Player::~Player() {}

/* METHODS */
void Player::addAnswerQuestion(int questionId) { answerQuestions.push_back(questionId); }

/* GETTERS */
const std::string& Player::getName() const { return name; }
int Player::getCurrentQuestion() const { return currentQuestion; }
int Player::getScore() const { return score; }
std::vector<int> Player::getAnswerQuestions() const {return answerQuestions;}

/* SETTERS */
void Player::setName(const std::string& name) { this->name = name; }
void Player::setCurrentQuestion(const int currentQuestion) { this->currentQuestion = currentQuestion; }
void Player::setScore(const int score) { this->score = score; }