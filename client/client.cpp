#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include <codecvt>

using namespace std;

int wmain()
{
	_setmode(_fileno(stdout), _O_U16TEXT);// кодировка на Unicode
	_setmode(_fileno(stdin), _O_U16TEXT);
	_setmode(_fileno(stderr), _O_U16TEXT);
	HANDLE hNamedPipe;// дескриптор
	wchar_t pipeName[] = L"\\\\.\\pipe\\demo_pipe";// имя канала

	while (!WaitNamedPipe(pipeName, 0));// возвращает 0, только если еще не существует канал (если открываем клиент, но канал еще не создан, то он будет крутиться здесь)
	Sleep(50);// ждем, и все хорошо

	// связываемся с именованным каналом
	hNamedPipe = CreateFile(
		pipeName, // имя канала
		GENERIC_ALL, // записываем в канал
		FILE_SHARE_READ, // разрешаем только запись в канал
		(LPSECURITY_ATTRIBUTES)NULL, // защита по умолчанию
		OPEN_EXISTING, // открываем существующий канал
		0, // атрибуты по умолчанию
		(HANDLE)NULL // дополнительных атрибутов нет
	);

	// проверяем связь с каналом
	if (hNamedPipe == INVALID_HANDLE_VALUE)
	{
		wcerr << L"Ошибка при подключении к серверу" << endl
			<< L"Код ошибки: " << GetLastError() << endl;
		wcout << L"Нажмите любую клавишу для выхода";
		_getch();
		return 0;
	}
	char connect[] = "connect";
	// пишем в именованный канал
	char in[100];
	DWORD dwBytesRead;
	while (true) {
		if (!ReadFile(// считываем
			hNamedPipe, // дескриптор канала
			in, // адрес буфера для ввода данных
			100, // максимум 100 байт
			&dwBytesRead, // количество прочитанных байтов
			(LPOVERLAPPED)NULL // передача данных синхронная
		))
		{
			wcerr << L"Не удалось прочитать данные из канала" << endl// если нулевой код, то выводим ошибку
				<< L"Код ошибки: " << GetLastError() << endl;
			CloseHandle(hNamedPipe);
			wcout << L"Нажмите любую клавишу для выхода";
			_getch();
			return 0;
		}
		wcout << in << " ";// то, что считали, выводим в консоль
	}
	_getch();
	return 0;
}
