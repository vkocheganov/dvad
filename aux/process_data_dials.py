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

#dial_file="C:\\Users\\victork\\Documents\\Kagle\\DVaD\\Data\\calls-log-result-export.csv"
dial_file="/home/victor/Develop/dvad/data/calls-log-result-export.csv"
#df=pd.read_csv(dial_file,sep=';', skiprows=skipping, encoding='cp1251')
df=pd.read_csv(dial_file,sep=';', encoding='cp1251')

w_t=df.loc[:,'wait_time']
df['wait_time']=w_t.apply(proc)

w_t=df.loc[:,'talk_time']
df['talk_time']=w_t.apply(proc)

print df.columns.values
#print df["status"].unique()
print df["queue"].unique()

# idx_automatics=df["queue"]=='Automatic_reply'
# print np.sum(idx_automatics)
# idx_neurika=df["queue"]=='Neurika_common_queue'
# print np.sum(idx_neurika)
# idx_duhi=df["queue"]=='Duhi_braslety'
# print np.sum(idx_duhi)
# idx_zdorov=df["queue"]=='XO_zdorov_queue_worm_call'
# print np.sum(idx_zdorov)
# idx_dostavka=df["queue"]=='Dostavka_Vykup_soprovogdenie'
# print np.sum(idx_dostavka)

# idx_nan= (df["queue"]!='Automatic_reply') & (df["queue"]!='Neurika_common_queue') & (df["queue"]!='Duhi_braslety') & (df["queue"]!='Neurika_common_queue') & (df["queue"]!='XO_zdorov_queue_worm_call') & (df["queue"]!='Dostavka_Vykup_soprovogdenie')
# print df[idx_nan]["queue"]
# print np.sum(idx_nan)


#par_fold="C:\\Users\\victork\\Documents\\Kagle\\DVaD\\dials"
par_fold="/home/victor/Develop/dvad/dvad_git/dials/"
wait_fold="%s/wait_time" % par_fold
os.mkdir(wait_fold)
wait_with_talk_fold="%s/wait_time_with_talk" % par_fold
os.mkdir(wait_with_talk_fold)
wait_without_talk_fold="%s/wait_time_without_talk" % par_fold
os.mkdir(wait_without_talk_fold)
talk_fold="%s/talk_time" % par_fold
os.mkdir(talk_fold)
for queue_type in df["queue"].unique():
    idxs = df["queue"]==queue_type
    mean_time=np.mean(df[idxs]['wait_time'])
    sns.distplot(df[idxs]['wait_time'], kde=False, norm_hist=True, color='r', label="wait_time_%.2f_%s__%.2d" % (mean_time,queue_type,np.sum(idxs)))
    plt.legend()
    plt.savefig("%s/wait_time_%s.png" % (wait_fold, queue_type))
    plt.close()

    non_zero_talk= (idxs) & (df["talk_time"] > 0)
    sucs_perc=float(np.sum(non_zero_talk))/np.sum(idxs)
    mean_time = np.mean(df[non_zero_talk]['wait_time'])
    sns.distplot(df[non_zero_talk]['wait_time'], kde=False, norm_hist=True, color='y', label="wait_time_%.2f_with_talk_%s__%.2d" % (mean_time,queue_type,np.sum(non_zero_talk)))
    plt.legend()
    plt.savefig("%s/wait_time_with_talk_%s.png" % (wait_with_talk_fold, queue_type))
    plt.close()

    zero_talk= (idxs) & (df["talk_time"] == 0)
    mean_time = np.mean(df[zero_talk]['wait_time'])
    sns.distplot(df[zero_talk]['wait_time'], kde=False, norm_hist=True, color='y', label="wait_time_%.2f_without_talk_%s__%.2d" % (mean_time,queue_type,np.sum(zero_talk)))
    plt.legend()
    plt.savefig("%s/wait_time_without_talk_%s.png" % (wait_without_talk_fold, queue_type))
    plt.close()

    mean_time = np.mean(df[non_zero_talk]['talk_time'])
    sns.distplot(df[non_zero_talk]['talk_time'], kde=False, norm_hist=True, color='g', label="talk_time_%.2f_%s_succ_%.2f__%.2d" % (mean_time,queue_type,sucs_perc,np.sum(non_zero_talk)))
    plt.legend()
    plt.savefig("%s/talk_time_%s.png" % (talk_fold, queue_type))
    plt.close()

mean_time=np.mean(df[:]['wait_time'])
sns.distplot(df[:]['wait_time'], kde=False, norm_hist=True, color='r', label="wait_time_%.2f_all__%.2d" % (mean_time,df.shape[0]))
plt.legend()
plt.savefig("%s/wait_time_all.png" % (wait_fold))
plt.close()

non_zero_talk = (df["talk_time"] > 0)
sucs_perc=float(np.sum(non_zero_talk))/df.shape[0]
mean_time = np.mean(df[non_zero_talk]['wait_time'])
sns.distplot(df[non_zero_talk]['wait_time'], kde=False, norm_hist=True, color='y', label="wait_time_all_%.2f_with_talk__%.2d" % (mean_time,np.sum(non_zero_talk)))
plt.legend()
plt.savefig("%s/wait_time_all_with_talk.png" % (wait_with_talk_fold))
plt.close()

zero_talk= (df["talk_time"] == 0)
mean_time = np.mean(df[zero_talk]['wait_time'])
sns.distplot(df[zero_talk]['wait_time'], kde=False, norm_hist=True, color='y', label="wait_time_%.2f_without_talk__%.2d" % (mean_time,np.sum(zero_talk)))
plt.legend()
plt.savefig("%s/wait_time_without_talk.png" % (wait_without_talk_fold))
plt.close()

mean_time = np.mean(df[non_zero_talk]['talk_time'])
sns.distplot(df[non_zero_talk]['talk_time'], kde=False, norm_hist=True, color='g', label="talk_time_all_%.2f_succ_%.2f__%.2d" % (mean_time,sucs_perc,np.sum(non_zero_talk)))
plt.legend()
plt.savefig("%s/talk_time_all.png" % (talk_fold))
plt.close()

