#ifndef TYPES_H
#define TYPES_H

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

#define SLOW 0
#define VERBOSE 0
#define EXTENDED_STAT 1

using namespace std;

class DialInfo
{
public:
    float dial_success_prob;
    float dial_mean_succ;
    float dial_mean_fail;
};

class ServiceTime
{
public:
    ServiceTime():
        time_before_call(0), time_after_call(0), time_to_service(0){};
    int time_before_call;
    int time_after_call;
    int time_to_service;
};
ServiceTime emulate_service_time(float dial_success_prob, float dial_mean, float dial_max, float call_mean);

class customer
{
public:
    customer (): group_id(-1) {};
    bool operator<(const customer& a) const {
        double diffSecs = difftime(this->enter_time_tm, a.enter_time_tm);
        return diffSecs <= 0;
    }
    void show()
        {
            cout << "group_id="<<group_id<<", ";
            struct tm* tmp = gmtime(&(enter_time_tm));
            cout << asctime(tmp);
        }
    time_t enter_time_tm;
    int group_id;
    string name;
};

class GroupsAptrioriMeans
{
public:
    map<int, float> means;
    float extra_mean;
GroupsAptrioriMeans(map<int,float> &_means, float _extra_mean): means(_means), extra_mean(_extra_mean){};
    float GetMean(int idx)
    {
        if (idx < 0)
        {
            cout <<"Idx < 0 in GetMean\n";
            exit(1);
        }
        if (idx >= means.size() || means[idx] <= 0.01)
            return extra_mean;
        else
            return means[idx];
    }
        
};

class Operator
{
    static int count;
public:
    Operator(vector<int> _ids = {}, string _operator_id = ""): is_free(0), time_to_free(0), group_ids(_ids), is_enabled(1), group_id(-1) {operator_id = _operator_id;};
    string operator_id;
    vector<int> group_ids;
    bool is_free;
    bool is_enabled;
    int time_to_free;
    int group_id;
    customer curr_cust;
    void show() const {
        cout << "Operator "<< operator_id<<": ";
        cout <<"time="<<time_to_free<< ", ";
        cout <<"\n";
    }
    void show_ext() const {
        cout << "Operator "<< operator_id<<": ";
        cout <<"time="<<time_to_free<< ", ";
        cout << "is_enabled="<<is_enabled<< ", ";
        cout <<"group_id="<<group_id<<", ";
        cout << "serves groups: [ ";
        for (const auto& a:group_ids)
            cout<<a<<" ";
        cout <<"]\n";
    }
    bool PossibleToService(customer& c)
        {
            for (const auto& g:group_ids)
                if (g == c.group_id)
                    return true;
            return false;
        }
    void start_new_customer(list<customer>& c, GroupsAptrioriMeans& grps, DialInfo dial_info)
        {
            if(VERBOSE)
                cout<<"starting new customer for operator "<<operator_id<<endl;
            for (auto a = c.begin(); a != c.end(); a++)
            {
                if (PossibleToService(*a))
                {
                    if(VERBOSE)
                    {
                        cout <<"chosen:";
                        a->show();
                        cout<<endl;
                    }
                    ServiceTime st = emulate_service_time(dial_info.dial_success_prob, dial_info.dial_mean_succ, dial_info.dial_mean_fail, grps.GetMean(a->group_id));
                    group_id = a->group_id;
		    curr_cust = *a;
                    time_to_free = st.time_to_service;
//                    printf("generated time_to_service (%f, %f, %f, %f) = %.3f\n", g_dial_success_prob, g_dial_mean, g_dial_max, grps[a->group_id], time_to_free);
                    c.erase(a);
                    return;
                }
            }
            is_enabled = false;
            if(VERBOSE)
                cout<<"disabling operator "<<operator_id<<endl;
        }
};

// class Group
// {
// public:
//     int group_id;
//     float call_mean;
// };



class Server
{
public:
Server(const vector<Operator>& ops, const GroupsAptrioriMeans &grps, const map<int,int> groups_of_interest, const DialInfo& _dial_info): operators(ops), groupMeans(grps), total_time(0), groups_times(groups_of_interest), groups_customers(groups_of_interest), dial_info(_dial_info)
    {
        for (auto& grp:groups_times)
        {
            grp.second = 0;
        }
    };
    vector<Operator> operators;
    DialInfo dial_info;
    GroupsAptrioriMeans groupMeans;
map<int,int> groups_customers;
    map<int,int> groups_times;
#if EXTENDED_STAT
    map<string,int> cust_times;
#endif
    int total_time;
    void show()
        {
            cout << "Server has following operators:\n";
            for (const auto& a:operators)
            {
                a.show();
            }
            cout << "Server has following queues:\n";
            for (const auto& a:groups_customers)
            {
                cout<<"group "<<a.first<<" ~ "<<a.second<<endl;
            }
        }
    void show_ext()
        {
            cout << "Server has following operators:\n";
            for (const auto& a:operators)
            {
                a.show_ext();
            }
        }
    void start(list<customer>& cs)
        {
            for (auto& op:operators)
            {
                op.start_new_customer(cs, groupMeans, dial_info);
            }
        }
    bool do_iteration(list<customer>& cs)
        {
            auto minimum = operators.begin();
            auto it = operators.begin();
            while (it != operators.end())
            {
                if (!minimum->is_enabled || (minimum->time_to_free > it->time_to_free && it->is_enabled))
                    minimum = it;
                it++;
            }
            if (minimum != operators.end() && minimum->is_enabled)
            {
                int time_to_reduce = minimum->time_to_free;
                for (auto& a:operators)
                {
                    if (a.is_enabled)
                        a.time_to_free -= time_to_reduce;
                }
                total_time += time_to_reduce;
                if (VERBOSE)
                    cout <<"total_time = " <<total_time<<endl;
                groups_customers[minimum->group_id]--;
                if (groups_customers[minimum->group_id] == 0)
                {
                    groups_times[minimum->group_id] = total_time;
#if EXTENDED_STAT
		    cust_times[minimum->curr_cust.name] = total_time;
#endif
//                    cout <<minimum->group_id <<" group is finished!"<<" its time = " << total_time<<"\n";
                }
                minimum->start_new_customer(cs, groupMeans,dial_info);
                return true;
            }
            return false;
            if (minimum == operators.end() || !minimum->is_enabled)
            {
                return false;
            }
            return true;
        }
    void GetGroupsStat(map<int,vector<int> >& stat, vector<int>& total_times, int idx)
    {
        for (auto& grp:groups_times)
        {
            stat[grp.first][idx] = grp.second;
        }
        total_times[idx] = total_time;
    }

#if EXTENDED_STAT
    void GetGroupsStat_cust(map<string,vector<int> >& stat, vector<int>& total_times, int idx)
    {
        for (auto& cust:cust_times)
        {
            stat[cust.first][idx] = cust.second;
        }
        total_times[idx] = total_time;
    }
#endif
};


#endif
