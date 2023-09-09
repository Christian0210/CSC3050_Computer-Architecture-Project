#include <iostream>
#include <cstdio>
#include <malloc.h>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <cmath>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "MIPS_to_Binary.h"
using namespace std;

const int R = 0, I = 1, J = 2, S = 3;                          // The constants used to identify different funtions.

const int memory_size = 6 * 1024 * 1024 * sizeof(char);
const int reg_size = 34 * sizeof(int);
const int text_address = 0x00400000;
const int data_address = 0x00500000;
const int data_type = 0, text_type = 1;

int MIPSnum, datanum;
void* reg;
void* start;
void* dataend;

vector<string> MIPS;
vector<string> dataset;


string int_to_mips(int mips)
{
    int i = 31, sign = mips / abs(mips);
    string mips_code = "00000000000000000000000000000000";

    while (mips)
    {
        mips_code[i--] = char(abs(mips % 2) + 48);
        mips /= 2;
    }

    if (sign == -1)
    {
        for (i = 1; i < 32; i++)
        {
            mips_code[i] = char(97 - mips_code[i]);
        }
        for (i = 31; i > 0; i--)
        {
            mips_code[i] = char(97 - mips_code[i]);
            if (mips_code[i] == 49)
                break;
        }
        mips_code[0] = '1';
    }
    return mips_code;
}


int mips_to_int(string mips_code, bool rev = false)
{
    int i, mips = 0;
    for (i = 0; i < mips_code.size(); i++)
    {
        if (rev)
            mips = (mips << 1) + (49 - int(mips_code[i]));
        else
            mips = (mips << 1) + (int(mips_code[i]) - 48);
    }
    return mips;
}


int str_to_int(string str)
{
    int tmp = 0, sign = 1, j;
    if (str[0] == '-')
    {
        sign = -1;
        str.erase(0, 1);
    }
    for (j = 0; j < str.size(); j++)
    {
        tmp = tmp * 10 + (int(str[j]) - 48);
    }
    return tmp;
}


void load_data(void* head)
{

    int i, j, begin, end, space;
    char c;
    string type, str;

    for (i = 0; i < datanum; i++)
    {
        end = dataset[i].find(":");
        if (end != -1)
            dataset[i].erase(0, end + 1);
        dataset[i] = clear_invalid(dataset[i]);

        end = dataset[i].find(' ');
        if (end != -1)
        {
            type = clear_invalid(dataset[i].substr(0, end));
            dataset[i].erase(0, end);
        }
        if (type == ".asciiz")
        {
            begin = dataset[i].find("\"");
            end = dataset[i].find("\"", begin + 1);
            str = dataset[i].substr(begin + 1, end - begin - 1);
            str = str + '\0';
            space = ceil((double)str.size() / 4.0) * 4;
            memcpy(head, str.c_str(), str.size());
            head = (void*)((long long)(head)+space);
        }

        if (type == ".ascii")
        {
            begin = dataset[i].find("\"");
            end = dataset[i].find("\"", begin + 1);
            str = dataset[i].substr(begin + 1, end - begin - 1);

            space = ceil(str.size() / 4) * 4;
            for (j = 0; j < str.size(); j++)
            {
                c = str[j];
                memcpy(head, &c, sizeof(char));
                head = (void*)((long long)(head)+1);
            }
            head = (void*)((long long)(head)+space - str.size());
        }

        if (type == ".word")
        {
            begin = 0;
            end = dataset[i].find(",", begin);
            while (end != -1)
            {
                str = clear_invalid(dataset[i].substr(begin, end - begin));
                int tmp = str_to_int(str);
                memcpy(head, &tmp, sizeof(int));

                head = (void*)((long long)(head)+sizeof(int));

                begin = end + 1;
                end = dataset[i].find(",", begin);
            }
            str = clear_invalid(dataset[i].substr(begin, end - begin));
            int tmp = str_to_int(str);
            memcpy(head, &tmp, sizeof(int));

            head = (void*)((long long)(head)+sizeof(int));
        }

        if (type == ".half")
        {
            begin = 0;
            end = dataset[i].find(",", begin);
            while (end != -1)
            {
                str = clear_invalid(dataset[i].substr(begin, end - begin));
                short tmp = str_to_int(str);
                memcpy(head, &tmp, sizeof(short));

                head = (void*)((long long)(head)+sizeof(short));

                begin = end + 1;
                end = dataset[i].find(",", begin);
            }
            str = clear_invalid(dataset[i].substr(begin, end - begin));
            short tmp = str_to_int(str);
            memcpy(head, &tmp, sizeof(short));

            head = (void*)((long long)(head)+sizeof(short));
        }

        if (type == ".byte")
        {
            begin = 0;
            end = dataset[i].find(",", begin);
            while (end != -1)
            {
                str = clear_invalid(dataset[i].substr(begin, end - begin));
                char tmp = str_to_int(str);
                memcpy(head, &tmp, sizeof(int));

                head = (void*)((long long)(head)+sizeof(int));

                begin = end + 1;
                end = dataset[i].find(",", begin);
            }
            str = clear_invalid(dataset[i].substr(begin, end - begin));
            char tmp = str_to_int(str);
            memcpy(head, &tmp, sizeof(char));

            head = (void*)((long long)(head)+sizeof(char));
        }
    }
    dataend = head;
}


void load_text(void* head)
{
    int i, mips;
    string binary = MIPS_to_Binary(MIPS);
    for (i = 0; i < MIPSnum; i++)
    {
        mips = mips_to_int(binary.substr(i * 32, 32));
        memcpy(head, &mips, sizeof(int));
        head = (void*)((long long)(head)+sizeof(int));
    }
}


void init()
{
    ifstream input;
    string inputfilename;

    cout << "Please enter the input filename: ";
    cin >> inputfilename;
    input.open(inputfilename.c_str());

    while (input.fail())
    {
        input.clear();
        cout << "Cannot open the file \"" << inputfilename << "\".\nPlease enter the input filename again: ";
        cin >> inputfilename;
        input.open(inputfilename.c_str());
    }

    cout << "Open the file \"" << inputfilename << "\" successfully.\n";

    int code_type = text_type, len, end;
    string mips, func;

    while (getline(input, mips))
    {
        len = mips.size();
        end = mips.find("#");

        if (end != -1)  // Erase comments.
            mips.erase(end, len - end);

        mips = clear_invalid(mips);

        if (mips.empty()) //Jump this line if it is empty.
            continue;

        if (mips == ".data")
        {
            code_type = data_type;
            continue;
        }

        if (mips == ".text")
        {
            code_type = text_type;
            continue;
        }

        if (code_type == text_type)
        {
            MIPSnum++;
            MIPS.push_back(mips);
        }

        if (code_type == data_type)
        {
            datanum++;
            dataset.push_back(mips);

        }
    }

    input.close();
    dataend = (void*)((long long)(start)+data_address - text_address);
    *(int*)reg = 0;
    *(int*)((long long)reg + 28 * sizeof(int)) = data_address;
    *(int*)((long long)reg + 29 * sizeof(int)) = text_address + memory_size - sizeof(int);
}


int code_type(string mips_code)
{
    string id = mips_code.substr(0, 6);
    string func = mips_code.substr(26, 6);
    if (mips_code == "00000000000000000000000000001100")
        return S;
    if (id == "000000")
    {
        if (func == "001001" || func == "001000")
            return J;
        else
            return R;
    }
    else
    {
        if (id == "000010" || id == "000011")
            return J;
        else
            return I;
    }
}


void add(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int rd = mips_to_int(mips.substr(16, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    void* rdad = (void*)((long long)(reg)+rd * sizeof(int));
    if ((*(int*)rsad > 0 && *(int*)rtad > 0 && *(int*)rsad + *(int*)rtad < 0) || (*(int*)rsad < 0 && *(int*)rtad < 0 && *(int*)rsad + *(int*)rtad > 0))
    {
        cout << "Failed: Overflow Exception";
        exit(0);
    }
    *(int*)rdad = *(int*)rsad + *(int*)rtad;
}


void addu(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int rd = mips_to_int(mips.substr(16, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    void* rdad = (void*)((long long)(reg)+rd * sizeof(int));
    *(unsigned int*)rdad = *(unsigned int*)rsad + *(unsigned int*)rtad;
}


void sub(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int rd = mips_to_int(mips.substr(16, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    void* rdad = (void*)((long long)(reg)+rd * sizeof(int));
    if ((*(int*)rsad < 0 && *(int*)rtad > 0 && *(int*)rsad - *(int*)rtad > 0) || (*(int*)rsad > 0 && *(int*)rtad < 0 && *(int*)rsad - *(int*)rtad < 0))
    {
        cout << "Failed: Overflow Exception";
        exit(0);
    }
    *(int*)rdad = *(int*)rsad - *(int*)rtad;
}


void subu(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int rd = mips_to_int(mips.substr(16, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    void* rdad = (void*)((long long)(reg)+rd * sizeof(int));
    *(unsigned int*)rdad = *(unsigned int*)rsad - *(unsigned int*)rtad;
}


void div(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    void* hi = (void*)((long long)(reg)+32 * sizeof(int));
    void* lo = (void*)((long long)(reg)+33 * sizeof(int));
    *(unsigned int*)lo = *(unsigned int*)rsad / *(unsigned int*)rtad;
    *(unsigned int*)hi = *(unsigned int*)rsad % *(unsigned int*)rtad;
}


void divu(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    void* hi = (void*)((long long)(reg)+32 * sizeof(int));
    void* lo = (void*)((long long)(reg)+33 * sizeof(int));
    *(unsigned int*)lo = *(unsigned int*)rsad / *(unsigned int*)rtad;
    *(unsigned int*)hi = *(unsigned int*)rsad % *(unsigned int*)rtad;
}


void mlt(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    void* hi = (void*)((long long)(reg)+32 * sizeof(int));
    void* lo = (void*)((long long)(reg)+33 * sizeof(int));
    long long res = (long long)((*(int*)rsad)) * (long long)((*(int*)rtad));
    *(unsigned int*)hi = (unsigned int)(abs(res >> 32));
    *(unsigned int*)lo = (unsigned int)(res & 4294967295);
}


void mltu(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    void* hi = (void*)((long long)(reg)+32 * sizeof(int));
    void* lo = (void*)((long long)(reg)+33 * sizeof(int));
    unsigned long long res = (unsigned long long)(*(int*)rsad) * (unsigned long long)(*(int*)rtad);
    *(unsigned int*)hi = (unsigned int)(res >> 32);
    *(unsigned int*)lo = (unsigned int)(res & 4294967295);
}


void _and(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int rd = mips_to_int(mips.substr(16, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    void* rdad = (void*)((long long)(reg)+rd * sizeof(int));
    *(int*)rdad = *(int*)rsad & *(int*)rtad;
}


void _or(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int rd = mips_to_int(mips.substr(16, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    void* rdad = (void*)((long long)(reg)+rd * sizeof(int));
    *(int*)rdad = *(int*)rsad | *(int*)rtad;
}


void _nor(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int rd = mips_to_int(mips.substr(16, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    void* rdad = (void*)((long long)(reg)+rd * sizeof(int));
    *(int*)rdad = !(*(int*)rsad | *(int*)rtad);
}


void _xor(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int rd = mips_to_int(mips.substr(16, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    void* rdad = (void*)((long long)(reg)+rd * sizeof(int));
    *(int*)rdad = *(int*)rsad ^ *(int*)rtad;
}


void sll(string mips)
{
    int rt = mips_to_int(mips.substr(11, 5));
    int rd = mips_to_int(mips.substr(16, 5));
    int shamt = mips_to_int(mips.substr(21, 5));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    void* rdad = (void*)((long long)(reg)+rd * sizeof(int));
}


void sllv(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int rd = mips_to_int(mips.substr(16, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    void* rdad = (void*)((long long)(reg)+rd * sizeof(int));
    *(int*)rdad = *(int*)rtad << *(int*)rsad;
}


void sra(string mips)
{
    int rt = mips_to_int(mips.substr(11, 5));
    int rd = mips_to_int(mips.substr(16, 5));
    int shamt = mips_to_int(mips.substr(21, 5));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    void* rdad = (void*)((long long)(reg)+rd * sizeof(int));
    *(int*)rdad = *(int*)rtad >> shamt;
}


void srav(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int rd = mips_to_int(mips.substr(16, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    void* rdad = (void*)((long long)(reg)+rd * sizeof(int));
    *(int*)rdad = *(int*)rtad >> *(int*)rsad;
}


void srl(string mips)
{
    int rt = mips_to_int(mips.substr(11, 5));
    int rd = mips_to_int(mips.substr(16, 5));
    int shamt = mips_to_int(mips.substr(21, 5));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    void* rdad = (void*)((long long)(reg)+rd * sizeof(int));
    *(int*)rdad = abs(*(int*)rtad >> shamt);
}


void srlv(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int rd = mips_to_int(mips.substr(16, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    void* rdad = (void*)((long long)(reg)+rd * sizeof(int));
    *(int*)rdad = abs(*(int*)rtad >> *(int*)rsad);
}


void slt(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int rd = mips_to_int(mips.substr(16, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    void* rdad = (void*)((long long)(reg)+rd * sizeof(int));
    if (*(int*)rtad < *(int*)rsad)
        *(int*)rdad = 1;
    else
        *(int*)rdad = 0;
}


void sltu(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int rd = mips_to_int(mips.substr(16, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    void* rdad = (void*)((long long)(reg)+rd * sizeof(int));
    if (*(unsigned int*)rtad < *(unsigned int*)rsad)
        *(unsigned int*)rdad = 1;
    else
        *(unsigned int*)rdad = 0;
}


void teq(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    if (*(int*)rtad == *(int*)rsad)
    {
        cout << "Failed: Trap Exception";
        exit(0);
    }
}


void tne(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    if (*(int*)rtad != *(int*)rsad)
    {
        cout << "Failed: Trap Exception";
        exit(0);
    }
}


void tge(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    if (*(int*)rtad >= *(int*)rsad)
    {
        cout << "Failed: Trap Exception";
        exit(0);
    }
}


void tgeu(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    if (*(unsigned int*)rtad >= *(unsigned int*)rsad)
    {
        cout << "Failed: Trap Exception";
        exit(0);
    }
}


void tlt(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    if (*(int*)rtad < *(int*)rsad)
    {
        cout << "Failed: Trap Exception";
        exit(0);
    }
}


void tltu(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    if (*(unsigned int*)rtad < *(unsigned int*)rsad)
    {
        cout << "Failed: Trap Exception";
        exit(0);
    }
}


void mfhi(string mips)
{
    int rd = mips_to_int(mips.substr(16, 5));
    void* rdad = (void*)((long long)(reg)+rd * sizeof(int));
    void* hi = (void*)((long long)(reg)+32 * sizeof(int));
    *(int*)rdad = *(int*)(hi);
}


void mflo(string mips)
{
    int rd = mips_to_int(mips.substr(16, 5));
    void* rdad = (void*)((long long)(reg)+rd * sizeof(int));
    void* lo = (void*)((long long)(reg)+33 * sizeof(int));
    *(int*)rdad = *(int*)(lo);
}


void mthi(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* hi = (void*)((long long)(reg)+32 * sizeof(int));
    *(int*)(hi) = *(int*)rsad;
}


void mtlo(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* lo = (void*)((long long)(reg)+33 * sizeof(int));
    *(int*)(lo) = *(int*)rsad;
}


void Rfunction(string mips, void* head)
{
    string func_code = mips.substr(26, 6);
    int func = mips_to_int(func_code);
    switch (func)                                               // Process the commands with different types in R format.
    {
    case 0:
        sll(mips);
        break;
    case 4:
        sllv(mips);
        break;
    case 3:
        sra(mips);
        break;
    case 7:
        srav(mips);
        break;
    case 2:
        srl(mips);
        break;
    case 6:
        srlv(mips);
        break;
    case 26:
        div(mips);
        break;
    case 27:
        divu(mips);
        break;
    case 32:
        add(mips);
        break;
    case 33:
        addu(mips);
        break;
    case 34:
        sub(mips);
        break;
    case 35:
        subu(mips);
        break;
    case 36:
        _and(mips);
        break;
    case 37:
        _or(mips);
        break;
    case 38:
        _xor(mips);
        break;
    case 39:
        _nor(mips);
        break;
    case 42:
        slt(mips);
        break;
    case 43:
        sltu(mips);
        break;
    case 52:
        teq(mips);
        break;
    case 54:
        tne(mips);
        break;
    case 48:
        tge(mips);
        break;
    case 49:
        tgeu(mips);
        break;
    case 50:
        tlt(mips);
        break;
    case 51:
        tltu(mips);
        break;
    case 16:
        mfhi(mips);
        break;
    case 18:
        mflo(mips);
        break;
    case 17:
        mthi(mips);
        break;
    case 19:
        mtlo(mips);
        break;
    }
}


void addi(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int imm = mips_to_int(mips.substr(17, 16));
    if (mips[16] == '1')
        imm *= -1;
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    if ((*(int*)rsad > 0 && imm > 0 && *(int*)rsad + imm < 0) || (*(int*)rsad < 0 && imm < 0 && *(int*)rsad + imm > 0))
    {
        cout << "Overflow Exception";
        exit(0);
    }
    *(int*)rtad = *(int*)rsad + imm;
}


void addiu(string mips)
{
    unsigned int rs = mips_to_int(mips.substr(6, 5));
    unsigned int rt = mips_to_int(mips.substr(11, 5));
    unsigned int imm = mips_to_int(mips.substr(16, 16));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    *(unsigned int*)rtad = *(unsigned int*)rsad + imm;
}


void andi(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int imm = mips_to_int(mips.substr(16, 16));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    *(int*)rtad = *(int*)rsad & imm;
}


void ori(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int imm = mips_to_int(mips.substr(16, 16));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    *(int*)rtad = *(int*)rsad | imm;
}


void xori(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int imm = mips_to_int(mips.substr(16, 16));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    *(int*)rtad = *(int*)rsad ^ imm;
}


void lui(string mips)
{
    int rt = mips_to_int(mips.substr(11, 5));
    int imm = mips_to_int(mips.substr(17, 16));
    if (mips[16] == '1')
        imm *= -1;
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    *(int*)rtad = imm << 16;
}


void teqi(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int imm = mips_to_int(mips.substr(17, 16));
    if (mips[16] == '1')
        imm *= -1;
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    if (*(int*)rsad == imm)
    {
        cout << "Failed: Trap Exception";
        exit(0);
    }
}


void tnei(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int imm = mips_to_int(mips.substr(17, 16));
    if (mips[16] == '1')
        imm *= -1;
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    if (*(int*)rsad != imm)
    {
        cout << "Failed: Trap Exception";
        exit(0);
    }
}


void tgei(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int imm = mips_to_int(mips.substr(17, 16));
    if (mips[16] == '1')
        imm *= -1;
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    if (*(int*)rsad >= imm)
    {
        cout << "Failed: Trap Exception";
        exit(0);
    }
}


void tgeiu(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int imm = mips_to_int(mips.substr(16, 16));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    if (*(unsigned int*)rsad >= imm)
    {
        cout << "Failed: Trap Exception";
        exit(0);
    }
}


void tlti(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int imm = mips_to_int(mips.substr(17, 16));
    if (mips[16] == '1')
        imm *= -1;
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    if (*(int*)rsad < imm)
    {
        cout << "Failed: Trap Exception";
        exit(0);
    }
}


void tltiu(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int imm = mips_to_int(mips.substr(16, 16));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    if (*(unsigned int*)rsad < imm)
    {
        cout << "Failed: Trap Exception";
        exit(0);
    }
}


void slti(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int imm = mips_to_int(mips.substr(16, 16));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    if (*(int*)rsad < imm)
        *(int*)rtad = 1;
    else
        *(int*)rtad = 0;
}


void sltiu(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int imm = mips_to_int(mips.substr(16, 16));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    if (*(unsigned int*)rsad < imm)
        *(unsigned int*)rtad = 1;
    else
        *(unsigned int*)rtad = 0;
}


int bgez(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int offset;
    if (mips[16] == '1')
        offset = -1 * (mips_to_int(mips.substr(17, 15), true) + 1);
    else
        offset = mips_to_int(mips.substr(17, 15));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    if (*(int*)rsad >= 0)
        return offset;
    else
        return 0;
}


int bgezal(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int offset;
    if (mips[16] == '1')
        offset = -1 * (mips_to_int(mips.substr(17, 15), true) + 1);
    else
        offset = mips_to_int(mips.substr(17, 15));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    if (*(int*)rsad >= 0)
        return offset;
    else
        return 0;
}


int bgtz(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int offset;
    if (mips[16] == '1')
        offset = -1 * (mips_to_int(mips.substr(17, 15), true) + 1);
    else
        offset = mips_to_int(mips.substr(17, 15));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    if (*(int*)rsad > 0)
        return offset;
    else
        return 0;
}


int beq(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int offset;
    if (mips[16] == '1')
        offset = -1 * (mips_to_int(mips.substr(17, 15), true) + 1);
    else
        offset = mips_to_int(mips.substr(17, 15));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    if (*(int*)rsad == *(int*)rtad)
        return offset;
    else
        return 0;
}


int bne(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int offset;
    if (mips[16] == '1')
        offset = -1 * (mips_to_int(mips.substr(17, 15), true) + 1);
    else
        offset = mips_to_int(mips.substr(17, 15));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    if (*(int*)rsad != *(int*)rtad)
        return offset;
    else
        return 0;
}


int blez(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int offset;
    if (mips[16] == '1')
        offset = -1 * (mips_to_int(mips.substr(17, 15), true) + 1);
    else
        offset = mips_to_int(mips.substr(17, 15));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    if (*(int*)rsad <= 0)
        return offset;
    else
        return 0;
}


int bltzal(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int offset;
    if (mips[16] == '1')
        offset = -1 * (mips_to_int(mips.substr(17, 15), true) + 1);
    else
        offset = mips_to_int(mips.substr(17, 15));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    if (*(int*)rsad < 0)
        return offset;
    else
        return 0;
}


int bltz(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int offset;
    if (mips[16] == '1')
        offset = -1 * (mips_to_int(mips.substr(17, 15), true) + 1);
    else
        offset = mips_to_int(mips.substr(17, 15));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    if (*(int*)rsad < 0)
        return offset;
    else
        return 0;
}


void lb(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int offset;
    if (mips[16] == '1')
        offset = -1 * (mips_to_int(mips.substr(17, 15), true) + 1);
    else
        offset = mips_to_int(mips.substr(17, 15));
    void** rsad = (void**)((long long)(reg)+rs * sizeof(int) + offset * sizeof(char));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    memcpy((void*)(rtad), (char*)((long long)(*(char**)rsad) + (long long)start - text_address), sizeof(char));
}


void lbu(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int offset;
    if (mips[16] == '1')
        offset = -1 * (mips_to_int(mips.substr(17, 15), true) + 1);
    else
        offset = mips_to_int(mips.substr(17, 15));
    void** rsad = (void**)((long long)(reg)+rs * sizeof(int) + offset * sizeof(char));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    *(int*)rtad = **(unsigned short**)rsad;
    memcpy((void*)(rtad), (char*)((long long)(*(char**)rsad) + (long long)start - text_address), sizeof(char));
}


void lh(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int offset;
    if (mips[16] == '1')
        offset = -1 * (mips_to_int(mips.substr(17, 15), true) + 1);
    else
        offset = mips_to_int(mips.substr(17, 15));
    void** rsad = (void**)((long long)(reg)+rs * sizeof(int) + offset * sizeof(char));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    memcpy((void*)(rtad), (short*)((long long)(*(short**)rsad) + (long long)start - text_address), 2 * sizeof(char));
}


void lhu(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int offset;
    if (mips[16] == '1')
        offset = -1 * (mips_to_int(mips.substr(17, 15), true) + 1);
    else
        offset = mips_to_int(mips.substr(17, 15));
    void** rsad = (void**)((long long)(reg)+rs * sizeof(int) + offset * sizeof(char));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    memcpy((void*)(rtad), (unsigned short*)((long long)(*(unsigned short**)rsad) + (long long)start - text_address), 2 * sizeof(char));
}


void lw(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int offset;
    if (mips[16] == '1')
        offset = -1 * (mips_to_int(mips.substr(17, 15), true) + 1);
    else
        offset = mips_to_int(mips.substr(17, 15));
    void** rsad = (void**)((long long)(reg)+rs * sizeof(int) + offset * sizeof(char));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    *(int*)rtad = *(int*)((long long)(*(int**)rsad) + (long long)start - text_address);
}


void lwl(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int offset;
    if (mips[16] == '1')
        offset = -1 * (mips_to_int(mips.substr(17, 15), true) + 1);
    else
        offset = mips_to_int(mips.substr(17, 15));
    void** rsad = (void**)((long long)(reg)+rs * sizeof(int) + offset * sizeof(char));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    *(short*)rtad = abs(*(int*)((long long)(*(int**)rsad) + (long long)start - text_address) >> 16);
}


void lwr(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int offset;
    if (mips[16] == '1')
        offset = -1 * (mips_to_int(mips.substr(17, 15), true) + 1);
    else
        offset = mips_to_int(mips.substr(17, 15));
    void** rsad = (void**)((long long)(reg)+rs * sizeof(int) + offset * sizeof(char));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    *(short*)rtad = *(int*)((long long)(*(int**)rsad) + (long long)start - text_address) & 65535;
}


void ll(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int offset;
    if (mips[16] == '1')
        offset = -1 * (mips_to_int(mips.substr(17, 15), true) + 1);
    else
        offset = mips_to_int(mips.substr(17, 15));
    void** rsad = (void**)((long long)(reg)+rs * sizeof(int) + offset * sizeof(char));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    *(int*)rtad = *(int*)((long long)(*(int**)rsad) + (long long)start - text_address);
}


void sb(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int offset;
    if (mips[16] == '1')
        offset = -1 * (mips_to_int(mips.substr(17, 15), true) + 1);
    else
        offset = mips_to_int(mips.substr(17, 15));
    void** rsad = (void**)((long long)(reg)+rs * sizeof(int) + offset * sizeof(char));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    memcpy((char*)((long long)(*(char**)rsad) + (long long)(start)-text_address), (void*)(rtad), sizeof(char));

}


void sh(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int offset;
    if (mips[16] == '1')
        offset = -1 * (mips_to_int(mips.substr(17, 15), true) + 1);
    else
        offset = mips_to_int(mips.substr(17, 15));
    void** rsad = (void**)((long long)(reg)+rs * sizeof(int) + offset * sizeof(char));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    memcpy((short*)((long long)(*(short**)rsad) + (long long)(start)-text_address), (void*)(rtad), 2 * sizeof(char));
}


void sw(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int offset;
    if (mips[16] == '1')
        offset = -1 * (mips_to_int(mips.substr(17, 15), true) + 1);
    else
        offset = mips_to_int(mips.substr(17, 15));
    void** rsad = (void**)((long long)(reg)+rs * sizeof(int) + offset * sizeof(char));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    *(int*)((long long)(*(int**)rsad) + (long long)start - text_address) = *(int*)rtad;
}


void swl(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int offset;
    if (mips[16] == '1')
        offset = -1 * (mips_to_int(mips.substr(17, 15), true) + 1);
    else
        offset = mips_to_int(mips.substr(17, 15));
    void** rsad = (void**)((long long)(reg)+rs * sizeof(int) + offset * sizeof(char));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    *(short*)((long long)(*(short**)rsad) + (long long)start - text_address) = abs(**(int**)rtad >> 16);
}


void swr(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int offset;
    if (mips[16] == '1')
        offset = -1 * (mips_to_int(mips.substr(17, 15), true) + 1);
    else
        offset = mips_to_int(mips.substr(17, 15));
    void** rsad = (void**)((long long)(reg)+rs * sizeof(int) + offset * sizeof(char));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    *(short*)((long long)(*(short**)rsad) + (long long)start - text_address) = **(int**)rtad & 65535;
}


void sc(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rt = mips_to_int(mips.substr(11, 5));
    int offset;
    if (mips[16] == '1')
        offset = -1 * (mips_to_int(mips.substr(17, 15), true) + 1);
    else
        offset = mips_to_int(mips.substr(17, 15));
    void** rsad = (void**)((long long)(reg)+rs * sizeof(int) + offset * sizeof(char));
    void* rtad = (void*)((long long)(reg)+rt * sizeof(int));
    *(int*)((long long)(*(int**)rsad) + (long long)start - text_address) = *(int*)rtad;
    *(int*)rtad = 1;
}


int Ifunction(string mips, void* head)
{
    string func_code = mips.substr(0, 6);
    string func_code2 = mips.substr(11, 5);
    int s = 0, func = mips_to_int(func_code), func2 = mips_to_int(func_code2);
    void* ra = (void*)((long long)(reg)+31 * sizeof(int));

    switch (func)
    {
    case 1:
        if (func2 == 12)
            teqi(mips);
        if (func2 == 14)
            tnei(mips);
        if (func2 == 8)
            tgei(mips);
        if (func2 == 9)
            tgeiu(mips);
        if (func2 == 10)
            tlti(mips);
        if (func2 == 11)
            tltiu(mips);
        if (func2 == 1)
            s = bgez(mips);
        if (func2 == 17)
        {
            s = bgezal(mips);
            *(int*)ra = (long long)(head)+sizeof(int);
        }
        if (func2 == 0)
            s = bltz(mips);
        if (func2 == 16)
        {
            s = bltzal(mips);
            *(int*)ra = (long long)(head)+sizeof(int);
        }
        break;
    case 4:
        s = beq(mips);
        break;
    case 5:
        s = bne(mips);
        break;
    case 6:
        s = blez(mips);
        break;
    case 7:
        s = bgtz(mips);
        break;
    case 8:
        addi(mips);
        break;
    case 9:
        addiu(mips);
        break;
    case 10:
        slti(mips);
        break;
    case 11:
        sltiu(mips);
        break;
    case 12:
        andi(mips);
        break;
    case 13:
        ori(mips);
        break;
    case 14:
        xori(mips);
        break;
    case 15:
        lui(mips);
        break;
    case 32:
        lb(mips);
        break;
    case 33:
        lh(mips);
        break;
    case 34:
        lwl(mips);
        break;
    case 35:
        lw(mips);
        break;
    case 36:
        lbu(mips);
        break;
    case 37:
        lhu(mips);
        break;
    case 38:
        lwr(mips);
        break;
    case 48:
        ll(mips);
        break;
    case 40:
        sb(mips);
        break;
    case 41:
        sh(mips);
        break;
    case 43:
        sw(mips);
        break;
    case 42:
        swl(mips);
        break;
    case 46:
        swr(mips);
        break;
    case 56:
        sc(mips);
        break;
    }
    return s;
}


int _j(string mips)
{
    int tar = mips_to_int(mips.substr(6, 26));
    return  tar;
}


int jal(string mips, void* head)
{
    int tar = mips_to_int(mips.substr(6, 26));
    void* ra = (void*)((long long)(reg)+31 * sizeof(int));
    *(int*)ra = (long long)(head)-(long long)(start)+text_address + sizeof(int);
    return  tar;
}


int jalr(string mips, void* head)
{
    int rs = mips_to_int(mips.substr(6, 5));
    int rd = mips_to_int(mips.substr(16, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    void* rdad = (void*)((long long)(reg)+rd * sizeof(int));
    *(int*)rdad = (long long)(head)-(long long)(start)+text_address + sizeof(int);
    return (*(int*)(rsad)-text_address) / sizeof(int);
}


int jr(string mips)
{
    int rs = mips_to_int(mips.substr(6, 5));
    void* rsad = (void*)((long long)(reg)+rs * sizeof(int));
    return (*(int*)(rsad)-text_address) / sizeof(int);
}


int Jfunction(string mips, void* head)
{
    string func_code = mips.substr(0, 6);
    string func_code2 = mips.substr(26, 6);
    int s = 0, func = mips_to_int(func_code), func2 = mips_to_int(func_code2);

    switch (func)
    {
    case 0:
        if (func2 == 9)
            s = jalr(mips, head);
        if (func2 == 8)
            s = jr(mips);
        break;
    case 2:
        s = _j(mips);
        break;
    case 3:
        s = jal(mips, head);
        break;
    }
    return s;
}


void print_int()
{
    void* a0 = (void*)((long long)(reg)+4 * sizeof(int));
    cout << *(int*)a0 << "\n";
}


void print_string()
{
    void** a0 = (void**)((long long)(reg)+4 * sizeof(int));
    char* st = (char*)((long long)(*(char**)a0) + (long long)(start)-text_address);
    while (*st != '\0')
    {
        cout << *st;
        st = (char*)((long long)st + sizeof(char));
    }
    cout << "\n";
}

void print_char()
{
    void* a0 = (void*)((long long)(reg)+4 * sizeof(int));
    cout << *(char*)a0 << "\n";
}


void read_int()
{
    int tmp;
    void* v0 = (void*)((long long)(reg)+2 * sizeof(int));
    cin >> tmp;
    *(int*)v0 = tmp;
}


void read_char()
{
    char tmp;
    void* v0 = (void*)((long long)(reg)+2 * sizeof(int));
    cin >> tmp;
    *(char*)v0 = tmp;
}


void read_string()
{
    string tmp;
    cin >> tmp;
    void** a0 = (void**)((long long)(reg)+4 * sizeof(int));
    void* a1 = (void*)((long long)(reg)+5 * sizeof(int));
    memcpy((char*)((long long)(*(char**)a0) + (long long)(start)-text_address), tmp.c_str(), min(int(tmp.size()), *(int*)a1 - 1));
    *(char*)((long long)(*(char**)a0) + (long long)(start)-text_address + *(int*)a1 - 1) = '\0';
}


void sbrk()
{
    void** v0 = (void**)((long long)(reg)+2 * sizeof(int));
    void* a0 = (void*)((long long)(reg)+4 * sizeof(int));
    *v0 = (void*)((long long)(dataend)-(long long)(start)+text_address);
    dataend = (void*)((long long)(dataend)+(*(int*)a0) * sizeof(char));
}


void sysopen()
{
    void* a0 = (void*)((long long)(reg)+4 * sizeof(int));
    void* a1 = (void*)((long long)(reg)+5 * sizeof(int));
    void* a2 = (void*)((long long)(reg)+6 * sizeof(int));
    char* st = (char*)((long long)(*(char**)a1) + (long long)(start)-text_address);
    int fd, f = *(int*)a1;
    string str = "";

    while (*st != '\0')
    {
        str = str + *st;
        st = (char*)((long long)st + sizeof(char));
    }
    if (f == 0)
        fd = open(str.c_str(), O_CREAT | O_RDONLY, 0777);
    if (f == 1)
        fd = open(str.c_str(), O_CREAT | O_WRONLY, 0777);
    if (f == 2)
        fd = open(str.c_str(), O_CREAT | O_RDWR, 0777);
    *(int*)a0 = fd;
}


void sysread()
{
    void* a0 = (void*)((long long)(reg)+4 * sizeof(int));
    void* a1 = (void*)((long long)(reg)+5 * sizeof(int));
    void* a2 = (void*)((long long)(reg)+6 * sizeof(int));
    int fd = *(int*)a0;
    int len = *(int*)a2;
    char* st = (char*)((long long)(*(char**)a1) + (long long)(start)-text_address);
    *(int*)a0 = read(fd, st, len);
}


void syswrite()
{
    void* a0 = (void*)((long long)(reg)+4 * sizeof(int));
    void* a1 = (void*)((long long)(reg)+5 * sizeof(int));
    void* a2 = (void*)((long long)(reg)+6 * sizeof(int));
    int fd = *(int*)a0;
    int len = *(int*)a2;
    char* st = (char*)((long long)(*(char**)a1) + (long long)(start)-text_address);
    *(int*)a0 = write(fd, st, len);
}


void sysclose()
{
    void* a0 = (void*)((long long)(reg)+4 * sizeof(int));
    int fd = *(int*)a0;
    close(fd);
}


void sysexit()
{
    exit(0);
}


void exit2()
{
    void* a0 = (void*)((long long)(reg)+4 * sizeof(int));
    exit(*(int*)a0);
}


void Sfunction(string mips, void* head)
{
    void* v0 = (void*)((long long)(reg)+2 * sizeof(int));
    switch (*(int*)v0)
    {
    case 1:
        print_int();
        break;
    case 4:
        print_string();
        break;
    case 5:
        read_int();
        break;
    case 8:
        read_string();
        break;
    case 9:
        sbrk();
        break;
    case 10:
        sysexit();
        break;
    case 11:
        print_char();
        break;
    case 12:
        read_char();
        break;
    case 13:
        sysopen();
        break;
    case 14:
        sysread();
        break;
    case 15:
        syswrite();
        break;
    case 16:
        sysclose();
        break;
    case 17:
        exit2();
        break;
    }
}


void work(void* head, void* reg)
{
    int t, s;
    string mips_code;
    while ((long long)head - (long long)start < MIPSnum * sizeof(int))
    {
        mips_code = int_to_mips(*(int*)head);
        t = code_type(mips_code);
        if (t == R)
            Rfunction(mips_code, head);
        if (t == I)
        {
            s = Ifunction(mips_code, head);
            head = (void*)((long long)(head)+s * sizeof(int));
        }
        if (t == J)
        {
            s = Jfunction(mips_code, head);
            head = (void*)((long long)(start)+(s - 1) * sizeof(int));
        }
        if (t == S)
            Sfunction(mips_code, head);
        head = (void*)((long long)(head)+sizeof(int));
    }
}


int main()
{
    void* head = malloc(memory_size);
    memset(head, 0, memory_size);
    reg = malloc(reg_size);
    memset(reg, 0, reg_size);
    start = head;

    init();
    load_data((void*)((long long)(head)+0x00100000));
    load_text(head);
    work(head, reg);

    free(head);
    free(reg);
    return 0;
}
