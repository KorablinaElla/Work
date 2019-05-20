#pragma once
#include <vector>
#include "toguess.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>

using namespace std;

class assumption
{
private:
	// структура вопросов: вопрос и его id
	struct s_question{
		string question;
		int id;
	};
	// структура ответов: ответ, спиок соответсвующих ему вопрсов, вероятность ответа
	struct s_response{
		string response;
		vector <int> idQuest;
		double probability;
	};
	// База для всей игры
	vector <s_question> questions;
	vector <s_response> responses;
	//База для текущей игры
	vector <int> donequest;
	vector <s_question> Tquestions;
	vector <s_response> Tresponses;
	//ищет максимальный id
	const int maxid(const vector <s_question> &_questions);
	//смотрит, есть ли вопросы , на которые пользователь не ответил, если есть, возвращает true
	const bool havequestions();
	//возвращает id наиболее популярного вопроса
	const int populatQuestion();
    public:
	int step;
	//консруктор, в котором происходит считывание базы
	assumption();
	//сохранение изменённой базы
	const void save();
	//создание новой игры: сброс вектора положительных ответов,
	//загрузка скорректированной базы
	void newgame();
	//генерация нового вопроса
	int getQuestion();
	//корректируются вероятности ответов в соответсвии с ответом на вопрос с id
	void setQuestion(int id, int q);
	//попытка выдать ответ
	const int getResponse(int id = 0);
	//возвращает ответ с данным id в вектор
	const string getfResponse(int id);
	//добавление в базу нового персонажа
	void newResponse(string response);
	//добавление вопроса, который отличает нового персонажа
	void newQuestion(string q);
	//удаляет вопрос из вектора ответов персонажей
	void dellQuestion(int id);
};

