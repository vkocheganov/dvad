Dependencies
   INSTALL python libs:
   ~: python --version
   ~: sudo yum install -y python27
   ~: sudo yum install python-pip
   ~: sudo yum install python-tk
   ~: sudo pip install scipy pandas matplotlib seaborn

1. Define environment variables:
   ~: INPUT_APRIORY_operations=/home/victor/Development/dvad/data/operations_12_02.txt
   ~: INPUT_DATA_orders=/home/victor/Development/dvad/data/Logs/13.02/orders_log_2018_02_13_09_12_48.txt
   ~: INPUT_DATA_operators=/home/victor/Development/dvad/data/Logs/13.02/operators_log_2018_02_13_09_12_48.txt
   ~: BUILD_PREDICTOR=yes
   ~: LEARN_PREDICTOR=yes


2.a To build/learn/run predictions (need only once):
    ~: python ./run_prediction.py $INPUT_APRIORY_operations $INPUT_DATA_orders $INPUT_DATA_operators $LEARN_PREDICTOR $BUILD_PREDICTOR

2.b To skip build/learn phases and initiate only run phase:
    ~: BUILD_PREDICTOR=no
    ~: LEARN_PREDICTOR=no
    ~: python ./run_prediction.py $INPUT_APRIORY_operations $INPUT_DATA_orders $INPUT_DATA_operators $LEARN_PREDICTOR $BUILD_PREDICTOR


It will generate "predictions_*timestamp*" file. The file structure is following:
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

