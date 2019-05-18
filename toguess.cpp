#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include "toguess.h"
#include <iostream>
#include <algorithm>
#include <fstream>
using namespace std;
toguess::toguess()
{ // конструктор
	const char* db_q_filename = "questions.txt";
	const char* db_r_filename = "responses.txt";
	// Загружаем базу данных
	// База вопросов
	FILE *file = fopen(db_q_filename, "r");
	if (!file)
	{
		return;
	}
	while (!feof(file))
	{
		char word[1025];
		int id;
		fscanf(file, "/%s /%i ", &word, &id);
		char *question = new char[strlen(word)];
		strcpy(question, word);
		s_question Q;
		Q.id = id;
		Q.question = question;
		questions.push_back(Q);	
	}
	fclose(file);

	// База ответов (персонажей)
	file = fopen(db_r_filename, "r");
	if (!file)
	  return;
	while (!feof(file)) {
		char resp[1025];
		int countq;
		float pro;
		fscanf(file, "%s /%i", &resp, &countq);
		char *response = new char[strlen(resp)];
		strcpy(response, resp);
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
		thisResponse.response = response;
		responses.push_back(thisResponse);
	}
	fclose(file);
}
void toguess::newgame() {
	// Старт новой игры
	donequest.clear();
	Tquestions.clear();
	Tresponses.clear();
	Tresponses = responses;
	Tquestions = questions;
	for (int i = 0; i < Tresponses.size(); i++)
	{
		reverse(Tresponses[i].idQuest.begin(), Tresponses[i].idQuest.end());
	}
	//текущий ход (вопрос)
	step = 1;
}
int toguess::getQuestion() {
	// новый вопрос
	int id;
	while (Tresponses[Tresponses.size() - 1].idQuest.size() > 0 
		&& Tresponses[Tresponses.size() - 1].idQuest[Tresponses[Tresponses.size() - 1].idQuest.size() - 1] == -1)
		Tresponses[Tresponses.size() - 1].idQuest.pop_back();
	while (Tresponses[Tresponses.size() - 2].idQuest.size() > 0 
		&& Tresponses[Tresponses.size() - 2].idQuest[Tresponses[Tresponses.size() - 2].idQuest.size() - 1] == -1) 
		Tresponses[Tresponses.size() - 2].idQuest.pop_back();
    if (step > 1)
	{
      if (Tresponses[Tresponses.size() - 1].idQuest.size() == 0)
		{
		  if (Tresponses[Tresponses.size() - 2].idQuest.size()!= 0)
			id = Tresponses[Tresponses.size() - 2].idQuest[Tresponses[Tresponses.size() - 2].idQuest.size() - 1];
			
			else
			{ // если о обоих на данный момент самых вероятных ответов больше не осталось вопросов, и их вероятности раны, лбио почти одинаковы,
			// то понимажем их вероятность и переходи к другим вопросам.
				if (Tresponses[Tresponses.size() - 1].probability - Tresponses[Tresponses.size() - 2].probability < 0.01 
					&& havequestions())
				{
					Tresponses[Tresponses.size() - 1].probability /= 1.5;
					Tresponses[Tresponses.size() - 2].probability /= 1.5;
					for (int i = Tresponses.size() - 1; i > 0; i--)
					{
						for (int j = 0; j < i; j++)
						{
							if (Tresponses[j].probability > Tresponses[j + 1].probability)
								swap(Tresponses[j], Tresponses[j + 1]);
						}
					}
					return getQuestion();
				}
				return -1; // система нашла ответ
			}
		}
		else
		{
			id = Tresponses[Tresponses.size() - 1].idQuest[Tresponses[Tresponses.size() - 1].idQuest.size() - 1];
			cout << "Now 2" << endl;
		}
	}
	else // первый вопрос
	id = populatQuestion();
	step++;//перешли к следующему вопросу
	for (int i = 0; i < Tquestions.size(); i++)
	{
		if (Tquestions[i].id == id)
		{
			cout << addspaces(Tquestions[i].question) << endl;
			cout << "1-да" << endl;
			cout << "0-нет" << endl;
			cout << "3-скорее да" << endl;
			cout << "4-скорее нет" << endl;
			return id;
		}
	}

}
void toguess::setQuestion(int id, int q) // корректируем текущую базу в соответсвии с овтетом на вопрос
{
	if (q == 1)
	{
		// У всех вариантов ответа, у которых есть даннный вопрос, повышаем вероятность
		int k = 0; int ii = 0;
		for (int i = 0; i < Tresponses.size(); i++)
		{
			if (findInVector(Tresponses[i].idQuest, id))
			{
				k++;
				ii = i;
				Tresponses[i].probability *= 4;
				for (int j = 0; j < Tresponses[i].idQuest.size(); j++)
				{
					if (Tresponses[i].idQuest[j] == id) Tresponses[i].idQuest[j] = -1;
				}
			}
			else
			{
				// если вопроса нет, понижаем вероятность
				Tresponses[i].probability /= 1.5;
			}
		}
		if (k == 1) Tresponses[ii].probability += 0.2; // если персонаж с даным ответом тоько один, то его вероятность резко повышается
		// сортируем все ответы в соответствии их вероятности
		for (int i = Tresponses.size() - 1; i > 0; i--)
		{
			for (int j = 0; j < i; j++)
			{
				if (Tresponses[j].probability > Tresponses[j + 1].probability)
					swap(Tresponses[j], Tresponses[j + 1]);
			}
		}
		donequest.push_back(id);
	}
	else if (!q)
	{ // ЕСЛИ ОТВЕТ ЛОЖЬ
	// У всех вариантов ответа, у которых есть даннный вопрос, понижаем вероятность
		for (int i = 0; i < Tresponses.size(); i++)
		{
			if (findInVector(Tresponses[i].idQuest, id))
			{
				Tresponses[i].probability /= 4;
				// if (Tresponses [i]. probability<0) {Tresponses [i]. probability*=0.001; Tresponses [i]. probability=abs (Tresponses [i]. probability); }
				for (int j = 0; j < Tresponses[i].idQuest.size(); j++)
				{
					if (Tresponses[i].idQuest[j] == id) Tresponses[i].idQuest[j] = -1;
				}
			}
			else
			{
				// а у кого нет, повышаем
				Tresponses[i].probability += 0.05;
			}
		}
		// сортируем все ответы в соответствии их вероятности
		for (int i = Tresponses.size() - 1; i > 0; i--)
		{
			for (int j = 0; j < i; j++)
			{
				if (Tresponses[j].probability > Tresponses[j + 1].probability)
					swap(Tresponses[j], Tresponses[j + 1]);
			}
		}
	}
	else if (q == 3)
	{ // Ответ Скорее да
		int k = 0; int ii = 0;
		for (int i = 0; i < Tresponses.size(); i++)
		{
			if (findInVector(Tresponses[i].idQuest, id))
			{
				k++;
				ii = i;
				Tresponses[i].probability *= 3;
				for (int j = 0; j < Tresponses[i].idQuest.size(); j++)
				{
					if (Tresponses[i].idQuest[j] == id) Tresponses[i].idQuest[j] = -1;
				}
			}
			else
			{
				// если вопроса нет, понижаем вероятность
				Tresponses[i].probability -= 0.01;
			}
		}
		if (k == 1) Tresponses[ii].probability += 0.15; // если персонаж с даным ответом тоько один, то его вероятность резко повышается
		// сортируем все ответы в соответствии их вероятности
		for (int i = Tresponses.size() - 1; i > 0; i--)
		{
			for (int j = 0; j < i; j++)
			{
				if (Tresponses[j].probability > Tresponses[j + 1].probability)
					swap(Tresponses[j], Tresponses[j + 1]);
			}
		}
		donequest.push_back(id);
	}
	else if (q == 4)
	{
		for (int i = 0; i < Tresponses.size(); i++)
		{
			if (findInVector(Tresponses[i].idQuest, id))
			{
				Tresponses[i].probability -= 0.15;
				if (Tresponses[i].probability < 0) { Tresponses[i].probability *= 0.001; Tresponses[i].probability = abs(Tresponses[i].probability); }
				for (int j = 0; j < Tresponses[i].idQuest.size(); j++)
				{
					if (Tresponses[i].idQuest[j] == id) Tresponses[i].idQuest[j] = -1;
				}
			}
			else
			{
				// а у кого нет, повышаем
				Tresponses[i].probability += 0.045;
			}
		}
		// сортируем все ответы в соответствии их вероятности
		for (int i = Tresponses.size() - 1; i > 0; i--)
		{
			for (int j = 0; j < i; j++)
			{
				if (Tresponses[j].probability > Tresponses[j + 1].probability)
					swap(Tresponses[j], Tresponses[j + 1]);
			}
		}
	}
}
bool toguess::findInVector(std::vector <int> v, int id)
{
	for (int i = 0; i < v.size(); i++)
	{
		if (v[i] == id) return true;
	}
	return false;
}
bool toguess::findInVector(std::vector <s_question> v, int id)
{
	for (int i = 0; i < v.size(); i++)
	{
		if (v[i].id == id) return true;
	}
	return false;
}
//ищем ответ
int toguess::getResponse(int id)
{
	const double eps = 0.05;
	// попытка получить ответ
	// Ответ есть, если разница между вероятностью двух, на данный
	// момент самых подходящих, ответов больше какой-то константы
	cout << "ANSWER 1=" << Tresponses[Tresponses.size() - 1].probability << endl;
	cout << "ANSWER 2=" << Tresponses[Tresponses.size() - 2].probability << endl;
	if (Tresponses[Tresponses.size() - 1].probability - Tresponses[Tresponses.size() - 2].probability >= eps || id < 0)
	{ // ответ есть
		return Tresponses.size() - 1;
	}
	//ответа нет
	else return 0;
}
//печатаем ответ
char* toguess::getfResponse(int id){ return Tresponses[id].response; }

void toguess::newResponse(char* response)
{
	//необходимо проверить, что ответ не вводят повторно
	for (int i = 0; i < responses.size(); i++)
	   if (strcmp(responses[i].response, response) == 0) return;
	// Добавление новго ответа в базу;
	s_response thisResponse;
	thisResponse.idQuest = donequest;
	thisResponse.probability = 0.001;
	thisResponse.response = response;
	responses.push_back(thisResponse);
}
void toguess::save()
{
// сохранение базы
	FILE* file = fopen("responses.txt", "w");
	if (!file)
	{
		cout << "Error!" << endl;
		return;
	}
	for (int i = 0; i < responses.size(); i++)
	{
		fprintf(file, "%s /%i ", responses[i].response, responses[i].idQuest.size());
		for (int j = 0; j < responses[i].idQuest.size(); j++) fprintf(file, "%i ", responses[i].idQuest[j]);
		fprintf(file, "%f/ \n", responses[i].probability);
	}
	fclose(file);
	ofstream file1("questions. txt");
	if (file1.fail())
	{
		cout << "Error !" << endl;
		return;
	}
	for (int i = 0; i < questions.size(); i++)
	{
		file1 << "/" << questions[i].question << " /" << questions[i].id << " " << endl;
	}
}
void toguess::newQuestion(char* q)
{// q-объект, который был загад, но его не оказалось в базе
	cout << "Введите вопрос, который позволит отличить " << 
		q << " от " << Tresponses[Tresponses.size() - 1].response << endl;
	char newQ[2024];
	cin >> newQ;
	char* Q = new char[strlen(newQ)];
	strcpy(newQ, Q);
	s_question thisQuestion;
	thisQuestion.id = maxid(questions) + 1;
	thisQuestion.question = Q;
	for (int i = 0; i < responses.size(); i++)
	{ 
		if (strcmp(responses[i].response, q) == 0)
		   responses[i].idQuest.push_back(thisQuestion.id);
	}
	questions.push_back(thisQuestion);
}
//берем масимальное id у имеющегося вопроса
int toguess::maxid(vector <s_question> _questions)
{
	int max = 0;
	for (int i = 0; i < _questions.size(); i++)
	{
		if (_questions[i].id > max) max = _questions[i].id;
	}
	return max;
}

void toguess::dellQuestion(int id)
{
	// удаление вопроса
	for (int i = 0; i < Tresponses.size(); i++)
	 if (findInVector(Tresponses[i].idQuest, id))
		  for (int j = 0; j < Tresponses[i].idQuest.size(); j++)
			  if (Tresponses[i].idQuest[j] == id) Tresponses[i].idQuest[j] = -1;
}
char* toguess::addspaces(char* str1)
{
	char* str = new char[strlen(str1)];
	strcpy(str, str1);
	for (int i = 0; i < strlen(str); i++)
	{
		if (str[i] == '_') str[i] = ' ';
	}
	return str;
}

bool toguess::havequestions()
{
	for (int i = 0; i < Tresponses.size(); i++)
	{
		for (int j = 0; j < Tresponses[i].idQuest.size(); j++)
		{
			if (Tresponses[i].idQuest[j]!= '-1') return true;
		}
	}
	return false;
}
int toguess::populatQuestion()
{
	int maxCount = 0;
	int mid = 0;
	for (int _id = 1; _id <= questions[questions.size() - 1].id; _id++) // для каждого вопроса
	{
	 int thisCount = 0;
		for (int i = 0; i < Tresponses.size(); i++)
		{
			for (int j = 0; j < Tresponses[i].idQuest.size(); j++)
			{
				if (Tresponses[i].idQuest[j] == _id) thisCount++;
				if (thisCount > maxCount)
				{
                    maxCount = thisCount;
					mid = _id;
				}
			}
		}
	}
	return mid;
}

 
