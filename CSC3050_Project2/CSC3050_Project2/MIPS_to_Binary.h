#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
using namespace std;

int Type(string func);

bool invalid(char ch);

string clear_invalid(string c);

string int_to_string(int n);

string decimal_to_binary(string num, int bit);

int Rclassify(string func);

string Rcommand(string c);

int Iclassify(string func);

string Icommand(string c, int line);

string Jcommand(string c);

string command(string c, int line);

void load_data();

/*
 * Main function
 */
string MIPS_to_Binary(vector<string> MIPS);

