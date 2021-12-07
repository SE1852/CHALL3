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
    vector<unsigned int> encodingList;
    int exist = 0;
    dict.push_back(""); //Thêm rỗng để xét
    //place để lưu các chuỗi để xét tồn tại trong từ điển mở rộng, next để lưu kí tự tiếp theo sẽ thêm vào place
    string place, next; 
    //xét kí tự đầu tiên
    place = data[0];
    for (int i = 0; i < data.length(); i++)
    { 
        //lưu kí tự tiếp theo
        if (i < data.length() - 1)
            next += data[i + 1];
        int pos = 0; //biến xét vị trí trong từ điển
        while (dict[pos] != "")
        {
            if (place + next == dict[pos] && i != data.length() - 1)
            {
                //nếu từ đã tồn tại trong từ điển mở rộng (>255) thì chèn next vào
                place += next;
                exist = pos; //lưu lại vị trí
                break;
            }
            pos++;
        }
        //Nếu place không tồn tại trong từ điển
        if (dict[pos] == "")
        {
            //thêm vào danh sách mã hóa
            if (place.length() < 2)
                encodingList.push_back((int)place[0]);
            else encodingList.push_back(exist + 256);
            //thêm từ vào từ điển
            //nếu rơi vào trường hợp kí tự cuối thì việc thêm vào sẽ bị lặp do đó chỉ xét tới vị trí kế cuối
            if (i < data.length() - 1)
            {
                //thêm (từ đã tồn tại+kí tự tiếp theo)
                dict[pos] = place + next;
                dict.push_back("");
                place = next; //chuyển tới kí tự tiếp theo để tiếp tục mã hóa
            }
        }
        next = "";
    }
    return encodingList;
}
void writeFile(vector<unsigned int> encodeList)
{
    ofstream output;
    string code; //biến chuyển số về 10bits
    string bitWrite; //biến để ghi 8bits vào file
    output.open("data.lzw", ios::binary);
    for (int i = 0; i < encodeList.size(); i++)
    {
        code = "";
        //bitset<10> để đưa số về 10bits theo dạng nhị phân
        code += bitset<10>(encodeList[i]).to_string();
        for (int j = 0; j < 10; j++)
        {
            bitWrite += code[j];
            //Nếu đủ 8bits hoặc duyệt tới bit cuối cùng
            if ((i*10 + j) % 8 == 7 || (i*10 + j) == encodeList.size()*10 - 1)
            {
                //Nếu 8bits cuối chưa đủ thì chèn thêm các bit 0 vào cho đủ
                while (bitWrite.length() < 8)
                    bitWrite += "0";
                //Chuyển về bit để ghi bit
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
    ifstream input;
    input.open("data.lzw", ios::binary);
    string str;
    bitset<8> code;
    while (!input.eof())
    {
        //Đọc bit vào biến code từng byte một
        input.read((char*)&code, 1);
        //chèn vào chuỗi tổng
        str += code.to_string();
    }
    input.close();
    //Trong quá trình đọc eof() bị dư 1 byte do đó trừ 8 bits cuối
    str.erase(str.length() - 8, str.length());
    //Dịch trên 10bits cho nên nếu dư phải trừ bớt bits
    while (str.length() % 10 != 0)
        str.pop_back();
    //duyệt bit bước nhảy 10
    for (int i = 0; i < str.length(); i+=10)
    {
        string subStr;
        //lưu 10bits vào chuỗi con
        for (int j = 0; j < 10; j++)
        {
            subStr += str[i + j];
        }
        //chuyển chuỗi con về dạng bit để chuyển sang thập phân
        bitset<10> a(subStr);
        //chèn số thập phân vào dữ liệu
        data.push_back(a.to_ulong());
    }
    return data;
}
string extract()
{
    string decoding = ""; //chuỗi mã giải
    string prevWord = ""; //Chuỗi lưu từ để xét trong từ điển
    int dictSize = 256; //Lượng từ đã có trong từ điển
    vector<unsigned int> dataFile = dataInFile(); //đọc mã từ file
    vector<string> dict; //Từ điển
    //Thêm 256 kí tự ASCII vào từ điển
    for (int i = 0; i < 256; i++)
    {
        string str = "";
        str += char(i);
        dict.push_back(str);
    }
    dict.push_back(""); //Thêm rỗng để xét
    prevWord += char(dataFile[0]); //Xét kí tự đầu tiên
    decoding += prevWord; //thêm kí tự đầu tiên vào mã giải
    //Xét các mã tiếp theo
    for (int i = 1; i < dataFile.size(); i++)
    {
        string wordAdd; 
        //Nếu từ tiếp theo đơn kí tự
        if (dataFile[i] < 256)
        {
            decoding += char(dataFile[i]); //Thêm vào mã giải
            wordAdd = prevWord + char(dataFile[i]); //Từ mới = từ cũ + kí tự tiếp theo
            dict[dictSize] = wordAdd; //Thêm từ mới vào từ điển
            dictSize++; //Mở rộng từ điển
            wordAdd = wordAdd[wordAdd.length() - 1]; //đặt tới kí tự cuối của từ mới để xét bước tiếp
        }
        //Nếu từ tiếp theo nằm trong từ điển mở rộng
        else
        {
            //Kiểm tra xem từ này đã có trong từ điển chưa
            int pos = 256;
            while (dict[pos] != "")
            {
                if (dataFile[i] != pos)
                    pos++;
                else break;
            }
            //Nếu chưa
            if (dict[pos] == "")
            {
                //Từ mới sẽ = từ trước đó + kí tự đầu của từ đó
                wordAdd = dict[dataFile[i - 1]] + dict[dataFile[i - 1]][0];
                dict[dictSize] = wordAdd;
                dictSize++;
                decoding += wordAdd;
            }
            //Nếu đã tồn tại
            else
            {
                //Thêm thẳng vào mã giải
                decoding += dict[pos];
                //Từ mới = từ cũ trước đó + từ đang xét
                wordAdd = prevWord + dict[pos][0];
                dict[dictSize] = wordAdd;
                dictSize++;
                wordAdd = dict[pos]; //giữ nguyên từ đang xét để tính làm từ cũ
            }
        }
        dict.push_back(""); //Mở rộng từ điển
        prevWord = wordAdd; //Đặt lại từ cũ
    }
    dict.push_back("");
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
