#include<iostream>
#include<windows.h> 

using namespace std;

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	cout << "Начинаем перемешивать файл"<<endl;

	OFSTRUCT f1 = {};
	HFILE file1 = OpenFile("file.txt", &f1, OF_READ);

	if (file1 == -1)
	{
		cout << "Ошибка открытия файла "<< GetLastError() <<endl;
		return -1;
	}

	HANDLE file2 = CreateFileA("result.txt", GENERIC_ALL, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (file2 == INVALID_HANDLE_VALUE)
	{
		cout << "Ошибка создания файла " << GetLastError() << endl;
		return -1;
	}

	DWORD file1size = GetFileSize((HANDLE)file1, nullptr);

	HANDLE heap = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, file1size - 1, file1size);

	if (heap == 0)
	{
		cout << "Ошибка выделения кучи " << GetLastError() << endl;
		return -1;
	}

	char *f1d = (char*)HeapAlloc(heap, HEAP_ZERO_MEMORY, file1size);

	LPDWORD f1as = nullptr;
	ReadFile((HANDLE)file1, f1d, file1size, f1as, 0);


	HANDLE mapping = CreateFileMappingA(file2, 0, PAGE_READWRITE, 0, file1size, 0);

	if (mapping == 0)
	{
		cout << "Mapping creation failed" << GetLastError() << endl;
		return -1;
	}

	char *mapped = (char*)MapViewOfFile(mapping, FILE_MAP_WRITE, 0, 0, file1size);
	int blocknumber = file1size / 256;
	cout << "Кол-во блоков " <<blocknumber << endl;
	int block = 0;
	for (int i = 0, j = 0; i < file1size; i += 256, j += 256)
	{
		if (file1size % 256 == 0)
		{
			cout << "Блоков целое число" << endl;
			memcpy(mapped + i, f1d + j + 128, 128);
			memcpy(mapped + i + 128, f1d + j, 128);
		}
		else
		{
			cout << "Блоков нецелое число" << endl;
			cout << blocknumber << " " << i << endl;
			cout << "file1size - i" << file1size - i << endl;
			if (blocknumber == block) 
			{
				if (file1size - i <= 128)
				{	
					cout << "Блок меньше 128" << endl;
					memcpy(mapped + i, f1d + j, file1size - i);
				}
				if (file1size - i > 128)
				{
					cout << "Блок больше 128" << endl;
					memcpy(mapped + i, f1d + j + 128, file1size - i - 128);
					memcpy(mapped + file1size - 128, f1d + j, 128);
				}

			}
			else
			{	
				cout << "Целый блок" << endl;
				memcpy(mapped + i, f1d + j + 128, 128);
				memcpy(mapped + i + 128, f1d + j, 128);
				block++;
			}
			
		}
		
		
	}
	

	HeapFree(heap, 0, f1d);

	CloseHandle(mapping);
	CloseHandle((HANDLE)file1);
	CloseHandle((HANDLE)file2);

	HeapDestroy(heap);
	cout << "Файл перемешан" << endl;
	system("pause");
	return 0;
}