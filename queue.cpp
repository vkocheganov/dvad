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

#define SLOW 1
#define VERBOSE 1

//expon with mean 1/lambda
float expon(float lambda, float u)
{
    return -(log(1 - u)/lambda);
}

class customer
{
public:
    customer (): time_to_service(0), group_id(-1) {};
    bool operator<(const customer& a) const {
        return enter_time < a.enter_time;
    }
    void emulate_service_time(float dial_success_prob, float dial_mean, float dial_max, float call_mean)
        {
            float u = (rand() / float(RAND_MAX));
            time_before_call = time_after_call = 0;
            if (u < dial_success_prob)
            {
                u = (rand() / float(RAND_MAX));
                time_before_call = expon(1/dial_mean,u);
                time_to_service += time_before_call;
                
                u = (rand() / float(RAND_MAX));
                time_after_call = expon(1/call_mean,u);
                time_to_service += time_after_call;
            }
            else
            {
                time_to_service = time_before_call = dial_max;
            }
            
        }
    void show()
        {
            printf("group_id=%d, enter_time=%.3f, time_to_service=%.3f ( %.3f + %.3f)",(int)group_id, (float)enter_time, (float)time_to_service, (float)time_before_call, (float)time_after_call);
//            cout <<"group_id="<<group_id<<" enter_time="<<enter_time<<" time_to_service="<<time_to_service<<"("<<time_before_call<<"+"<<time_after_call<<")";
        }
    float time_before_call;
    float time_after_call;
    float time_to_service;
    float enter_time;
    int group_id;
};


class Operator
{
    static int count;
public:
    Operator(vector<int> _ids = {}, int _operator_id = -1): is_free(0), time_to_free(0), group_ids(_ids), is_enabled(1), group_id(-1) {operator_id = _operator_id;};
    int operator_id;
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
    void start_new_customer(list<customer>& c)
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
                    time_to_free = a->time_to_service;
                    c.erase(a);
                    return;
                }
            }
            is_enabled = false;
            if(VERBOSE)
                cout<<"disabling operator "<<operator_id<<endl;
        }
};
int Operator::count;

class Server
{
public:
    Server(const vector<Operator>& ops): operators(ops){};
    vector<Operator> operators;
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
                op.start_new_customer(cs);
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
                minimum->start_new_customer(cs);
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



list<customer> serv_queue;


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
//        printf("(%d, %.2f)\n",(int)a.group_id, (float)a.time_to_service);

//        printf("(%d, %.4f)\n",(int)a.group_id, (float)a.enter_time);
        printf("(%d, %d, %.4f, %.4f) ",i, (int)a.group_id, (float)a.enter_time, (float)a.time_to_service);
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

vector<customer> generate_customers(const vector<int> &group_nums, const vector<float>& group_means, float dial_success_prob, float dial_mean, float dial_max)
{
    int total_size = accumulate(group_nums.begin(), group_nums.end(), 0);
    vector<customer> temp(total_size);
    int shift = 0;
    int group_id = 0;
    for (const auto& gn:group_nums)
    {
        for (int i = shift; i < shift + gn; i++ )
        {
            temp[i].group_id = group_id;
            temp[i].enter_time = (rand() / float(RAND_MAX));
            temp[i].emulate_service_time(dial_success_prob, dial_mean, dial_max, group_means[group_id]);
            temp[i].group_id = group_id;
            // cout<<"generated customer:";
            // temp[i].show();
            // cout<<endl;
        }
        shift += gn;
        group_id++;
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
        cout <<"readint operators"<<endl;
        cout <<"num operators = "<<nops<<endl;
    }
    for (int i = 0; i < nops; i++)
    {
        getline(file, line);
        istringstream iss(line);
        vector<int> groups;
        while (!iss.eof())
        {
            int temp;
            iss >> temp;
            groups.push_back(temp);
        }
        temp[i] = Operator(groups, i);
        if(VERBOSE)
            temp[i].show_ext();
    }
    return temp;
}


void read_groups(string gr_filename, vector<int>& groups_num, vector<float>& groups_mean)
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
    
    getline(file, line);
    istringstream iss3(line);
    groups_mean.clear();
    if(VERBOSE)
        cout <<"means: ";
    while (!iss3.eof())
    {
        float temp;
        iss3 >> temp;
        groups_mean.push_back(temp);
        if(VERBOSE)
            cout << temp<< " ";
    }
    if(VERBOSE)
        cout<<endl;
}


void read_dial_options(string dial_filename, float& success, float& dial_mean, float& dial_max)
{
    ifstream file(dial_filename);
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


//void generate_customer_database(vector<int>& groups_nums,)
#define DIAL_SUCCESS_PROB 0.9
#define DIAL_MEAN 5
#define DIAL_MAX 15

const vector<int> GROUPS_NUM = {10, 30};
const vector<float> GROUPS_MEAN = {200, 200};
const vector<Operator> OPERATORS = {
    Operator({0}, 0),
    Operator({0,1}, 1),
};

int main(int argc, char* argv[])
{
    srand(time(NULL));
    vector<Operator> ops;
    vector<int> groups_nums;
    vector<float> groups_means;
    float dial_success_prob,
        dial_mean,
        dial_max;
    
    if (argc >= 4)
    {
        ops = read_operators(argv[1]);
        read_groups(argv[2], groups_nums, groups_means);
        read_dial_options(argv[3], dial_success_prob, dial_mean, dial_max);
        
    }
    else
    {
        ops = OPERATORS;
        groups_nums = GROUPS_NUM;
        groups_means = GROUPS_MEAN;
        dial_success_prob = DIAL_SUCCESS_PROB;
        dial_mean = DIAL_MEAN;
        dial_max = DIAL_MAX;
    }


    if (SLOW)
        cin.get();
    // return 0;
    float total_time = 0, iteration_time;
//    for (int j = 0; j < 1000; j++)
    for (int j = 0; j < 1; j++)    
    {
        
        Server s(ops);
        vector<customer> temp_c = generate_customers(groups_nums, groups_means, dial_success_prob, dial_mean, dial_mean);
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
