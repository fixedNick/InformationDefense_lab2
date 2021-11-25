#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <locale>

using namespace std;

string baseEncodedText;

class Efimenko_vigener
{
private:
	string key;
	int mod;
	string word;
public:
	static string get_lower_case_text(string _text)
	{
		string text;
		std::locale loc("ru_RU");
		for (std::string::size_type i = 0; i < _text.length(); ++i)
		{
			unsigned char ch = std::tolower((unsigned)_text[i], loc);
			text += ch;
		}

		return text;
	}

	// ≈сли мы приведем нашу строку в lowercase, то необходимо запомнить изначальную строку,
	// потому что 'ѕ' и 'п' при дешифровке будут смещены неверно
	Efimenko_vigener(string key_input, int size_N = 255)
	{
		key = key_input;
		mod = size_N;
	}
	Efimenko_vigener()
	{
	}
	string encrypt(string word)
	{
		string output = "";
		int j = 0;

		for (int i = 0; i < word.size(); i++)
		{
			if (j >= key.size())
				j = 0;

			//unsigned char uchKeySymbol = (unsigned char) key[j];
			//unsigned char uchWordSymbol = (unsigned char)word[j];
			//unsigned char uchResult = (uchKeySymbol + uchWordSymbol) % mod;

			unsigned char ch = word[i] + key[j];
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

	// ++
	vector<pair<unsigned char, int>> get_sorted_symbols_count(string _text)
	{
		string text = get_lower_case_text(_text);
		bool chars_nullable[255];
		vector<pair<unsigned char, int>> chars_counter;
		for (auto& nullable : chars_nullable)
			nullable = false;

		for (int i = 0; i < text.size(); i++)
		{
			for (int j = 0; j < 255; j++)
			{
				unsigned char ucSymbol = (unsigned char)tolower(text[i]);

				if (chars_nullable[j] == false)
				{
					pair<unsigned char, int> c_counter_pair = pair<unsigned char, int>(ucSymbol, 1);
					chars_counter.push_back(c_counter_pair);
					chars_nullable[j] = true;
					break;
				}
				else if (ucSymbol == chars_counter.at(j).first)
				{
					chars_counter.at(j).second++;
					break;
				}
				//cout << chars_counter.at(j).first << " " << chars_counter.at(j).second<<endl;
				continue;
			}
		}
		return chars_counter;
	}

	// ++
	vector<pair<unsigned char, double>> get_sorted_symbols_countd(string _text)
	{
		string text = get_lower_case_text(_text);
		bool chars_nullable[255];
		vector<pair<unsigned char, double>> chars_counter;
		for (auto& nullable : chars_nullable)
			nullable = false;

		for (int i = 0; i < text.size(); i++)
		{
			for (int j = 0; j < 255; j++)
			{

				char ch = text[i];
				unsigned char chAny = (unsigned char)ch;
				unsigned char ucSymbol = tolower((int)chAny);
				ucSymbol = (unsigned char)tolower((unsigned)ucSymbol);

				if (chars_nullable[j] == false)
				{
					pair<unsigned char, double> c_counter_pair = pair<unsigned char, double>(ucSymbol, 1);
					chars_counter.push_back(c_counter_pair);
					chars_nullable[j] = true;
					break;
				}
				else if (ucSymbol == chars_counter.at(j).first)
				{
					chars_counter.at(j).second++;
					break;
				}
				//cout << chars_counter.at(j).first << " " << chars_counter.at(j).second<<endl;
				continue;
			}
		}
		return chars_counter;
	}
	// ++
	double findEC(vector<pair<unsigned char, int>>& sub)
	{
		double index = 0;
		int N = 0;
		for (auto pair : sub)
			N += pair.second;

		for (auto pair : sub)
		{
			double add = 0;
			int num = pair.second * (pair.second - 1);
			int k = N * (N - 1);
			add = (double)num / (double)k;
			index += add;
		}
		return index;
	}
	// ++
	pair<unsigned char, double> decrypt_by_average_symbol_value(string ref_text)
	{
		// “ут частоты каждого символа эталонного текст в виде вектор пары (буква, частота)
		vector<pair<unsigned char, double>> standart_text_symbols_data = get_sorted_symbols_countd(ref_text);
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
		cout << "Letter: " << buf.first << "  frequency:" << buf.second << endl;
		return buf;
	}
	// ++
	vector<unsigned char> frequency_analysis(int key_length, string textEnc, pair<unsigned char, double> max_fr_letter)
	{
		/// ѕолучаем все L-граммы в зависимости от длины ключа
		vector<unsigned char> result;
		string result2 = "";
		int _LGrammCount = key_length;
		vector<string>_substrs(_LGrammCount);
		int _tmp = 0;
		for (int i = 0; i < textEnc.size(); i++)
		{
			_substrs[_tmp].push_back(textEnc[i]);
			_tmp++;
			if (_tmp >= key_length)
				_tmp = 0;
		}


		for (int j = 0; j < _LGrammCount; j++)
		{
			cout << "L-gramma [" << j << "]: " << endl;
			//cout << _substrs[j] << endl;
			pair<unsigned char, int> high_freq_sub = decrypt_by_average_symbol_value(_substrs[j]);
			int i1 = (int)high_freq_sub.first;
			int i2 = (int)max_fr_letter.first;
			int res = i1 - i2;

			unsigned char difference = (unsigned char)res;

			result.push_back(difference);
			result2 = +difference;
		}
		return result;
	}

	// --++--
	void findIndexs(string word2)
	{
		vector <double> indexes;
		vector<string> substring;
		vector<pair<unsigned char, int>> collect_index;
		//find length
		//find substrings
		int tmp_counter = 0;
		int key_length = 2;
		string substr_result = "";
		do
		{
			for (int i = 0; i < word2.size(); i++)
			{
				if (++tmp_counter % key_length == 0) {
					substr_result += word2[i];
				}
			}
			substring.push_back(substr_result);
			//cout << substr_result;
			collect_index = get_sorted_symbols_count(substr_result);
			indexes.push_back(findEC(collect_index));
			key_length++;
			substr_result = "";
			tmp_counter = 0;
		} while (key_length < 20);
	}
	// find indexes

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
	//ofstream writer("base.txt", ios::binary);
	//vector<unsigned char> text;
	//string strtext = "HellO, MafaWeR ѕривет мир, это какое-то сообщение, чтобы пон€ть, правильно ли мы получаем Ћ√раммы или что-то идет не по плану.\n¬от нова€ строка, а мы и дальше пытаемс€ написать большой текст от руки\nЁто сама€ глупа€ зате€, но что поделать, будем писать, ведь если даже не пытатьс€ это сделать, то в мире может загрустить одна византийска€ ваза\n’оть эта глупа€ ваза и думает, что должна все делать сама, никто ей помогать не должен и т.д, и т.п., но € не буду слушать вазу, ну это же глупость кака€, вз€ть и слушать вазу, лучше делать так, как считаю нужным и не останавливатьс€ в попытках помочь\nƒа, € - идиот, ничего не удаетс€ сделать, времени тупо не хватает, но кто сказал, что € не смогу найти решени€, может и не своего, мне ведь главное, чтобы мо€ мила€ ваза все усвоила и успешно сдала работы, а она все протекает, и протекает и вовсе не в том смысле, в котором ей это дозволено, поэтому не знаю что тут думать";
	//string lowerstr = "";
	//std::locale loc("ru_RU");
	//for (std::string::size_type i = 0; i < strtext.length(); ++i)
	//{
	//	unsigned char ch = std::tolower((unsigned)strtext[i], loc);
	//	lowerstr += ch;
	//}


	 //![0] = ѕв рэ к- оеечбп€,ринлмпуеЋаылч- енппн\nто€тк миашпаснитбьйетту\nоа€лазе  оольбепа, е лдееыт€тсльтви ж гстоаинйавао алава мтч ласда м к  ма  лн ди., о  дсшьа, утжгптка € сшьа, уеетт, аст жм  таит€ пк мьа€ и, игнуе€да, ре пнхттнк алч нсгнтре€меиевг ее ао о €и€а еси уеодаат о ерее пта вснвосс, тоеэ злоптуен оуда
	 //![1] = рем,ткотсбн,тыот алоиыочм√м иттитеолу¬ в рааы леые€асьош к  кЁ м у€а€нч да,умит деиа  ть ода,о рмезртьд зтс€а\nтэ у€а да, тдж еетсантепотндж т, п  нбула з  оелоьа€втила з ч льа кчанн ноаввь пыапо\n, ио чоедт ет ентоева, отсз, т€емуаиен, от  оом дгвечбм л зв влисш а быанв октирееиоее ммл комйтдве, оо  ач тут
	 //![2] = ити оаеоощи о ньпвь   ла рми оод   а.онасо,  дь тм па лотсоритсагп т,отпет д сьвьс жнпасэ ет  меотауи нваик з’ьтгп зиуе оонв льа,иойогьеоеи. т.н€еу утвунэ  ус к,зь утвулшда кк июуыиеснласвотхочƒ -дтне  ассльвмиу  ае  ока о  о й ши ж нсе,нвьлн,тыомавасуоа пнслро, аспта, окт в  т ыевор  ооон эмнзютт мь


	//for (int i = 0; i < strlen(strtext.c_str()); i++)
	//{
	//	unsigned char uch = (unsigned char) strtext[i];
	//	text.push_back(uch);
	//	writer << text[i];
	//}

	//char c = '–';
	//char lowerc = (char) tolower((unsigned)c);

	//Efimenko_vigener crypt = Efimenko_vigener();
	//crypt.frequency_analysis(3, strtext, pair<unsigned char, int>(' ', 1.423));

	//writer.close();

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
			Efimenko_vigener chiefr2;
			chiefr2.findIndexs(Efimenko_vigener::get_lower_case_text(word2));
			baseEncodedText = word2;

			cout << "Enter name reference text: "; //Input ideal text and count frequency 
			cin >> filename;
			ifstream fin(filename, ios::binary);
			word_reference = "";
			while (!fin.eof())
			{
				getline(fin, count);
				word_reference += count;
			}
			fin.close(); //begin frequency analyse

			pair<unsigned char, double> high_freq_ideal = chiefr2.decrypt_by_average_symbol_value(Efimenko_vigener::get_lower_case_text(word_reference)); //самый частовстречаемый символ в эталонном тексте

			vector<unsigned char> keyChange = chiefr2.frequency_analysis(3, Efimenko_vigener::get_lower_case_text(word2), high_freq_ideal); //find key_name
			ofstream fout;
			fout.open("key.txt", ios::binary);
			for (auto pos : keyChange)
			{
				fout << pos;
				cout << pos;
			}
			fout.close();
			break;
		}
		case 3:
		{
			return 0;
		}
		}
		cout << '\n';
	}
	return 0;
}
