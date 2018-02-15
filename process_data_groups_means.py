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

def dateparse (time_str):
    return datetime.datetime.strptime(time_str, "%Y-%m-%d %H:%M:%S")

work_dir="/home/victor/Develop/dvad/dvad_git/call_durations_new"
#main_file="C:\\Users\\victork\\Documents\\Kagle\\DVaD\\Data\\operations.txt"
#main_file="/home/victor/Develop/dvad/data/operations.txt"
main_file="/home/victor/Develop/dvad/data/operations_12_02.txt"

df=pd.read_csv(main_file,sep='|',parse_dates=['date'], date_parser=dateparse)

idx_fisrt_call_fail=(df['customer_type']=='first_call') & (df['call_duration']==0)
df.set_value(idx_fisrt_call_fail,'customer_type',"first_call_fail")
operation_types=['first_call','first_touch']

#days=np.sort(df['date'].dt.day.unique())
#days=days[days[:] <=29 ]

operators=df['operator_id'].unique()

idx_first_op=df['customer_type']=='first_operation'
first_op_customers=df[idx_first_op]['customer_id'].unique()
print first_op_customers[0:10]
first_call_fail=df[idx_fisrt_call_fail]['customer_id'].unique()
print first_call_fail[0:10]



idxes=(df['call_duration']>0)
np.sum(idxes)
of_interest=df[idxes]['call_duration']
of_interest_mean=np.mean(of_interest)
x = np.random.exponential(scale = of_interest_mean, size=2000)
sns.distplot(x, kde=False,  norm_hist=True, color='r',label="exponential_mean %.2f" % of_interest_mean)
sns.distplot(of_interest, kde=False,  norm_hist=True, color='b', label="real_data")
plt.legend()
plt.title("all durations, operations=%d" %(np.sum(idxes)))
plt.show()


groups=df['group_id'].unique()
par_fold="%s/group_wise" % work_dir;
os.mkdir(par_fold)

fold_name="%s/%s" % (par_fold,'full')
os.mkdir(fold_name)
for group in groups:
    idxes=(df['group_id']==group) &  ( (df['customer_type'].str.strip() == 'first_call') | (df['customer_type'].str.strip() == 'first_touch'))
    if (np.sum(idxes) == 0):
        continue
    of_interest=(df[idxes]['call_duration'])

    of_interest_mean=np.mean(of_interest)
    x = np.random.exponential(scale = of_interest_mean, size=2000)
    sns.distplot(x, kde=False,  norm_hist=True, color='r',label="exponential_mean %.2f" % of_interest_mean)
    sns.distplot(of_interest, kde=False,  norm_hist=True, color='b', label="real_data")
    plt.legend()
    plt.title("group=%s, operations=%d" %(group,np.sum(idxes)))
    plt.savefig("%s/group_%s.png"%(fold_name,group) )
    plt.close()






