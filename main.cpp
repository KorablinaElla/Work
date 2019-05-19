#include "toguess.h"
#include <iostream>
#include <string>
#include <Windows.h>

using namespace std;
int main() {
	setlocale(LC_ALL, "Rus");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	/*->1.вопрос пользователю
		2.обработка ответов, пересчет вероятностей для всех ответов
		3.проверка возможности дать ответ ( если возможности нет, идём к 1)
		4.выдача ответа ожидание подтвеждения пользователем,если верный, то идем к 6
		5.Если ответ неверный, то добавляем новый ответ и персонажа в базу
		6.Конец игры
	*/
	//создаём экземпляр нашей игры
	assumption tg = assumption();
	cout << "Соотношение цифр и ответов:" << endl;
	cout << "1-да" << endl;
	cout << "0-нет" << endl;
	cout << "2-скорее да" << endl;
	cout << "3-скорее нет" << endl;
	//начинаем новую игру и подгружаем базу
	tg.newgame();
	//начинаем задавать вопросы
	cout << endl;
	cout << "Игра началась!" << endl;
	cout << endl;
	int q = tg.getQuestion();
	tg.dellQuestion(q);
	int p_ans, ans;
	cin >> p_ans;
	tg.setQuestion(q, p_ans);
	ans = tg.getResponse();
	while (ans == 0)
	{
		q = tg.getQuestion();
		cin >> p_ans;
		tg.setQuestion(q, p_ans);
		ans = tg.getResponse();
		tg.dellQuestion(q);
	}
	//Если ответ угада верно игра завершается
	cout << endl;
	cout << "Соотношение цифр и ответов:" << endl;
	cout << "1-да" << endl;
	cout << "0-нет" << endl;
	cout << endl;
	cout << "Ваш персонаж - " << tg.getfResponse(ans) << " ?" << endl;
	cin >> p_ans;
	//Если нет, то добавляем персонажа в базу
	if (!ans) {
		char* response = new char [2024];
		cout << "Введите загаданного вами персонажа:" << endl;
		cin >> response;
		cout<< response <<endl;
		tg.newResponse(response);
		tg.newQuestion(response);
		tg.save();
	}
	system("pause");
	return 0;

}
