#
#   customer_type|customer_id|date|group_id|shop_id|good_id|good_name|call_duration|operator_id
import pandas as pd, numpy as np
from scipy.stats import expon
import datetime, os, sys, time
from subprocess import call

# import matplotlib.pyplot as plt
# import seaborn as sns

def proc(str):
    [hh, mm, ss] = [int(s) for s in str.split(':') ]
    if (hh < 0 | mm < 0 | ss < 0):
        return 0
    return (hh*3600 + mm*60 + ss)

def dateparse (time_str):
    return datetime.datetime.strptime(time_str, "%Y-%m-%d %H:%M:%S")

def skipping (line_num):
    if (line_num % 2 == 0):
        return False
    else:
        return True
    
VISUALIZE=False

input_dial_file_name="calls-log-result-export.csv"
binary_file_name="./predict_service"
if (len(sys.argv) == 6):
    input_operations_history_file=sys.argv[1]
    orders_data_base=sys.argv[2]
    operators_data_base=sys.argv[3]
    learn_new_data=sys.argv[4]
    build_data=sys.argv[5]
else:
    print "wrong number of arguments. Expected 5, given %s" % (len(sys.argv)-1)
    exit(1)

output_groups_file_name="apriori_data_groups"
output_dial_file_name="apriori_data_dial"
output_file="predictions_%s" % (time.strftime("%Y%m%d-%H%M%S"))


    
    

if (learn_new_data == "yes"):
    print "---------- Learning phase"
    #########################################################
    # Generate groups
    #########################################################

    df_operations=pd.read_csv(input_operations_history_file,sep='|',parse_dates=['date'], date_parser=dateparse)
    groups_file = open(output_groups_file_name,'w')
    print "Creating file %s " % output_groups_file_name

    #of_interest_idxes=(df_operations['call_duration']>0) & (df_operations['customer_type']=='first_call')
    #of_interest_idxes=(df_operations['call_duration']>0) & (df_operations['customer_type']=='first_touch')
    of_interest_idxes=(df_operations['call_duration']>0)
    of_interest=df_operations[of_interest_idxes]['call_duration']
    of_interest_mean=np.mean(of_interest)



    if (VISUALIZE):
        x = np.random.exponential(scale = of_interest_mean, size=2000)
        sns.distplot(x, kde=False,  norm_hist=True, color='r',label="exponential_mean %.2f" % of_interest_mean)
        sns.distplot(of_interest, kde=False,  norm_hist=True, color='b', label="real_data")
        plt.legend()
        plt.title("all durations, operations=%d" %(np.sum(of_interest_idxes)))
        plt.show()

    groups=df_operations[of_interest_idxes]['group_id'].unique()
    groups=np.sort(groups)


    groups_file.write("%d\n" % groups.shape[0])
    groups_file.write("%.0f\n" % of_interest_mean)


    for group in groups:
        idxes=(of_interest_idxes) & (df_operations['group_id']==group)
        of_interest=(df_operations[idxes]['call_duration'])
        of_interest_mean=np.mean(of_interest)
        groups_file.write("%d %.0f\n" %(group,of_interest_mean))
        if (VISUALIZE):
            x = np.random.exponential(scale = of_interest_mean, size=2000)
            sns.distplot(x, kde=False,  norm_hist=True, color='r',label="exponential_mean %.2f" % of_interest_mean)
            sns.distplot(of_interest, kde=False,  norm_hist=True, color='b', label="real_data")
            plt.legend()
            plt.title("group=%s, operations=%d" %(group,np.sum(idxes)))
            plt.savefig("%s/group_%s.png"%(fold_name,group) )
            plt.close()

#    print "finish generating groups' means"
    groups_file.close()


    #########################################################
    # Generate dial info
    #########################################################

    dial_file = open(output_dial_file_name,'w')
    print "Creating file %s " % output_dial_file_name

    # Success call probability
    first_calls=(df_operations['customer_type']=='first_call')
    first_calls_succ=(df_operations['customer_type']=='first_call') & (df_operations['call_duration']>0)
    succ_prob=np.sum(first_calls_succ)/float(np.sum(first_calls))
    dial_file.write("%.2f " % succ_prob)


    #Wait times
    df_dial=pd.read_csv(input_dial_file_name,sep=';', encoding='cp1251')

    w_t=df_dial.loc[:,'wait_time']
    df_dial['wait_time']=w_t.apply(proc)
    w_t=df_dial.loc[:,'talk_time']
    df_dial['talk_time']=w_t.apply(proc)

    idx_without_delivery=df_dial['queue'] != 'delivery'

    with_succ=(idx_without_delivery) & (df_dial['talk_time'] > 0)
    wait_with_succ = df_dial[with_succ]['wait_time']
    mean_time=np.mean(wait_with_succ)
    dial_file.write("%.2f " % mean_time)
    if (VISUALIZE):
        sns.distplot(wait_with_succ, kde=False,  norm_hist=True, color='r',label="wait_time_%.2f_without_talk__%.2d" % (mean_time,np.sum(with_succ)))
        plt.legend()
        plt.show()

    without_succ=(idx_without_delivery) & (df_dial['talk_time'] == 0)
    wait_without_succ = df_dial[without_succ]['wait_time']
    mean_time=np.mean(wait_without_succ)
    dial_file.write("%.2f\n" % mean_time)
    if (VISUALIZE):
        sns.distplot(wait_without_succ, kde=False,  norm_hist=True, color='g',label="wait_time_%.2f_without_talk__%.2d" % (mean_time,np.sum(without_succ)))
        plt.legend()
        plt.show()


#    print "finish generating dial's means"
    dial_file.close()

if (build_data == "yes"):
    build_command="g++ -std=c++11 ./queue.cpp ./io.cpp -o %s" % binary_file_name
    print "---------- Runing build command: %s"% build_command
    os.system(build_command)
    print "Creating file %s " % binary_file_name

string_to_run = "%s %s %s %s %s %s" % (binary_file_name, output_groups_file_name, output_dial_file_name, orders_data_base, operators_data_base, output_file)
print "---------- Running app:\n%s" % string_to_run 
os.system(string_to_run)
