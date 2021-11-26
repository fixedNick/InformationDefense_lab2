/// 

#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include <windows.h>
#include <vector>
#include <algorithm>

using namespace std;

class Efimenko_vigener
{
private:
	string key;
	int mod;
	string word;
public:
	Efimenko_vigener(string key_input, int size_N = 255)
	{
		key = key_input;
		mod = size_N;
	}
	Efimenko_vigener()
	{
	}

	/// Мы вычиляем в ecrypt/decrypt как сумму char'ов ключа и шифруемого текста
	/// Записываем все в unsigned char, без явного преобразования(без (unsigned char) var_name)
	/// После добавляем символ в строку output, предположительно проиcходит неявное преобразование в char.
	string encrypt(string word)
	{
		string output = "";
		int j = 0;

		for (int i = 0; i < word.size(); i++)
		{
			if (j >= key.size())
				j = 0;

			char wordSymbol = word[i];
			char keySymbol = key[j];

			unsigned char ch = wordSymbol + keySymbol;
			ch = ch % mod;
			output += ch;
			j++;
		}
		return output;
	}
	string decrypt(string data)
	{
		string output;
		int j = 0;
		for (int i = 0; i < data.size(); i++)
		{
			if (j >= key.size())
				j = 0;

			unsigned char ch = data[i] - key[j];
			ch = (ch + mod) % mod;
			output += ch;
			j++;
		}
		return output;
	}

	/// Метод для вычисления сколько раз каждый уникальный символ встречается в тексте
	/// Возвращает вектор пар: <символ, сколько раз встречается>
	/// Пример <'c', 22>
	vector<pair<unsigned char, int>> get_symbols_count(string text)
	{
		// NULL'able массив, чтобы проверять, до какого элемента мы уже записали уникальные символы
		// Если при проверке допустим chars_nullable[27] в нем будет false - значит 28,29..254 элементы точно false
		// Далее этот пример [27] будет обозначен как [ИНДЕКС], предполагая, что до него 0..26 все TRUE
		// Соответственно, если мы искали какой-то символ, но на каком-то моменте chars_nullable[ИНДЕКС] имеет false
		// Значит мы еще не записали символ, добавим его в вектор chars_counter (он будет на позиции [ИНДЕКС]) , где хранятся наши пары
		// А в chars_nullable[ИНДЕКС] запишем TRUE, это будет означать, что [ИНДЕКС] занят каким-то символом, этот символ
		// можно будет достать из chars_counter по тому же [ИНДЕКС]
		bool chars_nullable[255];
		vector<pair<unsigned char, int>> chars_counter;
		for (auto& nullable : chars_nullable) // Инициализируем массив пустыми элементами (false)
			nullable = false;

		for (int i = 0; i < text.size(); i++)
		{
			for (int j = 0; j < 255; j++)
			{
				unsigned char ucSymbol = (unsigned char)text[i]; // Необходимое явное преобразование, для корректного сравнения

				if (chars_nullable[j] == false) // Дошли до индекса [j] где уже нет элемента, дальше проверять нет смысла, там везде false
				{
					// Добавляем новый найденный символ в вектор пар, а в chars_nullable[j] записываем, что индекс занят
					pair<unsigned char, int> c_counter_pair = pair<unsigned char, int>(ucSymbol, 1);
					chars_counter.push_back(c_counter_pair);
					chars_nullable[j] = true;
					break;
				}
				else if (ucSymbol == chars_counter.at(j).first) // Символ найден в нашем векторе
				{
					// Увеличиваем количество данного символа в строке
					chars_counter.at(j).second++;
					break;
				}
				continue;
			}
		}
		return chars_counter;
	}

	// Индексы определяются, раньше они показывали отрицательные значения из-за переполнения памяти
	// Томы Tолстого влезают в только в long long, потому что значение k - длина строки - 36 миллиардов
	double findEC(vector<pair<unsigned char, int>>& sub)
	{
		double index = 0;
		long long N = 0;
		for (auto pair : sub)
			N += pair.second;

		for (auto pair : sub)
		{
			double add = 0;
			long long num = pair.second * (pair.second - 1);
			long long k = N * (N - 1);
			add = (double)num / (double)k;
			index += add;
		}
		return index;
	}
	pair<unsigned char, double> decrypt_by_average_symbol_value(string ref_text)
	{
		// Тут частоты каждого символа эталонного текст в виде вектор пары (буква, частота)
		vector<pair<unsigned char, int>> standart_text_symbols_data = get_symbols_count(ref_text);
		vector<pair<unsigned char, double>> frequency_vector;
		pair<unsigned char, double>letter_frequency;
		for (auto& pair : standart_text_symbols_data)
		{
			double frequency = (double)pair.second / (double)ref_text.size();
			letter_frequency.first = pair.first;
			letter_frequency.second = frequency;
			frequency_vector.push_back(letter_frequency);
		}
		pair< unsigned char, double > buf(frequency_vector.at(0).first, frequency_vector.at(0).second);
		for (auto& pair : frequency_vector)
		{
			if (pair.second > buf.second)
				buf = pair;
		}
		cout << "Letter: " << (char)buf.first << "  frequency:" << buf.second << endl;
		return buf;
	}
	vector<unsigned char> frequency_analysis(int key_length, string textEnc, unsigned char max_fr_letter)
	{
		/// Получаем все L-граммы в зависимости от длины ключа
		vector<unsigned char> result;
		int _LGrammCount = key_length;
		vector<string>_substrs(_LGrammCount);
		int _tmp = 0;
		// Дробим текст на ЛГраммы
		for (int i = 0; i < textEnc.size(); i++)
		{
			_substrs[_tmp].push_back(textEnc[i]);
			_tmp++;
			if (_tmp >= key_length)
				_tmp = 0;
		}

		// Пробегаемся по каждой ЛГрамме - находим в ней самый встречающийся символ, вычитаем из него самый встречающийся
		// символ эталона - это и будет одним из символов нашего ключа
		for (int j = 0; j < _LGrammCount; j++)
		{
			cout << "L-gramma [" << j << "]: " << endl;
			pair<unsigned char, int> high_freq_sub = decrypt_by_average_symbol_value(_substrs[j]);

			int i1 = (int)high_freq_sub.first; // Самый встречающийся символ в ЛГрамме
			int i2 = (int)max_fr_letter; // Самый встречающийся символ в эталоне
			int res = i1 - i2; // Символ нашего ключа

			unsigned char difference = (unsigned char)res; // Символ нашего ключа

			result.push_back(difference); // Записываем ключ
		}
		return result;
	}

	// Метод делит наш шифротекст на ЛГраммы и для каждой находит индекс совпадения
	// Возвращает все индексы совпадения
	vector<double> findIndexs(string word2)
	{
		vector <double> indexes;
		int tmp_counter = 0;
		string substr_result = "";

		for (int key_length = 2; key_length < 20; key_length++)
		{
			for (int i = 0; i < word2.size(); i++)
			{
				if (++tmp_counter % key_length == 0) {
					substr_result += word2[i];
				}
			}
			auto sybstr_symbols = get_symbols_count(substr_result);
			indexes.push_back(findEC(sybstr_symbols));
			substr_result = "";
			tmp_counter = 0;
		}
		return indexes;
	}
};

void PrintMenu()
{
	cout << "1.Encrypt/Decrypt" << endl
		<< "2.Decrypt without key" << endl
		<< "3.Exit" << endl
		<< "Choice:";
}

int main()
{
	while (1) {
		PrintMenu();
		int x;
		cin >> x;
		switch (x) {
			case 1:
			{
				string filename, string, word, key, count;
				cout << "Enter name of file: ";
				cin >> filename;
				ifstream inf(filename, ios::binary);

				if (inf.is_open() == false) {
					cout << "file doesnt exists" << endl;
					continue;
				}

				while (!inf.eof())
				{
					getline(inf, count);
					word += count;
				}
				inf.close();

				cout << "Enter key: ";
				cin >> key;
				Efimenko_vigener chiefr(key, 255);

				ofstream fout;
				std::string enc_text = chiefr.encrypt(word);
				fout.open("encrypted.txt", ios::binary);
				fout << enc_text;
				fout.close();
				fout.open("decrypted.txt", ios::binary);
				fout << chiefr.decrypt(chiefr.encrypt(word));
				fout.close();
				break;
			}
			case 2:
			{
				string filename, string, word_reference, word2, key, count;
				cout << "Enter name of file: ";
				cin >> filename;
				ifstream inf(filename, ios::binary);
				word2 = "";
				while (!inf.eof())
				{
					getline(inf, count);
					word2 += count;
				}
				inf.close();
				
				cout << "Enter name reference text: "; //Input ideal text and count frequency 
				cin >> filename;
				ifstream fin(filename, ios::binary);
				word_reference = "";
				while (!fin.eof())
				{
					getline(fin, count);
					word_reference += count;
				}
				fin.close();
				
				Efimenko_vigener chiefr2;
				vector<double> indexes = chiefr2.findIndexs(word2);
				// Тут мы уже получили все индексы совпадений для всех длин ключа, даем пользователю возможность выбрать
				// понравившуюся ему длину ключа и попробовать расшифровать по ней текст, с возможностью перевыбора

				while (true) {
					cout << "Coincidence indexes: " << endl;
					for (int i = 0; i < indexes.size(); i++)
						cout << "For key length [" << i + 2 << "], index is [" << indexes[i] << "]" << endl;

					int input_key_length = 0;
					cout << "Enter key length to decrypt text or '0' to exit: ";
					cin >> input_key_length;

					if (input_key_length == 0) return 0;
					else if (input_key_length == 1 || input_key_length-2 > indexes.size()) {
						cout << "We didnt calculate ci for this key length" << endl;
						continue;
					}
					
					pair<unsigned char, double> high_freq_ideal = chiefr2.decrypt_by_average_symbol_value(word_reference); //самый частовстречаемый символ в эталонном тексте

					vector<unsigned char> key = chiefr2.frequency_analysis(input_key_length, word2, high_freq_ideal.first); //find key_name
					ofstream fout;
					fout.open("key.txt", ios::binary);

					cout << "Found key [";
					for (auto pos : key)
					{
						char charS = pos;
						fout << charS;
						cout << charS;
					}
					cout << "]" << endl;
					fout.close();
				}

				break;
			}
			case 3: return 0;
		}
		cout << '\n';
	}
	return 0;
}
