#include <iostream>
#include <intrin.h>
using namespace std;

int main()
{
	const int n = 8;
	float M1[n][n];
	float M2[n][n];

	float M3[n][n];
	float temp[n];
	float rez[n][n];

	float tempcopy[n];
	float rezcopy[n][n];
	float M1copy[n][n], M2copy[n][n], M3copy[n][n];

	float x;
	float input = 2; // некоторое число, на которое умножаются все элементы матрицы

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++)
		{
			M1[i][j] = rand() % 10; //Заполняем все элементы матриц случайными числами от 0 до 9
			M2[i][j] = rand() % 10;
		}

	}
	setlocale(LC_ALL, "Rus");

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			M1copy[i][j] = M1[i][j]; //Создаем копии матриц
			M2copy[i][j] = M2[i][j];
		}
	}
	cout << "M1" << endl;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			cout << M1[i][j] << ' '; //Выводим матрицы до преобразований
		}
		cout << endl;
	}
	cout << "M2" << endl;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			cout << M2[i][j] << ' ';
		}
		cout << endl;
	}
	cout << endl;
	unsigned __int64 t1, t2; //временные метки

	t1 = __rdtsc();

	_asm
	{

		mov ecx, 16 // сложение матриц M1 и M2
		//в вектор xmm_i загоняется по 4 числа => 64/4=16 итераций
		LOOP1:

		mov eax, ecx
			mov dx, 16
			mul dx //формируем адрес ячейки
			movups xmm1, [M1 + eax - 16] //перемещаем 4 числа из матриц (начинаем с конца)
			movups xmm2, [M2 + eax - 16]
			addps xmm1, xmm2 //складываем вектора
			movups[rez + eax - 16], xmm1 //результат сохраняем в матрице rez
			loop LOOP1 //завершаем цикл

			mov ecx, 8 // перемножение матриц rez и M2

			LOOP2:

			cmp ecx, 0 //проверка на то, содержится ли в ecx 0
			je LOOP2EXIT //если содержится - переходим к метке LOOP2EXIT
			mov ebx, ecx //сохраняем кол-во оставшихся итераций в ebx, т.к ecx понадобится нам для вложенных циклов
			mov ecx, 8 //заносим в ecx новое значение итераций

			LOOP3 :

			cld //устанавливем флаг направления (df) в 0
			lea edi, [temp + ecx * 4 - 4] //заносим в edi адрес источника (2 операнд)
			lea esi, [M2 + ebx * 4 - 4] // заносим в esi адрес источника(2 операнд)
			mov eax, ecx 
			dec eax //получаем индекс столбца
			mov dx, 32 
			mul dx
			add esi, eax //получаем адрес элемента матрицы
			movsd 
			loop LOOP3
			mov ecx, 8

			LOOP4:

		cmp ecx, 0 //проверка ecx
			je LOOP4EXIT // если Ecx == 0: выходим
			mov eax, ecx
			mov dx, 32
			mul dx //формируем адрес ячейки
			movups xmm1, [rez + eax - 32] //заносим адреса перемножаемых ячеек в xmm1
			movups xmm2, [temp]//и в xmm2
			mulps xmm1, xmm2 //перемножение соотв. значений из xmm1 в xmm2, с сохранением в xmm1
			mov eax, ecx//сохранение в eax значения из ecx
			mov dx, 32
			mul dx //в eax получаем адрес
			movups xmm2, [rez + eax - 16] //в xmm2 заносим значение по указанному адресу
			movups xmm3, [temp + 16] // аналогично
			mulps xmm2, xmm3 //перемножаем
			addps xmm1, xmm2 //сложение соотв.значений из xmm1 в xmm2, с сохранением в xmm1
			movss[x], xmm1 //копируем элемент в память из младшего элемента xmm1
			fld[x] //Сохраняем это значение в стек
			shufps xmm1, xmm1, 11100101b //Меняем порядок элементов в xmm1
			movss[x], xmm1
			fld[x]
			shufps xmm1, xmm1, 11101010b
			movss[x], xmm1
			fld[x]
			shufps xmm1, xmm1, 11111111b
			movss[x], xmm1
			fld[x]
			fadd //суммируем элементы в стеке
			fadd
			fadd
			xor edi, edi //очищаем edi
			mov eax, ebx 
			dec eax
			mov dx, 4
			mul dx
			add edi, eax
			mov eax, ecx
			dec eax
			mov dx, 32
			mul dx
			add edi, eax
			fstp[M3 + edi] //кладем значение из стека в элемент матрицы, и удаляем его из стека
			dec ecx
			jmp LOOP4

			LOOP4EXIT :

		mov ecx, ebx // возвращаем в итератор значение внешнего цикла
			dec ecx // уменьшаем его на 1
			jmp LOOP2

			LOOP2EXIT :

		mov ecx, 16 // умножение матрицы M3 на число input

			LOOP5 :

			mov eax, ecx
			mov dx, 16
			mul dx
			movups xmm1, [M3 + eax - 16]
			movss xmm2, [input]
			shufps xmm2, xmm2, 00000000b
			mulps xmm1, xmm2
			movups[M3 + eax - 16], xmm1
			loop LOOP5

	}

	t2 = __rdtsc();

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			cout << M3[i][j] << ' ';
		}
		cout << endl;
	}

	cout << endl << "С SSE: " << (float)(t2 - t1) / 2100000000 << endl << endl;

	t1 = __rdtsc();

	_asm

	{
		mov ecx, 64 // сложение матриц M1 и M2

		LOOP6:

		fld[M1copy + ecx * 4 - 4]
			fld[M2copy + ecx * 4 - 4]
			fadd
			fstp[rezcopy + ecx * 4 - 4]
			loop LOOP6

			mov ecx, 8 // перемножение матриц rez и M2

			LOOP7:

		cmp ecx, 0
			je LOOP7EXIT
			mov ebx, ecx
			mov ecx, 8

			LOOP8 :

			cld
			lea edi, [tempcopy + ecx * 4 - 4]
			lea esi, [M2copy + ebx * 4 - 4]
			mov eax, ecx
			dec eax
			mov dx, 32
			mul dx
			add esi, eax
			movsd
			loop LOOP8
			mov ecx, 8

			LOOP9:

		cmp ecx, 0
			je LOOP9EXIT
			mov eax, ecx
			mov dx, 32
			mul dx
			fld[rezcopy + eax - 32]
			fld[tempcopy]
			fmul
			fld[rezcopy + eax - 28]
			fld[tempcopy + 4]
			fmul
			fadd
			fld[rezcopy + eax - 24]
			fld[tempcopy + 8]
			fmul
			fadd
			fld[rezcopy + eax - 20]
			fld[tempcopy + 12]
			fmul
			fadd
			fld[rezcopy + eax - 16]
			fld[tempcopy + 16]
			fmul
			fadd
			fld[rezcopy + eax - 12]
			fld[tempcopy + 20]
			fmul
			fadd
			fld[rezcopy + eax - 8]
			fld[tempcopy + 24]
			fmul
			fadd
			fld[rezcopy + eax - 4]
			fld[tempcopy + 28]
			fmul
			fadd
			xor edi, edi
			mov eax, ebx
			dec eax
			mov dx, 4
			mul dx
			add edi, eax
			mov eax, ecx
			dec eax
			mov dx, 32
			mul dx
			add edi, eax
			fstp[M3copy + edi]
			dec ecx
			jmp LOOP9

			LOOP9EXIT :

		mov ecx, ebx
			dec ecx
			jmp LOOP7

			LOOP7EXIT :

		mov ecx, 64 // умножение матрицы M3 на число input

			LOOP10 :

			fld[M3copy + ecx * 4 - 4]
			fld[input]
			fmul
			fstp[M3copy + ecx * 4 - 4]
			loop LOOP10
	}

	t2 = __rdtsc();

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			cout << M3copy[i][j] << ' ';
		}
		cout << endl;
	}

	cout << endl << "Без SSE: " << (float)(t2 - t1) / 2100000000 << endl;
}

