#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include "toguess.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>

using namespace std;

assumption::assumption()
{ 
	FILE *file = fopen("questions.txt", "r");
	string q;
	int id;
	// База вопросов
	if (!file) {
		cerr << "File error." << endl;
		return ;
	}
	while (!feof(file))
	{
		char word[1025];
		int id;
		fscanf(file, "/%s /%i ", &word, &id);
		string question = string(word);
		replace(question.begin(), question.end(), '_', ' ');
		s_question Q;
		Q.id = id;
		Q.question = question;
		questions.push_back(Q);
	}
	fclose(file);
	// База ответов (персонажей)
	file = fopen("responses.txt", "r");
	while (!feof(file)) {
		char resp[1025];
		int countq;
		float pro;
		fscanf(file, "%s /%i", &resp, &countq);
		string s=string(resp);
		replace(s.begin(), s.end(), '_', ' ');
		vector <int> quests;
		for (int i = 1; i <= countq; i++)
		{
			int temp;
			fscanf(file, "%i ", &temp);
			quests.push_back(temp);
		}
		fscanf(file, "%f/ ", &pro);
		s_response thisResponse;
		thisResponse.idQuest = quests;
		thisResponse.probability = pro;
		thisResponse.response = s;
		responses.push_back(thisResponse);
	}
	fclose(file);
}

void assumption::newgame() {
	// Старт новой игры
	donequest.clear();
	Tquestions.clear();
	Tresponses.clear();
	Tresponses = responses;
	Tquestions = questions;
	for (auto && t : Tresponses)
	 reverse(t.idQuest.begin(), t.idQuest.end());
	//текущий ход (вопрос)
	step = 1;
}

int assumption::getQuestion() {
	// новый вопрос
	int id;
	while (size_t(Tresponses[size_t(Tresponses.size()) - 1].idQuest.size()) > 0
		&& Tresponses[size_t(Tresponses.size()) - 1].idQuest[size_t(Tresponses[size_t(Tresponses.size()) - 1].idQuest.size()) - 1] == -1)
		Tresponses[size_t(Tresponses.size()) - 1].idQuest.pop_back();
	while (size_t(Tresponses[size_t(Tresponses.size()) - 2].idQuest.size()) > 0
		&& Tresponses[size_t(Tresponses.size()) - 2].idQuest[size_t(Tresponses[size_t(Tresponses.size()) - 2].idQuest.size()) - 1] == -1)
		Tresponses[size_t(Tresponses.size()) - 2].idQuest.pop_back();
	if (step > 1)
	{
		if (size_t(Tresponses[size_t(Tresponses.size()) - 1].idQuest.size()) == 0)
		{
			if (size_t(Tresponses[size_t(Tresponses.size()) - 2].idQuest.size()) != 0)
				id = Tresponses[size_t(Tresponses.size()) - 2].idQuest[size_t(Tresponses[size_t(Tresponses.size()) - 2].idQuest.size()) - 1];

			else
			{ // если о обоих на данный момент самых вероятных ответов больше не осталось вопросов, 
				//и их вероятности раны, лбио почти одинаковы,
			// то понимажем их вероятность и переходи к другим вопросам.
				double eps = 0.01;
				if (Tresponses[size_t(Tresponses.size()) - 1].probability - Tresponses[size_t(Tresponses.size()) - 2].probability < eps
					&& havequestions())
				{
					Tresponses[size_t(Tresponses.size()) - 1].probability /= 1.5;
					Tresponses[size_t(Tresponses.size()) - 2].probability /= 1.5;
					for (size_t i = size_t(Tresponses.size()) - 1; i > 0; i--)
					  for (size_t j = 0; j < i; j++)
						  if (Tresponses[j].probability > Tresponses[j + 1].probability)
								swap(Tresponses[j], Tresponses[j + 1]);
					return getQuestion();
				}
				return -1; // система нашла ответ
			}
		}
		else
		{
			id = Tresponses[Tresponses.size() - 1].idQuest[Tresponses[Tresponses.size() - 1].idQuest.size() - 1];
		}
	}
	else // первый вопрос
		id = populatQuestion();
	step++;
	//перешли к следующему вопросу
	for (auto && t : Tquestions)
		if (t.id == id)
		{
			cout << t.question << endl;
			return id;
		}
}

void assumption::setQuestion(int id, int q) // корректируем текущую базу в соответсвии с овтетом на вопрос
{
	if (q == 1 )
	{
		// У всех вариантов ответа, у которых есть даннный вопрос, повышаем вероятность
		int k = 0; int ii = 0,i=0;
		for (auto && t : Tresponses)
		{
			auto it = find(t.idQuest.begin(), t.idQuest.end(), id);
			if (it != t.idQuest.end())
			{
				k++;
				ii = i;
				t.probability *= 4;
				for (auto && tid : t.idQuest)
				  if (tid == id) tid = -1;
			}
			else
			 // если вопроса нет, понижаем вероятность
				t.probability /= 1.5;	
			i++;
		}
		if (k == 1) Tresponses[ii].probability += 0.2; 
		// если персонаж с даным ответом тоько один, то его вероятность резко повышается
		// сортируем все ответы в соответствии их вероятности
		for (size_t i = size_t(Tresponses.size()) - 1; i > 0; i--)
		for (size_t j = 0; j < i; j++)
			  if (Tresponses[j].probability > Tresponses[j + 1].probability)
					swap(Tresponses[j], Tresponses[j + 1]);
		donequest.push_back(id);
	}
	else if (!q)
	{ // ЕСЛИ ОТВЕТ ЛОЖЬ
	// У всех вариантов ответа, у которых есть даннный вопрос, понижаем вероятность
		for (auto && t : Tresponses)
		//for (int i = 0; i < Tresponses.size(); i++)
		{
			auto it = find(t.idQuest.begin(), t.idQuest.end(), id);
			if (it!= t.idQuest.end())
			{
				t.probability /= 4;
				for (auto && tid : t.idQuest)
				  if (tid == id) tid = -1;
			}
			else
			{
				// а у кого нет, повышаем
				t.probability += 0.05;
			}
		}
		// сортируем все ответы в соответствии их вероятности
		for (size_t i = size_t(Tresponses.size()) - 1; i > 0; i--)
		{
			for (size_t j = 0; j < i; j++)
			{
				if (Tresponses[j].probability > Tresponses[j + 1].probability)
					swap(Tresponses[j], Tresponses[j + 1]);
			}
		}
	}
	else if (q == 2)
	{ // Ответ Скорее да
		int k = 0; int ii = 0,i=0;
		for (auto && t : Tresponses)
		{
			auto it = find(t.idQuest.begin(), t.idQuest.end(), id);
			if (it!= t.idQuest.end())
			{
				k++;
				ii = i;
				t.probability *= 3;
				for (auto && tid : t.idQuest)
				  if (tid == id) tid = -1;
			}
			else
			 // если вопроса нет, понижаем вероятность
				t.probability -= 0.01;
			i++;
		}
		if (k == 1) Tresponses[ii].probability += 0.15; 
		// если персонаж с даным ответом тоько один, то его вероятность резко повышается
		// сортируем все ответы в соответствии их вероятности
		for (size_t i = size_t(Tresponses.size()) - 1; i > 0; i--)
		  for (size_t j = 0; j < i; j++)
			  if (Tresponses[j].probability > Tresponses[j + 1].probability)
					swap(Tresponses[j], Tresponses[j + 1]);
		donequest.push_back(id);
	}
	else if (q == 3)
	{
		for (auto && t : Tresponses)
		{
			auto it = find(t.idQuest.begin(), t.idQuest.end(), id);
			if (it!= t.idQuest.end())
			{
				t.probability -= 0.15;
				if (t.probability < 0)
				{
					t.probability *= 0.001;
					t.probability = abs(t.probability);
				}
				for (auto && tid : t.idQuest)
				  if (tid == id) tid = -1;
			}
			else
			  // а у кого нет, повышаем
				t.probability += 0.045;
		}
		// сортируем все ответы в соответствии их вероятности
		for (size_t i = size_t(Tresponses.size())- 1; i > 0; i--)
		 for (size_t j = 0; j < i; j++)
			 if (Tresponses[j].probability > Tresponses[j + 1].probability)
					swap(Tresponses[j], Tresponses[j + 1]);
	}
}

//ищем ответ
const int assumption::getResponse(int id)
{
	const double eps = 0.05;
	// попытка получить ответ
	// Ответ есть, если разница между вероятностью двух, на данный
	// момент самых подходящих, ответов больше какой-то константы
	if (Tresponses[Tresponses.size() - 1].probability - Tresponses[Tresponses.size() - 2].probability >= eps || id < 0)
	{ // ответ есть
		return Tresponses.size() - 1;
	}
	//ответа нет
	else return 0;
}

//печатаем ответ
const string assumption::getfResponse(int id) { return Tresponses[id].response; }

void assumption::newResponse(string response)
{
	//необходимо проверить, что ответ не вводят повторно
	for (auto && r : responses)
	   if (r.response == response)  return;
	// Добавление новго ответа в базу;
	s_response thisResponse;
	thisResponse.idQuest = donequest;
	thisResponse.probability = 0.001;
	thisResponse.response = response;
	responses.push_back(thisResponse);
}

const void assumption::save()
{
	// сохранение базы
	FILE* file = fopen("responses.txt", "w");
	if (!file){
		cout << "Error opening file!" << endl;
		return;
	}
	for (auto && r : responses)
	{
		fprintf(file, "%s /%i ", r.response, size_t(r.idQuest.size()));
		for (auto && rid : r.idQuest)
		   fprintf(file, "%i ", rid);
		fprintf(file, "%f/ \n", r.probability);
	}
	fclose(file);

	ofstream file1("questions. txt");
	if (file1.fail())
	{
		cout << "Error opening file!" << endl;
		return;
	}
	for (auto && q : questions)
	 file1 << "/" << q.question << " /" << q.id << " " << endl;
}

void assumption::newQuestion(string q)
{// q-объект, который был загад, но его не оказалось в базе
	cout << "Введите вопрос, который позволит отличить " <<
		q << " от " << Tresponses[Tresponses.size() - 1].response << endl;
	string ques;
	getline(cin, ques);
	s_question thisQuestion;
	thisQuestion.id = maxid(questions) + 1;
	thisQuestion.question = ques;
	for (auto && r : responses)
	{
		if ( r.response == q)
			r.idQuest.push_back(thisQuestion.id);
	}
	questions.push_back(thisQuestion);
}

const int assumption::maxid(const vector <s_question> &_questions)
{
		int max = 0;
		for_each(_questions.begin(), _questions.end(),
			[&max](s_question a) {if (a.id > max) max = a.id; });
		return max;
}

void assumption::dellQuestion(int id)
{
	    for (auto && t : Tresponses){
			auto it = find(t.idQuest.begin(), t.idQuest.end(), id);
		  if (it != t.idQuest.end()) {
			for (auto && tid : t.idQuest)
			  if (tid == id) tid = -1;
		  }
	    }
}

const bool assumption::havequestions()
{
	    auto it = find_if(Tresponses.begin(), Tresponses.end(),
		[](s_response a) {for (auto && j : a.idQuest)
		if (j != '-1') return true;
	    return false; });
	return it != Tresponses.end();
}

const int assumption::populatQuestion()
{
		int maxCount = 0,mid = 0;
		for (auto && _id : Tresponses)
		for (size_t _id = 1; _id <= questions[size_t(questions.size()) - 1].id; _id++) // для каждого вопроса
		{
		 int thisCount = 0;
		 for_each(Tresponses.begin(), Tresponses.end(),
			 [_id, &thisCount, &maxCount, &mid](s_response a) {
			 for (auto && j : a.idQuest)
			 {
			  if (j == _id) thisCount++;
			   if (thisCount > maxCount)
			   {
				 maxCount = thisCount;
				 mid = _id;
			   }
		     }});
			
		}
		return mid;
}
