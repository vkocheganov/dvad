#include "io.h"
#include "types.h"
#include <sstream>
#include <iostream>
#include <math.h>
#include <vector>
#include <list>
#include <algorithm>
#include <stdio.h>
#include <time.h>
#include <string>
#include <fstream>

vector<customer> ReadCustomersDataBase(string filename, map<int,int>& groups_of_interest)
{
    ifstream file(filename);
    string line;
    int customers_num;
    getline(file,line);
    
    istringstream iss(line);
    iss >> customers_num;
    vector<customer> temp(customers_num);
    int cust_idx = 0;
    struct tm temp_tm;
    while (getline(file, line))
    {
        istringstream iss(line);
        string times_day,
            times_time;
        int group_id;
        iss >> times_day;

        iss >> times_time;
        times_day += " " + times_time;
//        cout <<times_day<<endl;
        
        strptime(times_day.c_str(), "%Y-%m-%d %H:%M:%S", &temp_tm);        
        
        temp[cust_idx].enter_time_tm = timegm(&temp_tm);

        iss >> group_id;
        if (group_id < 0)
        {
            cout << "wrong group_id!!"<<endl;
            exit(1);
        }
        groups_of_interest[group_id] = 1;
        temp[cust_idx].group_id = group_id;
        cust_idx++;
    }

    sort(temp.begin(),temp.end());
    
    // cout<<"customers read from file "<<filename <<":"<<endl;
    // for (auto& a:temp)
    //     a.show();
    return temp;
}


vector<Operator> read_operators(string op_filename)
{
    ifstream file(op_filename);
    int nops;
    string line;
    getline(file, line);
    istringstream iss(line);
    iss>>nops;
    vector<Operator> temp(nops);
    if(VERBOSE)
    {
        cout <<"reading operators"<<endl;
        cout <<"num operators = "<<nops<<endl;
    }
    for (int i = 0; i < nops; i++)
    {
        getline(file, line);
        istringstream iss(line);
        string operator_id;
        iss >> operator_id;
        
        vector<int> groups;
        while (!iss.eof())
        {
            int temp;
            iss >> temp;
            groups.push_back(temp);
        }
        temp[i] = Operator(groups, operator_id);
    }
    return temp;
}




void ReadDialOptions(string filename, float& success, float& dial_mean, float& dial_max)
{
    ifstream file(filename);
    string line;
    getline(file, line);
    istringstream iss(line);
    iss>>success;
    iss>>dial_mean;
    iss>>dial_max;
    if(VERBOSE)
    {
        printf("dial options: success_prob = %.3f, dial_mean=%.3f, dial_max=%.3f\n", (float)success, (float)dial_mean, (float)dial_max);
    }
}


GroupsAptrioriMeans ReadGroupMeans(string filename)
{
    ifstream file(filename);
    string line;
    int max_group_id;
    getline(file,line);
    
    istringstream iss(line);
    iss >> max_group_id;
    
    getline(file,line);
    iss.clear();
    iss.str(line);
    
    float extra_mean;
    iss >> extra_mean;

    vector<float> temp(max_group_id, 0.0);
    while (getline(file, line))
    {
        istringstream iss(line);
        int group_id;
        float mean;
        iss >> group_id;
        iss >> mean;
        if (group_id < 0)
        {
            cout << "wrong group_id!!"<<endl;
            exit(1);
        }
        if (mean <= 0 || mean >= 1000)
        {
            cout << "wrong group mean "<<mean <<endl;
            exit(1);
        }
        if (group_id >= temp.size())
        {
            cout<<"max id was wrong!\n";
            exit(1);
            // int old_size = temp.size();
            // cout <<  "resizing group_id array\n";
            // temp.resize(group_id + 1);
            // for (int i = old_size - 1; i < group_id; i++)
            // {
            //     temp[i] = 0;
            // }
        }
        temp[group_id] = mean;
    }

    cout<<"group means read from file "<<filename <<":"<<endl;
    for (int i = 0; i < temp.size(); i++)
        if (temp[i] >0)
            cout <<"("<<i<<", "<< temp[i]<<")"<<endl;

    cout <<endl;
    return GroupsAptrioriMeans(temp, extra_mean);
}







void read_groups_nums(string gr_filename, vector<int>& groups_num)
{
    if(VERBOSE)
        cout <<"reading groups\n";
    ifstream file(gr_filename);
    int ngrs;
    string line;
    
    getline(file, line);
    istringstream iss1(line);
    iss1>>ngrs;
    if(VERBOSE)
        cout <<"num_groups="<<ngrs<<endl;

    
    getline(file, line);
    istringstream iss2(line);
    groups_num.clear();
    if(VERBOSE)
        cout <<"nums: ";
    while (!iss2.eof())
    {
        int temp;
        iss2 >> temp;
        groups_num.push_back(temp);
        if(VERBOSE)
            cout <<temp<<" ";
    }
    if(VERBOSE)
        cout <<endl;
}

void generate_customers(string in_filename, string out_filename)
{
    vector<int> group_nums;
    read_groups_nums(in_filename, group_nums);
    
    ofstream file(out_filename);
    int total_size = accumulate(group_nums.begin(), group_nums.end(), 0);
    int shift = 0;
    int group_id = 0;
    file<<total_size<<endl;
    for (const auto& gn:group_nums)
    {
        for (int i = shift; i < shift + gn; i++ )
        {
            file<<"2018-01-02 " <<
                rand() % 24 << ":" <<
                rand() % 60 << ":" <<
                rand() % 60 << " ";
            file << group_id<<endl;
        }
        shift += gn;
        group_id++;
    }
}


