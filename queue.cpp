// To run:
//   ./a.out ./apriori_stat/apriori_data_groups ./apriori_stat/apriori_data_dial ./data_bases/input_customers ./data_bases/input_operators ./test_data/group_file

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
#include <map>
using namespace std;

float g_dial_success_prob;
float g_dial_mean;
float g_dial_max;
float g_call_mean;

int Operator::count;
   
//expon with mean 1/lambda
const double eps=0.000001;
float expon(float lambda, float u)
{
    if (1 - u < eps)
        u = eps;
    if (u < eps)
        u = eps;
    return -(log(1 - u)/lambda);
}

ServiceTime emulate_service_time(float dial_success_prob, float dial_mean, float dial_max, float call_mean)
{
    ServiceTime st;
    float u = (rand() / float(RAND_MAX));
    st.time_to_service = 0;
    if (u < dial_success_prob)
    {
        u = (rand() / float(RAND_MAX));
        st.time_before_call = round(expon(1/dial_mean,u));
        st.time_to_service += st.time_before_call;
                
        u = (rand() / float(RAND_MAX));
        st.time_after_call = round(expon(1/call_mean,u));
        st.time_to_service += st.time_after_call;
    }
    else
    {
        st.time_to_service = st.time_before_call = round(dial_max);
    }
    if (st.time_to_service < 0 || st.time_to_service > 10000)
    {
        cout <<"time to service is wrong " << st.time_to_service<<endl;
        cout <<"simulation from: "<<dial_success_prob <<" "<<dial_mean <<" "<<dial_max <<" "<<call_mean<<endl;
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
    for ( auto& a:queue)
    {
        cout<<i<<",";
        a.show();
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

    // 1. Read apriori info: groups-wise call means and dial call means
    GroupsAptrioriMeans groupsMean = ReadGroupMeans(group_stat_file);
    ReadDialOptions(argv[2], dial_success_prob, dial_mean, dial_max);
    g_dial_success_prob = dial_success_prob;
    g_dial_mean = dial_mean;
    g_dial_max = dial_max;

    // 2. Read online operators info
    ops = read_operators(operators_file);

    if (VERBOSE)
    {
        for (auto& a:ops)
            a.show_ext();
    }

    // 3. Read online customers info
    map<int,int> groups_of_interest;
    vector<customer> customers = ReadCustomersDataBase(customers_data_base_file, groups_of_interest);

    cout <<"size of groups in customers: "<< groups_of_interest.size()<<endl;

    // return 0;
    
    if (SLOW)
        cin.get();
    
    int total_time = 0;
    int iteration_time;
    int iterations = 10000;
    vector<int> times(iterations);

    cout <<"sizeof (int) "<<sizeof(int)<<endl;
    cout <<"sizeof (unsigned long int) "<<sizeof(unsigned long int)<<endl;
    cout <<"sizeof (unsigned long long int) "<<sizeof(unsigned long long int)<<endl;
    unsigned long start_time = time(NULL),
        end_time;
    map<int, vector<int> > statInfo;
    vector<int> total_times(iterations,0);
    for (const auto& grp:groups_of_interest)
    {
        statInfo[grp.first] = vector<int>(iterations,0);
    }
    // cout <<"start time: "<<start_time<<endl;
    for (int j = 0; j < iterations; j++)
//    for (int j = 0; j < 1; j++)    
    {
        total_time = 0;
        
        Server s(ops, groupsMean, groups_of_interest);
        list<customer> c = create_queue(customers,true);
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
            // iteration_time = -1;
            cont = s.do_iteration(c);
        
            if(VERBOSE)
            {
                s.show();
                Show_queue_extended(c);
                cout<<endl<<endl;
            }
            if (SLOW)
                cin.get();
            i++;
        }
        s.GetGroupsStat(statInfo, total_times, j);
    }
    end_time = time(nullptr);
    // cout <<"end time: "<<end_time<<endl;
    unsigned long long summ = 0;
    for (int j = 0; j < iterations; j++)
    {
        summ += total_times[j];
    }
    cout << summ<<endl;
    summ /= iterations;
    cout <<"time spent alt = "<<summ<<endl;

    for (const auto& SI: statInfo)
    {
        unsigned long long tmp = 0;
        for (const auto& si:SI.second)
            tmp += si;
        tmp /= SI.second.size();
        cout <<"Group "<<SI.first<<" ~ "<<tmp<<" secs\n";
    }

    

    cout <<"time for generating:" << end_time - start_time<<endl;
    return 0;
}
