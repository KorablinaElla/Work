#pragma once
#include <vector>
class toguess
{
private:
	// структура вопросов: вопрос и его id
	struct s_question{
		char* question;
		int id;
	};
	// структура ответов: ответ, спиок соответсвующих ему вопрсов, вероятность ответа
	struct s_response{
		char* response;
		std::vector <int> idQuest;
		double probability;
	};
	// База для всей игры
	std::vector <s_question> questions;
	std::vector <s_response> responses;
	//База для текущей игры
	std::vector <int> donequest;
	std::vector <s_question> Tquestions;
	std::vector <s_response> Tresponses;
	//int step;
	//ищет id в векторе, если находит, возвращает true
	bool findInVector(std::vector <int> v, int id);
	bool findInVector(std::vector <s_question> v, int id);
	//ищет максимальный id
	int maxid(std::vector <s_question> _questions);
	//заменяет нижнее подчёркивание на пробелы
	char* addspaces(char* str);
	//смотрит, есть ли вопросы , на которые пользователь не ответил, если есть, возвращает true
	bool havequestions();
	//возвращает id наиболее популярного вопроса
	int populatQuestion();
public:
	int step;
	//консруктор, в котором происходит считывание базы
	toguess();
	//сохранение изменённой базы
	void save();
	//создание новой игры: сброс вектора положительных ответов,
	//загрузка скорректированной базы
	void newgame();
	//генерация нового вопроса
	int getQuestion();
	//корректируются вероятности ответов в соответсвии с ответом на вопрос с id
	void setQuestion(int id, int q);
	//попытка выдать ответ
	int getResponse(int id = 0);
	//возвращает ответ с данным id в вектор
	char* getfResponse(int id);
	//добавление в базу нового персонажа
	void newResponse(char* response);
	//добавление вопроса, который отличает нового персонажа
	void newQuestion(char* q);
	//удаляет вопрос из вектора ответов персонажей
	void dellQuestion(int id);
};

