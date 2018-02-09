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

extern float g_dial_success_prob;
extern float g_dial_mean;
extern float g_dial_max;
extern float g_call_mean;

using namespace std;

class ServiceTime
{
public:
    ServiceTime():
        time_before_call(0), time_after_call(0), time_to_service(0){};
    float time_before_call;
    float time_after_call;
    float time_to_service;
};
ServiceTime emulate_service_time(float dial_success_prob, float dial_mean, float dial_max, float call_mean);

class customer
{
public:
    customer (): enter_time(0), group_id(-1) {};
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
    float enter_time;
    time_t enter_time_tm;
    int group_id;
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
    float time_to_free;
    int group_id;
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
    void start_new_customer(list<customer>& c, const vector<float>& grps)
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
                    ServiceTime st = emulate_service_time(g_dial_success_prob, g_dial_mean, g_dial_max, grps[a->group_id]);
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

class Group
{
public:
    int group_id;
    float call_mean;
};


class Server
{
public:
Server(const vector<Operator>& ops, const vector<float>& grps): operators(ops), groupMeans(grps){};
    vector<Operator> operators;
    vector<float> groupMeans;
    void show()
        {
            cout << "Server has following operators:\n";
            for (const auto& a:operators)
            {
                a.show();
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
                op.start_new_customer(cs, groupMeans);
            }
        }
    bool do_iteration(list<customer>& cs, float& time)
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
                float time_to_reduce = minimum->time_to_free;
                for (auto& a:operators)
                {
                    if (a.is_enabled)
                        a.time_to_free -= time_to_reduce;
                }
                time = time_to_reduce;
                minimum->start_new_customer(cs, groupMeans);
                return true;
            }
            return false;
            if (minimum == operators.end() || !minimum->is_enabled)
            {
                return false;
            }
            return true;
        }
};

#endif
