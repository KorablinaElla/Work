#include "toguess.h"
#include <iostream>

using namespace std;
int main() {
	setlocale(LC_ALL, "Rus");
	/*->1.вопрос пользователю
	    2.обработка ответов, пересчет вероятностей для всех ответов
		3.проверка возможности дать ответ ( если возможности нет, идём к 1)
		4.выдача ответа ожидание подтвеждения пользователем,если верный, то идем к 6
		5.Если ответ неверный, то добавляем новый ответ и персонажа в базу
		6.Конец игры
	*/
	//создаём экземпляр нашей игры
	toguess tg = toguess();
	//начинаем новую игру и подгружаем базу
	tg.newgame();
	//начинаем задавать вопросы
	int q=tg.getQuestion();
	tg.dellQuestion(q);
	int p_ans,ans;
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
	cout << "Вы загодали " << tg.getfResponse(ans) << " ?"<<endl;
	cout << "1-да" << endl;
	cout << "0-нет" << endl;
	cin >> p_ans;
	//Если нет, то добавляем персонажа в базу
	if (!ans) {
		char* response;
		cout << "Введите загаданного вами персонажа:" << endl;
		cin >> response;
		tg.newResponse(response);
		tg.newQuestion(response);
		tg.save();
	}
	system("pause");
	return 0;

}
