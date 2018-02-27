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
        auto grp = groups_of_interest.find(group_id);
        if (grp == groups_of_interest.end())
        {
            groups_of_interest[group_id] = 1;
        }
        else
        {
            groups_of_interest[group_id]++;
        }
        temp[cust_idx].group_id = group_id;
        cust_idx++;
    }

    sort(temp.begin(),temp.end());
    
    // cout<<"customers read from file "<<filename <<":"<<endl;
    // for (auto& a:temp)
    //     a.show();

    cout <<"from file queues:"<<endl;
    for (auto & grp:groups_of_interest)
    {
        cout <<grp.first<<" ~ "<<grp.second<<endl;
    }
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
        int num_grps;
        iss >> num_grps;
        
        vector<int> groups;
        while (!iss.eof())
        {
            int temp;
            iss >> temp;
            groups.push_back(temp);
        }
        if (num_grps != groups.size())
            cout<<"num groups "<< groups.size() << " does not match expected " << num_grps<< "  !!!!!\n";
        temp[i] = Operator(groups, operator_id);
    }
    return temp;
}




void ReadDialOptions(string filename, DialInfo& dial_info)
{
    ifstream file(filename);
    string line;
    getline(file, line);
    istringstream iss(line);
    iss>>dial_info.dial_success_prob;
    iss>> dial_info.dial_mean_succ;
    iss>>dial_info.dial_mean_fail;
    if(VERBOSE)
    {
        printf("dial options: success_prob = %.3f, dial_mean=%.3f, dial_max=%.3f\n", (float)dial_info.dial_success_prob, (float)dial_info.dial_mean_succ, (float)dial_info.dial_mean_fail);
    }
}


GroupsAptrioriMeans ReadGroupMeans(string filename)
{
    ifstream file(filename);
    string line;
    int grp_nums;
    getline(file,line);
    
    istringstream iss(line);
    iss >> grp_nums;
    
    getline(file,line);
    iss.clear();
    iss.str(line);
    
    float extra_mean;
    iss >> extra_mean;

    map<int,float> temp;
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
        temp[group_id] = mean;
    }
    if (grp_nums != temp.size())
        cout <<"groups nums "<< temp.size()<<" not equals to expected "<<grp_nums<<endl;

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


