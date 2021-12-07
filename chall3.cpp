#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <math.h>
#include <vector>
#include <bitset>
using namespace std;

vector<unsigned int> compress(string data, vector<string>& dict)
{
    vector<unsigned int> encoding;
    int nbit = 9;
    int dictSize = pow(2, nbit);
    int exist;
    dict.push_back("");
    string place, next;
    place = data[0];
    for (int i = 0; i < data.length(); i++)
    {
        if (i != data.length() - 1)
            next += data[i + 1];
        int pos = 0;
        while (dict[pos] != "")
        {
            if (place + next == dict[pos] && i != data.length() - 1)
            {
                place += next;
                exist = pos;
                break;
            }
            pos++;
        }
        if (dict[pos] == "")
        {
            if (place.length() < 2)
                encoding.push_back((int)place[0]);
            else encoding.push_back(exist + 256);
            if (i != data.length() - 1)
            {
                dict[pos] = place + next;
                dict.push_back("");
                place = next;
            }
        }
        next = "";
    }
    return encoding;
}
void writeFile(vector<unsigned int> encoding)
{
    ofstream output;
    string code;
    string bitWrite;
    output.open("data.lzw", ios::binary);
    for (int i = 0; i < encoding.size(); i++)
    {
        cout << encoding[i] << " ";
        code = "";
        code += bitset<10>(encoding[i]).to_string();
        for (int j = 0; j < 10; j++)
        {
            bitWrite += code[j];
            if ((i*10 + j) % 8 == 7 || (i*10 + j) == encoding.size()*10 - 1)
            {
                while (bitWrite.length() < 8)
                    bitWrite += "0";
                bitset<8> bit(bitWrite);
                output.write((char*)&bit, 1);
                bitWrite = "";
            }
        }
    }
    output.close();
}
vector<unsigned int> dataInFile()
{
    vector<unsigned int> data;

    ifstream readF;
    readF.open("data.lzw", ios::binary);
    string str;
    bitset<8> t;
    while (!readF.eof())
    {
        readF.read((char*)&t, 1);
        str += t.to_string();
    }
    readF.close();
    str.erase(str.length() - 8, str.length());
    while (str.length() % 10 != 0)
        str.pop_back();
    cout << str << endl;
    for (int i = 0; i < str.length(); i+=10)
    {
        string subStr;
        for (int j = 0; j < 10; j++)
        {
            subStr += str[i + j];
        }
        bitset<10> a(subStr);
        data.push_back(a.to_ulong());
    }
    //data.pop_back();
    return data;
}
string extract()
{
    string decoding = "";
    string code = "";
    int dictSize = 256;
    vector<unsigned int> dataFile = dataInFile();
    for (int i = 0; i < dataFile.size(); i++)
    {
        cout << dataFile[i] << " ";
    }
    cout << endl;
    vector<string> dict;
    for (int i = 0; i < 256; i++)
    {
        string str = "";
        str += char(i);
        dict.push_back(str);
    }
    dict.push_back("");
    code += char(dataFile[0]);
    decoding += code;
    for (int i = 1; i < dataFile.size(); i++)
    {
        string s;
        if (dataFile[i] < 256)
        {
            decoding += char(dataFile[i]);
            s = code + char(dataFile[i]);
            dict[dictSize] = s;
            dictSize++;
            s = s[s.length() - 1];
        }
        else
        {
            int pos = 256;
            while (dict[pos] != "")
            {
                if (dataFile[i] != pos)
                    pos++;
                else break;
            }
            if (dict[pos] == "")
            {
                s = dict[dataFile[i - 1]] + dict[dataFile[i - 1]][0];
                dict[dictSize] = s;
                dictSize++;
                decoding += s;
            }
            else
            {
                decoding += dict[pos];
                s = code + dict[pos][0];
                dict[dictSize] = s;
                dictSize++;
                s = dict[pos];
            }
        }
        cout << dict[dictSize - 1] << endl;
        dict.push_back("");
        code = s;
    }
    dict.push_back("");
    cout << decoding << endl;
    return decoding;
}
int main()
{
    // ifstream input("data.txt"), readF;
    // string data;
    // input >> data;
    // vector<string> dict;
    // vector<unsigned int> encoding = compress(data, dict);
    // writeFile(encoding);
    // cout << endl;
    string decoding = extract();
    // string num;
    // for (int i = 0; i < dict.size() - 1; i++)
    //     cout << dict[i] << ": " << i + 256 << endl;
}
