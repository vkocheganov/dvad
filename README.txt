1. To build binary file:
~: g++ -std=c++11 ./queue.cpp ./io.cpp -o predict_service


2. To run binary file:

2.1 Provide input files names (customers to service and free operators):
~: INPUT_DATA_orders=/home/victor/Develop/dvad/data/Logs/13.02/orders_log_2018_02_13_09_12_48.txt
~: INPUT_DATA_operators=/home/victor/Develop/dvad/data/Logs/13.02/operators_log_2018_02_13_09_12_48.txt

2.2 Run the binary file:
~: ./predict_service ./apriori_stat/apriori_data_groups ./apriori_stat/apriori_data_dial $INPUT_DATA_orders $INPUT_DATA_operators ./output 


where ./output is the file where binary file saves result in the form:
t_all
N
g_1 t_1 b_1
g_2 t_2 b_2
...
g_N t_N b_N

t_all --- mean time to service all queues
g_i --- group ids
t_i --- mean time to service g_i group
b_i --- one of {SUCCESS,FAIL} strings to indicate if queue can be serviced. For example if no one operator can service this group b_i will be FAIL
