# %%
# Libraries
import math, sys
import numpy as np
import pandas as pd
import scipy.stats as st
from matplotlib import pyplot as plt
plt.style.use(['science','no-latex', 'ieee', 'light'])

# Class function for validating sensor data
# (For CSV format reference, see "test.csv")
#
# Input: file1 - csv file with sensor data
#        file2 - csv file with sensor data
# Output: r - Pearsons Correlation Coefficient: Tells us how correlated the data is
#         p - p value: Used to test the null hypothesis, which says that the data is not correlated
#         valid - Bool flag: True when p value < 0.05, which rejects null hypothesis (data is correlated)
#                            False when p value > 0.05, which accepts null hypothesis (data not correlected)
class Valid:
    def __init__(self, val1, val2):

        # Check if lenght of arrays match
        if (len(val1) != len(val2)):
            sys.exit("Length of arrays must be the same")

        # Set class 'r', 't' and 'p' values, as well as validity tests
        # print(st.pearsonr(val, val))
        length = len(val1)
        self.r = st.pearsonr(val1, val2)[0] 
        self.valid1 = abs(self.r) >= 2/math.sqrt(length) # First test - Check pearson correlation rule of thumb
        self.t = r*math.sqrt((length - 2)/(1 - r**2)) # Get T value from student T-distrubution of pearson correlation
        self.p = st.t.sf(abs(self.t), df=2) # Lookup p value based on t value

        if (self.p < 0.05): # Check if p value is less than alpha level => rejects null hypothesis
            self.valid2 = True
        else: # Check if p value is greater than alpha level => accept null hypothesis
            self.valid2 = False
            
# %%
# Prep Grafana data

# Read data and prepare arrays
data = pd.read_csv("ozon.csv", header=0, parse_dates=True)
t = data.iloc[:, 0].to_numpy()
t = np.flip(t)
pm2_5 = np.zeros(len(t))
pm10 = np.zeros(len(t))
pm_typ = np.zeros(len(t))
voc = np.zeros(len(t))
o3 = np.zeros(len(t))
r = data.iloc[:, 4].to_numpy()

# Extract elemnts by sensor type
x = data.iloc[:, 6].to_numpy()
voc = x[r==0]
o3 = x[r==1]
pm2_5 = x[r==2]
pm10 = x[r==3]
pm_typ = x[r==4]

# Flip arrays
o3 = np.flip(o3)
pm2_5 = np.flip(pm2_5)
pm10 = np.flip(pm10)

# Remove null values
t1 = t[r==1]
i = 0
for x in o3:
    if ((x == 0) or ((t1[i] == t1[i-1]) and i != 0)):
        o3 = np.delete(o3, i)
        t1 = np.delete(t1, i)
    else:
        i = i + 1

t2 = t[r==2]
i = 0
for x in pm2_5:
    if ((x == 0) or ((t2[i] == t2[i-1]) and i != 0)):
        pm2_5 = np.delete(pm2_5, i)
        t2 = np.delete(t2, i)
    else:
        i = i + 1

t3 = t[r==3]
i = 0
for x in pm10:
    if ((x == 0) or ((t3[i] == t3[i-1]) and i != 0)):
        pm10 = np.delete(pm10, i)
        t3 = np.delete(t3, i)
    else:
        i = i + 1

# Extract times that match with NILU data
# pm2_5 = np.take(pm2_5, [7, 17, 22, 28, 34, 39, 46, 53])/1e9
pm2_5 = np.take(pm2_5, [7, 17, 22, 28, 34, 39, 46, 53])
pm10 = np.take(pm10, [8, 16, 23, 31, 41, 48, 54, 60])
t2 = np.take(t2, [7, 17, 22, 28, 34, 39, 46, 53])
t3 = np.take(t3, [8, 16, 23, 31, 41, 48, 54, 60])

# %%
# Prep NILU
data = pd.read_csv("nilu.csv", header=3, parse_dates=True)
nt = data.iloc[:, 0].to_numpy()

# Extract data from 14:00 to 21:00
nt = nt[0:8] 
t2 = t2[0:8]
t3 = t3[0:8]

npm2_5 = data.iloc[:, 2]
npm2_5 = npm2_5[0:8]

npm10 = data.iloc[:, 5]
npm10 = npm10[0:8]

# %%
r = 0
t = Valid(pm2_5, npm2_5)
print("Test - PM2.5")
print("r value: ", t.r)
print("t value: ", t.t)
print("p value: ", t.p)
print("Test1 (Pearson RoT): ", t.valid1)
print("Test2 (T-test): ", t.valid2)
print("\n")

t = Valid(pm10, npm10)
print("Test - PM10")
print("r value: ", t.r)
print("t value: ", t.t)
print("p value: ", t.p)
print("Test1 (Pearson RoT): ", t.valid1)
print("Test2 (T-test): ", t.valid2)
print("\n")

# %%
plt.subplot(2,1,1)
plt.title("PM2.5", fontsize=6)
plt.gcf().axes[0].yaxis.get_major_formatter().set_scientific(False)
plt.plot(t2, pm2_5, '#ffaabb')
plt.xticks(fontsize=4)
plt.yticks(fontsize=4)
plt.ylabel("ug/m3", fontsize=5)
plt.legend(["Grafana"], fontsize=5)

plt.subplot(2,1,2)
plt.gcf().axes[1].yaxis.get_major_formatter().set_scientific(False)
plt.plot(nt, npm2_5, '#77aadd')
plt.xticks(fontsize=4)
plt.yticks(fontsize=4)
plt.ylabel("ug/m3", fontsize=5)
plt.xlabel("UTC+01", fontsize=5)
plt.legend(["NILU"], fontsize=5)

# plt.savefig("test_pm2_5.png")
plt.show()

plt.subplot(2,1,1)
plt.title("PM10.0", fontsize=6)
plt.gcf().axes[0].yaxis.get_major_formatter().set_scientific(False)
plt.plot(t3, pm10, '#bbcc33')
plt.xticks(fontsize=4)
plt.yticks(fontsize=4)
plt.ylabel("ug/m3", fontsize=5)
plt.legend(["Grafana"], fontsize=5)

plt.subplot(2,1,2)
plt.gcf().axes[1].yaxis.get_major_formatter().set_scientific(False)
plt.plot(nt, npm10, '#44bb99')
plt.xticks(fontsize=4)
plt.yticks(fontsize=4)
plt.ylabel("ug/m3", fontsize=5)
plt.xlabel("UTC+01", fontsize=5)
plt.legend(["NILU"], fontsize=5)

# plt.savefig("test_pm10.png")
plt.show()