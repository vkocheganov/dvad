#
#   customer_type|customer_id|date|group_id|shop_id|good_id|good_name|call_duration|operator_id
from scipy.stats import gamma
import pandas as pd
import numpy as np
import datetime
import os
from scipy.stats import expon
from scipy import stats, integrate
import matplotlib.pyplot as plt
import seaborn as sns
import sys

def dateparse (time_str):
    return datetime.datetime.strptime(time_str, "%Y-%m-%d %H:%M:%S")

if (len(sys.argv) <= 2):
    print "Too few arguments"

work_dir=sys.argv[1]
main_file=sys.argv[2]
# work_dir="/home/victor/Develop/dvad/dvad_git/call_durations_new"
#main_file="C:\\Users\\victork\\Documents\\Kagle\\DVaD\\Data\\operations.txt"
#main_file="/home/victor/Develop/dvad/data/operations.txt"

df_operations=pd.read_csv(main_file,header=None, sep='|',parse_dates=[2], date_parser=dateparse)


of_interest_idxes=(df_operations[7]>0)
of_interest=df_operations[of_interest_idxes][7]
of_interest_mean=np.mean(of_interest)


VISUALIZE=1
if (VISUALIZE):
    x = np.random.exponential(scale = of_interest_mean, size=2000)
    sns.distplot(x, kde=False,  norm_hist=True, color='r',label="exponential_mean %.2f" % of_interest_mean)
    sns.distplot(of_interest, kde=False,  norm_hist=True, color='b', label="real_data")
    plt.legend()
    plt.title("all durations, operations=%d" %(np.sum(of_interest_idxes)))
    plt.show()

groups=df_operations[of_interest_idxes][3].unique()
groups=np.sort(groups)


par_fold="%s/group_wise" % work_dir;
os.mkdir(par_fold)

fold_name="%s/%s" % (par_fold,'full')
os.mkdir(fold_name)
for group in groups:
    idxes=(of_interest_idxes) & (df_operations[3]==group)
    of_interest=(df_operations[idxes][7])
    of_interest_mean=np.mean(of_interest)
    if (VISUALIZE):
        x = np.random.exponential(scale = of_interest_mean, size=2000)
        sns.distplot(x, kde=False,  norm_hist=True, color='r',label="exponential_mean %.2f" % of_interest_mean)
        sns.distplot(of_interest, kde=False,  norm_hist=True, color='b', label="real_data")
        plt.legend()
        plt.title("group=%s, operations=%d" %(group,np.sum(idxes)))
        plt.savefig("%s/group_%s.png"%(fold_name,group) )
        plt.close()

    # x = np.random.exponential(scale = of_interest_mean, size=2000)
    # sns.distplot(x, kde=False,  norm_hist=True, color='r',label="exponential_mean %.2f" % of_interest_mean)
    # sns.distplot(of_interest, kde=False,  norm_hist=True, color='b', label="real_data")
    # plt.legend()
    # plt.title("group=%s, operations=%d" %(group,np.sum(idxes)))
    # plt.savefig("%s/group_%s.png"%(fold_name,group) )
    # plt.close()






