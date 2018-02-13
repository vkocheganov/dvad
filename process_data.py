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
#main_file="C:\\Users\\victork\\Documents\\Kagle\\DVaD\\Data\\operations.txt"
main_file="/home/victor/Develop/dvad/data/operations.txt"

df=pd.read_csv(main_file,sep='|',parse_dates=['date'], date_parser=dateparse)

idx_fisrt_call_fail=(df['customer_type']=='first_call') & (df['call_duration']==0)
df.set_value(idx_fisrt_call_fail,'customer_type',"first_call_fail")
#np_df=df.as_matrix()
#operation_types=np.unique(df['customer_type'])
operation_types=['first_call','first_touch']

days=np.sort(df['date'].dt.day.unique())
days=days[days[:] <=29 ]

operators=df['operator_id'].unique()

idx_first_op=df['customer_type']=='first_operation'
first_op_customers=df[idx_first_op]['customer_id'].unique()
print first_op_customers[0:10]
first_call_fail=df[idx_fisrt_call_fail]['customer_id'].unique()
print first_call_fail[0:10]


#par_fold="call_durations\\day_wise"
# par_fold="call_durations/day_wise"
# for operation_type in operation_types:
#     fold_name="%s/%s" % (par_fold,operation_type)
#     os.mkdir(fold_name)
#     for day_number in days:
#         from_date=datetime.datetime(2018,1,day_number,0,0,0)
#         to_date= from_date + datetime.timedelta(days=1)
#         idxes=(df['date']>=from_date) & (df['date']<to_date) & (df['customer_type'].str.strip() == operation_type)
#         if (np.sum(idxes) == 0):
#             continue
#         of_interest=df[idxes]['call_duration']

#         of_interest_mean=np.mean(of_interest)
#         x = np.random.exponential(scale = of_interest_mean, size=2000)
#         sns.distplot(x, kde=False,  norm_hist=True, color='r',label="exponential_mean %.2f" % of_interest_mean)
#         sns.distplot(of_interest, kde=False,  norm_hist=True, color='b', label="real_data")
#         plt.legend()
#         plt.title("Day=%d, operation_type=\"%s\", operations=%d" %(day_number,operation_type,np.sum(idxes)))
#         plt.savefig("%s/day_%d_oper_%s.png"%(fold_name,day_number,operation_type) )
#         plt.close()




# operators=df['operator_id'].unique()
# par_fold="call_durations/operator_wise"
# for operation_type in operation_types:
#     fold_name="%s/%s" % (par_fold,operation_type)
#     os.mkdir(fold_name)
#     for operator in operators:
#         idxes=(df['operator_id']==operator) & (df['customer_type'].str.strip() == operation_type)
#         if (np.sum(idxes) == 0):
#             continue
#         of_interest=df[idxes]['call_duration']

#         of_interest_mean=np.mean(of_interest)
#         x = np.random.exponential(scale = of_interest_mean, size=2000)
#         sns.distplot(x, kde=False,  norm_hist=True, color='r',label="exponential_mean %.2f" % of_interest_mean)
#         sns.distplot(of_interest, kde=False,  norm_hist=True, color='b', label="real_data")
#         plt.legend()
#         plt.title("operator=%s, operation_type=\"%s\", operations=%d" %(operator,operation_type,np.sum(idxes)))
#         plt.savefig("%s/operator_%s_oper_%s.png"%(fold_name,operator,operation_type) )
#         plt.close()





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




# idxes=(df['call_duration']>0)
# #idxes=(df['call_duration']>0) & (df['customer_type'].str.strip() == 'first_call')
# #idxes=(df['call_duration']>0) & (df['customer_type'].str.strip() == 'first_touch')
# np.sum(idxes)
# of_interest=df[idxes]['call_duration']
# of_interest_mean=np.mean(of_interest)
# x = np.random.exponential(scale = of_interest_mean, size=2000)
# sns.distplot(x, kde=False,  norm_hist=True, color='r',label="exponential_mean %.2f" % of_interest_mean)
# sns.distplot(of_interest, kde=False,  norm_hist=True, color='b', label="real_data")
# plt.legend()
# #plt.title("all durations, operations=%d" %(np.sum(idxes)))
# plt.title("all first_touch durations, operations=%d" %(np.sum(idxes)))




groups=df['group_id'].unique()
par_fold="call_durations/group_wise"

fold_name="%s/%s" % (par_fold,'full')
os.mkdir(fold_name)
for group in groups:
    idxes=(df['group_id']==group) &  ( (df['customer_type'].str.strip() == 'first_call') | (df['customer_type'].str.strip() == 'first_touch'))
    if (np.sum(idxes) == 0):
        continue
    of_interest=(df[idxes]['call_duration'])
    print(np.sum(of_interest==0))

    of_interest_mean=np.mean(of_interest)
    x = np.random.exponential(scale = of_interest_mean, size=2000)
    sns.distplot(x, kde=False,  norm_hist=True, color='r',label="exponential_mean %.2f" % of_interest_mean)
    sns.distplot(of_interest, kde=False,  norm_hist=True, color='b', label="real_data")
    plt.legend()
    plt.title("group=%s, operations=%d" %(group,np.sum(idxes)))
    plt.savefig("%s/group_%s.png"%(fold_name,group) )
    plt.close()




# par_fold="call_durations\\operator_wise\\group_wise"
# operators=df['operator_id'].unique()

# for operator in operators:
#     fold_name="%s\\%s" % (par_fold,operator)
#     os.mkdir(fold_name)
#     groups = df['group_id'].unique()
#     for group in groups:
#         idxes=(df['operator_id']==operator) & (df['group_id'].str.strip() == group)
#         if (np.sum(idxes) == 0):
#             continue
#         of_interest=df[idxes]['call_duration']

#         of_interest_mean=np.mean(of_interest)
#         x = np.random.exponential(scale = of_interest_mean, size=2000)
#         sns.distplot(x, kde=False,  norm_hist=True, color='r',label="exponential_mean %.2f" % of_interest_mean)
#         sns.distplot(of_interest, kde=False,  norm_hist=True, color='b', label="real_data")
#         plt.legend()
#         plt.title("operator=%s, group=\"%s\", operations=%d" %(operator,group,np.sum(idxes)))
#         plt.savefig("%s\\operator_%s_group_%s.png"%(fold_name,operator, group) )
#         plt.close()


# prob=0.9
# q1=(1-prob)/2
# q2=1-(1-prob)/2
# a=20
# scale=200
# print gamma.mean(a,loc=0,scale=scale)
#
# alpha1=gamma.ppf(q1, a, loc=0, scale=scale)
# alpha2=gamma.ppf(q2, a, loc=0, scale=scale)
# print "(%.2f, %.2f) vs (%.2f, %.2f)" % (alpha1,alpha2,0,a*scale)
