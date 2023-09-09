#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
using namespace std;

/*
 * Paremeters definition.
 */
const int N = 40, M = 10;                               // The constants used to define capacity of arrays.
const int R = 0, I = 1, J = 2;                          // The constants used to identify different funtions.
const int Rnum = 32, Inum = 36, Jnum = 2, Renum = 32;   // The numbers of funtions.

int MIPSnum;                                            // The number of tags.

vector<string> MIPS;                                    // Commands.

map<string, int> tags;                                  // Tags.
map<string, string> Reg;                                // Register names and numbers.
map<string, string> Rm;                                 // R format functions.
map<string, string> Im;                                 // I format functions.
map<string, string> Jm;                                 // J format functions.

string Register[N]      = { "$zero", "$at",   "$v0",   "$v1",    "$a0",   "$a1",   "$a2",   "$a3",   "$t0",   "$t1",   "$t2",   "$t3",   "$t4",   "$t5",   "$t6",   "$t7",   "$s0",   "$s1",   "$s2",   "$s3",   "$s4",   "$s5",   "$s6",   "$s7",   "$t8",   "$t9",   "$k0",   "$k1",   "$gp",   "$sp",   "$fp",   "$ra"};
string Register_Code[N] = { "00000", "00001", "00010", "00011", "00100", "00101", "00110", "00111", "01000", "01001", "01010", "01011", "01100", "01101", "01110", "01111", "10000", "10001", "10010", "10011", "10100", "10101", "10110", "10111", "11000", "11001", "11010", "11011", "11100", "11101", "11110", "11111" };

vector<string> Rtype1 = { "add", "addu", "and", "nor", "or", "sub", "subu", "xor", "slt", "sltu"};
vector<string> Rtype2 = { "div", "divu", "mult", "multu", "teq", "tne", "tge", "tgeu", "tlt", "tltu" };
vector<string> Rtype3 = { "sllv", "srav", "srlv"};
vector<string> Rtype4 = { "jalr"};
vector<string> Rtype5 = { "jr", "mthi", "mtlo" };
vector<string> Rtype6 = { "mfhi", "mflo" };
vector<string> Rtype7 = { "sll", "sra", "srl" };

string Rtype[N]      = { "add",    "addu",   "and",    "div",    "divu",   "mult",   "multu",  "nor",    "or",     "sll",    "sllv",   "sra",    "srav",   "srl",    "srlv",   "sub",    "subu",   "xor",    "slt",    "sltu",   "teq",    "tne",    "tge",    "tgeu",   "tlt",    "tltu",   "mfhi",   "mflo",   "mthi",   "mtlo",   "jr",     "jalr" };
string Rtype_Code[N] = { "100000", "100001", "100100", "011010", "011011", "011000", "011001", "100111", "100101", "000000", "000100", "000011", "000111", "000010", "000110", "100010", "100011", "100110", "101010", "101011", "110100", "110110", "110000", "110001", "110010", "110011", "010000", "010010", "010001", "010011", "001000", "001001" };

vector<string> Itype1 = { "addi", "addiu", "andi", "ori", "xori", "slti", "sltiu"};
vector<string> Itype2 = { "teqi", "tnei", "tgei", "tgeiu", "tlti", "tltiu" };
vector<string> Itype3 = { "lui" };
vector<string> Itype4 = { "beq", "bne" };
vector<string> Itype5 = { "bgtz", "blez" };
vector<string> Itype6 = { "bgez", "bgezal", "bltzal", "bltz" };
vector<string> Itype7 = { "lb", "lbu", "lh", "lhu", "lw", "lwl", "lwr", "ll", "sb", "sh", "sw", "swl", "swr", "sc"};

string Itype[N]      = { "addi",   "addiu",  "andi",   "ori",    "xori",   "lui",    "slti",   "sltiu",  "beq",    "bgez",  "bgezal", "bgtz",   "blez",   "bltzal", "bltz",  "bne",    "teqi",  "tnei",  "tgei",  "tgeiu", "tlti",  "tltiu",  "lb",    "lbu",    "lh",     "lhu",    "lw",     "lwl",    "lwr",    "ll",     "sb",     "sh",     "sw",     "swl",    "swr",    "sc"};
string Itype_Code[N] = { "001000", "001001", "001100", "001101", "001110", "001111", "001010", "001011", "000100", "00001", "10001",  "000111", "000110", "10000",  "00000", "000101", "01100", "01110", "01000", "01001", "01010", "01011", "100000", "100100", "100001", "100101", "100011", "100010", "100110", "110000", "101000", "101001", "101011", "101010", "101110", "111000" };

string Jtype[N]      = { "j",     "jal" };
string Jtype_Code[N] = { "000010","000011" };


/*
 * Function: Type
 * Usage: int type = Type(func);
 * -----------------------------
 * Distinguish the R, I and J format instructions.
 */
int Type(string func)
{
    int t = 0;
    if (Rm.find(func) != Rm.end())
        t = R;
    if (Im.find(func) != Im.end())
        t = I;
    if (Jm.find(func) != Jm.end())
        t = J;
    return t;
}


/*
 * Function: invalid
 * Usage: invalid(mips[0]);
 * ------------------------
 * Delete the invalid characters.
 */
bool invalid(char ch)
{
    if (' ' == ch || '\r' == ch || '\n' == ch || '\t' == ch || '\f' == ch)
        return true;
    return false;
}


/*
 * Function: clear_invalid
 * Usage: op = clear_invalid(op);
 * ------------------------------
 * Erase invalid characters in the string.
 */
string clear_invalid(string c)
{
    while (!c.empty() && invalid(c[0]))
        c.erase(0, 1);
    while (!c.empty() && invalid(c[c.size() - 1]))
        c.erase(c.size() - 1, 1);
    return c;
}


/*
 * Function: int_to_string
 * Usage: line = int_to_string(tags[op]);
 * --------------------------------------
 * Convert a int variable to a string.
 */
string int_to_string(int n)
{
    string c = "";
    int sign = 0;
    if (n < 0)
    {
        sign = 1;
        n *= -1;
    }
    while (n)
    {
        c = char(n % 10 + 48) + c;
        n /= 10;
    }
    if (c == "")
        c = "0";
    if (sign == 1)
        c = "-" + c;
    return c;
}


/*
 * Function: decimal_to_binary
 * Usage: op_code.push_back(decimal_to_binary(op));
 * ------------------------------------------------
 * Convert a decimal number to a fixed-bit
 * binary number.
 */
string decimal_to_binary(string num, int bit)
{
    int i, n = 0, re, sign = 0, car;
    string b = "";

    if (num[0] == '-')  // Check the sign.
    {
        num.erase(0, 1);
        sign = 1;
    }

    for (i = 0; i < num.size(); i++)
        n = n * 10 + num[i] - 48;

    if (sign == 1 && n == 0)  // Deal with -0.
        sign = 0;

    while (n)
    {
        b = char(n%2 + 48) + b;
        n /= 2;
    }
    re = bit - b.size();
    for (i = 0; i < re; i++)
        b = "0" + b;

    if (sign)  // Convert to two's ccomplement form.
    {
        for (i = 0; i < bit; i++)  
            b[i] = char(97 - b[i]);
        i = bit - 1;
        car = 1;
        while (i >= 0 && car == 1)   // Plus 1.
        {
            if (b[i] == '0')
                car = 0;
            b[i] = char(97 - b[i]);
            i--;
        }
        b.replace(0, 1, "1");
    }
    return b;
}


/*
 * Function: Rclassify
 * Usage: switch (Rclassify(func))
 * -------------------------------
 * Classify R format commands.
 */
int Rclassify(string func)
{
    int t = 0;
    if (find(Rtype1.begin(), Rtype1.end(), func) != Rtype1.end())
        t = 1;
    if (find(Rtype2.begin(), Rtype2.end(), func) != Rtype2.end())
        t = 2;
    if (find(Rtype3.begin(), Rtype3.end(), func) != Rtype3.end())
        t = 3;
    if (find(Rtype4.begin(), Rtype4.end(), func) != Rtype4.end())
        t = 4;
    if (find(Rtype5.begin(), Rtype5.end(), func) != Rtype5.end())
        t = 5;
    if (find(Rtype6.begin(), Rtype6.end(), func) != Rtype6.end())
        t = 6;
    if (find(Rtype7.begin(), Rtype7.end(), func) != Rtype7.end())
        t = 7;
    return t;
}


/*
 * Function: Rcommand
 * Usage: tran = Rcommand(c);
 * --------------------------
 * Process single R format command in MIPS.
 */
string Rcommand(string c)
{
    int begin = 0, end = c.find(' ');
    string func = clear_invalid(c.substr(begin, end - begin));    // Extract the function name.
    string op;
    vector<string> op_code;
    string mach = "00000000000000000000000000000000";
    mach.replace(26, 6, Rm[func]);
    
    begin = end + 1;
    end = c.find(',', begin);

    while (end != string::npos)      // Extract the parameters.
    {
        op = clear_invalid(c.substr(begin, end - begin));
        if (op[0] == '$')
            op_code.push_back(Reg[op]); 
        else
            op_code.push_back(decimal_to_binary(op, 5));

        begin = end + 1;
        end = c.find(',', begin);
    }

    op = clear_invalid(c.substr(begin, c.size() - begin)); 
    if (op[0] == '$')
        op_code.push_back(Reg[op]); 
    else
        op_code.push_back(decimal_to_binary(op, 5));

    switch (Rclassify(func))        // Process the commands with different types in R format.
    {
    case 1:
        mach.replace(6, 5, op_code[1]);
        mach.replace(11, 5, op_code[2]);
        mach.replace(16, 5, op_code[0]);
        break;
    case 2:
        mach.replace(6, 5, op_code[0]);
        mach.replace(11, 5, op_code[1]);
        break;
    case 3:
        mach.replace(6, 5, op_code[2]);
        mach.replace(11, 5, op_code[1]);
        mach.replace(16, 5, op_code[0]);
        break;
    case 4:
        mach.replace(6, 5, op_code[0]);
        mach.replace(16, 5, op_code[1]);
        break;
    case 5:
        mach.replace(6, 5, op_code[0]);
        break;
    case 6:
        mach.replace(16, 5, op_code[0]);
        break;
    case 7:
        mach.replace(11, 5, op_code[1]);
        mach.replace(16, 5, op_code[0]);
        mach.replace(21, 5, op_code[2]);
        break;
    }

    return mach;
}


/*
 * Function: Iclassify
 * Usage: switch (Iclassify(func))
 * -------------------------------
 * Classify I format commands.
 */
int Iclassify(string func)
{
    int t = 0;
    if (find(Itype1.begin(), Itype1.end(), func) != Itype1.end())
        t = 1;
    if (find(Itype2.begin(), Itype2.end(), func) != Itype2.end())
        t = 2;
    if (find(Itype3.begin(), Itype3.end(), func) != Itype3.end())
        t = 3;
    if (find(Itype4.begin(), Itype4.end(), func) != Itype4.end())
        t = 4;
    if (find(Itype5.begin(), Itype5.end(), func) != Itype5.end())
        t = 5;
    if (find(Itype6.begin(), Itype6.end(), func) != Itype6.end())
        t = 6;
    if (find(Itype7.begin(), Itype7.end(), func) != Itype7.end())
        t = 7;
    return t;
}


/*
 * Function: Icommand
 * Usage: tran = Icommand(c, line);
 * --------------------------------
 * Process single I format command in MIPS.
 */
string Icommand(string c, int line)
{
    int begin = 0, end = c.find(' '), left, right;
    string func = clear_invalid(c.substr(begin, end - begin));    // Extract the function name.
    vector<string> op;
    string mach = "00000000000000000000000000000000";

    begin = end + 1;
    end = c.find(',', begin);

    while (end != string::npos)      // Extract the parameters.
    {
        op.push_back(clear_invalid(c.substr(begin, end - begin)));
        begin = end + 1;
        end = c.find(',', begin);
    }

    op.push_back(clear_invalid(c.substr(begin, c.size() - begin)));
    
    switch (Iclassify(func))        // Process the commands with different types in I format.
    {
    case 1:
        mach.replace(0, 6, Im[func]);
        mach.replace(6, 5, Reg[op[1]]);
        mach.replace(11, 5, Reg[op[0]]);
        mach.replace(16, 16, decimal_to_binary(op[2], 16));
        break;
    case 2:
        mach.replace(0, 6, "000001");
        mach.replace(6, 5, Reg[op[0]]);
        mach.replace(11, 5, Im[func]);
        mach.replace(16, 16, decimal_to_binary(op[1], 16));
        break;
    case 3:
        mach.replace(0, 6, Im[func]);
        mach.replace(11, 5, Reg[op[0]]);
        mach.replace(16, 16, decimal_to_binary(op[1], 16));
        break;
    case 4:
        mach.replace(0, 6, Im[func]);
        mach.replace(6, 5, Reg[op[0]]);
        mach.replace(11, 5, Reg[op[1]]);
        mach.replace(16, 16, decimal_to_binary(int_to_string(tags[op[2]] - line - 1), 16));
        break;
    case 5:
        mach.replace(0, 6, Im[func]);
        mach.replace(6, 5, Reg[op[0]]);
        mach.replace(16, 16, decimal_to_binary(int_to_string(tags[op[1]] - line - 1), 16));
        break;
    case 6:
        mach.replace(0, 6, "000001");
        mach.replace(6, 5, Reg[op[0]]);
        mach.replace(11, 5, Im[func]);
        mach.replace(16, 16, decimal_to_binary(int_to_string(tags[op[1]] - line - 1), 16));
        break;
    case 7:
        mach.replace(0, 6, Im[func]);
        mach.replace(11, 5, Reg[op[0]]);
        left = op[1].find('(', 0);
        right = op[1].find(')', 0);
        mach.replace(6, 5, Reg[clear_invalid(op[1].substr(left + 1, right - left - 1))]);
        mach.replace(16, 16, decimal_to_binary(clear_invalid(op[1].substr(0, left)), 16));
        break;
    }

    return mach;
}


/*
 * Function: Jcommand
 * Usage: tran = Jcommand(c);
 * --------------------------
 * Process single J format command in MIPS.
 */
string Jcommand(string c)
{
    int begin = 0, end = c.find(' ');
    string func = clear_invalid(c.substr(begin, end - begin));    // Extract the function name.
    string op, jline;
    string mach = "00000000000000000000000000000000";

    mach.replace(0, 6, Jm[func]);

    begin = end + 1;
    end = c.size();

    op = clear_invalid(c.substr(begin, end - begin));
    jline = int_to_string(tags[op]);

    mach.replace(6, 26, decimal_to_binary(jline, 26));

    return mach;
}


/*
 * Function: command
 * Usage: cout << command(MIPS[i]);
 * --------------------------------
 * Process single command in MIPS.
 */
string command(string c, int line)
{
    int end = c.find(' ');
    string func = clear_invalid(c.substr(0, end));
    string result = "";
    int type = Type(func);

    if (type == R)
        result = Rcommand(c);
    if (type == I)
        result = Icommand(c, line);
    if (type == J)
        result = Jcommand(c);
    return result;
}


/*
 * Function: init
 * Usage: init();
 * --------------
 * Read a MIPS file.
 * Preprocess the commandes for transformation.
 * Load in the function and register code. 
 */
void init()
{
    string inputfilename;
    cout << "Please enter the input filename: ";
    cin >> inputfilename;
    ifstream input(inputfilename.c_str());

    while (input.fail())
    {   
        input.clear();
        cout << "Cannot open the file "<< inputfilename <<".\nPlease enter the input filename again: ";
        cin >> inputfilename;
        input.open(inputfilename.c_str());
    }

    cout << "Open the file " << inputfilename << " successfully.\n";

    int i;
    string mips;

    while (getline(input, mips))
    {
        int len = mips.size();
        int end = mips.find("#");
        int tag_end;
        string tag;
        
        if (end != -1)  // Erase comments.
            mips.erase(end, len - end);

        mips = clear_invalid(mips);

        if (mips.empty()) //Jump this line if it is empty.
            continue;

        tag_end = mips.find(":");
        
        if (tag_end != -1)  // Record the tag.
        {
            tag = mips.substr(0, tag_end);
            tags.insert(pair<string, int>(tag, MIPSnum));
            mips.erase(0, tag_end+1);
        }

        MIPSnum++;
        MIPS.push_back(clear_invalid(mips));
    }

    for (i = 0; i < Rnum; i++)   // Load in R format functions.
        Rm.insert(pair<string, string>(Rtype[i], Rtype_Code[i]));

    for (i = 0; i < Inum; i++)   // Load in I format functions.
        Im.insert(pair<string, string>(Itype[i], Itype_Code[i]));

    for (i = 0; i < Jnum; i++)   // Load in J format functions.
        Jm.insert(pair<string, string>(Jtype[i], Jtype_Code[i]));

    for (i = 0; i < Renum; i++)   // Load in Register names and numbers.
        Reg.insert(pair<string, string>(Register[i], Register_Code[i]));

    input.close();
}


/*
 * Function: work
 * Usage: work();
 * --------------
 * Process all commands.
 */
void work()
{
    string outputfilename;

    cout << "Please enter the output filename: ";
    cin >> outputfilename;

    ofstream output(outputfilename);

    for (int i = 0; i < MIPSnum; i++)
        output << command(MIPS[i], i) << "\n";
}


/* 
 * Main function
 */
int main()
{
    init();
    work();
    return 0;
}
