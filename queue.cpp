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
using namespace std;

float g_dial_success_prob;
float g_dial_mean;
float g_dial_max;
float g_call_mean;

int Operator::count;
   
//expon with mean 1/lambda
float expon(float lambda, float u)
{
    return -(log(1 - u)/lambda);
}

ServiceTime emulate_service_time(float dial_success_prob, float dial_mean, float dial_max, float call_mean)
{
    ServiceTime st;
    float u = (rand() / float(RAND_MAX));
    if (u < dial_success_prob)
    {
        u = (rand() / float(RAND_MAX));
        st.time_before_call = expon(1/dial_mean,u);
        st.time_to_service += st.time_before_call;
                
        u = (rand() / float(RAND_MAX));
        st.time_after_call = expon(1/call_mean,u);
        st.time_to_service += st.time_after_call;
    }
    else
    {
        st.time_to_service = st.time_before_call = dial_max;
    }
    return st;
}

void Show_queue(list<customer> queue)
{
    for (const auto& a:queue)
    {
        cout << a.group_id<<" ";
    }
    cout <<endl;
}

void Show_queue_extended(list<customer> queue)
{
    int i = 0;
    for (const auto& a:queue)
    {
        printf("(%d, %d, %.4f) ",i, (int)a.group_id, (float)a.enter_time);
        i++;
    }

}

list<customer> create_queue(vector<customer> &c, bool need_sort)
{
    list<customer> temp;
    if (need_sort)
    {
        sort(c.begin(),c.end());
    }

    for (const auto& e: c)
    {
        temp.push_back(e);
    }
    return temp;
}


#define DIAL_SUCCESS_PROB 0.9
#define DIAL_MEAN 5
#define DIAL_MAX 15

const vector<int> GROUPS_NUM = {10, 30};
const vector<float> GROUPS_MEAN = {200, 200};

int main(int argc, char* argv[])
{
    srand(time(NULL));
    vector<Operator> ops;
    vector<int> groups_nums;
    vector<float> groups_means;
    float dial_success_prob,
        dial_mean,
        dial_max;
    int idx = 1;


    cout << "Usage:\n"<<
        "./a.out file_stat_1 file_stat_2 file_database_customers file_database_free_operators\n\n";
    
    string group_stat_file = argv[1],
        dial_stat_file = argv[2],
        customers_data_base_file = argv[3],
        operators_file = argv[4],
        groups_nums_file;
    if (argc > 5)
    {
        groups_nums_file = argv[5];
        generate_customers(groups_nums_file, customers_data_base_file);
    }
    ops = read_operators(operators_file);
    for (auto& a:ops)
        a.show_ext();
    

    return 0;

    vector<float> groupMeans = ReadGroupMeans(argv[1]);
    ReadDialOptions(argv[2], dial_success_prob, dial_mean, dial_max);
    vector<customer> customers = ReadCustomersDataBase(argv[3]);
    return 0;
    if (argc >= 4)
    {
        ops = read_operators(argv[1]);
//        read_groups(argv[2], groups_nums, groups_means);
    }
    else
    {
        // ops = OPERATORS;
        groups_nums = GROUPS_NUM;
        groups_means = GROUPS_MEAN;
        dial_success_prob = DIAL_SUCCESS_PROB;
        dial_mean = DIAL_MEAN;
        dial_max = DIAL_MAX;
    }
    g_dial_success_prob = dial_success_prob;
    g_dial_mean = dial_mean;
    g_dial_max = dial_max;
    g_call_mean = groups_means[0];

    if (SLOW)
        cin.get();
    // return 0;
    float total_time = 0, iteration_time;
//    for (int j = 0; j < 1000; j++)
    for (int j = 0; j < 1; j++)    
    {
        
        Server s(ops);
        vector<customer> temp_c;
        //= generate_customers(groups_nums, groups_means, dial_success_prob, dial_mean, dial_mean);
        list<customer> c = create_queue(temp_c,true);
        if(VERBOSE)
        {
            Show_queue_extended(c);
        }
    
        if(VERBOSE)
        {
            s.show();
        }
        if (SLOW)
            cin.get();
        s.start(c);
        if(VERBOSE)
        {
            s.show();
        }
        int temp;
        bool cont = true;
        int i = 0;
        total_time = 0;
        while (cont)
        {
            if(VERBOSE)
            {
                cout<<endl;
                cout<<"do_iteration "<<i<<endl;
            }
            iteration_time = -1;
            cont = s.do_iteration(c, iteration_time);
            if (iteration_time < 0)
            {
                if(VERBOSE)
                    cout <<"it time < 0!\n";
            }
            else
                total_time += iteration_time;
        
            if(VERBOSE)
            {
                printf("iteration time = %.3f, total_time = %.3f\n", iteration_time, total_time);
                s.show();
                Show_queue_extended(c);
                cout<<endl<<endl;
            }
            if (SLOW)
                cin.get();
            i++;
        }
    }
    cout <<"time spent = "<<total_time<<endl;
    return 0;
}
