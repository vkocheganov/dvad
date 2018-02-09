#ifndef IO_H
#define OI_H

#include <vector>
#include <string>
#include "types.h"
using namespace std;

vector<customer> ReadCustomersDataBase(string filename);
vector<Operator> read_operators(string op_filename);
void ReadDialOptions(string filename, float& success, float& dial_mean, float& dial_max);
vector<float> ReadGroupMeans(string filename);
void generate_customers(string in_filename, string out_filename);
void read_groups_nums(string gr_filename, vector<int>& groups_num);

#endif
