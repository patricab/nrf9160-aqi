
# %%
# Libraries
import numpy as np
import pandas as pd
import scipy.stats as st

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
    def __init__(self, file1, file2, length):

        # Read CSV files, extract values column into numpy array
        df1 = pd.read_csv(file1, header=0, parse_dates=True)
        df2 = pd.read_csv(file2, header=0, parse_dates=True)
        val1 = df1.iloc[0:length, 6].to_numpy()
        val2 = df2.iloc[0:length, 6].to_numpy()

        if (len(val1) != len(val2)):
            sys.exit("Length of arrays must be the same")

        # Set class 'r', 't' and 'p' values, as well as validity tests
        # print(st.pearsonr(val, val))
        self.r = st.pearsonr(val1, val2)[0] 
        self.valid1 = abs(self.r) >= 2/math.sqrt(length) # First test - Check pearson correlation rule of thumb
        self.t = r*math.sqrt((length - 2)/(1 - r**2)) # Get T value from student T-distrubution of pearson correlation
        self.p = st.t.sf(abs(self.t), df=2) # Lookup p value based on t value

        if (self.p <= 0.05): # Check if p value is less than alpha level => rejects null hypothesis
            self.valid2 = True
        else: # Check if p value is greater than alpha level => accept null hypothesis
            self.valid2 = False
            
# %%
# Read/format values from cloud
data = pd.read_csv("ozon.csv", parse_dates=True)
t = data.iloc[:, 0].to_numpy()
pm2_5 = np.zeros(len(t))
pm10 = np.zeros(len(t))
pm_typ = np.zeros(len(t))
voc = np.zeros(len(t))
o3 = np.zeros(len(t))
r = data.iloc[:, 4].to_numpy()

i = 0
for x in r:
    # print(i, x)
    if x == 0: # VOC
        # np.append(voc, data.iloc[i, 6])
        voc[i] = data.iloc[i, 6]
    if x == 1: # NO2
        # np.append(no2, data.iloc[i, 6])
        o3[i] = data.iloc[i, 6]
    if x == 2: # PM2.5
        # np.append(pm2_5, data.iloc[i, 6])
        pm2_5[i] = data.iloc[i, 6]
    if x == 3: # PM10
        # np.append(pm10, data.iloc[i, 6])
        pm10[i] = data.iloc[i, 6]
    if x == 4: # PM_Typical
        # np.append(pm_typ, data.iloc[i, 6])
        pm_typ[i] = data.iloc[i, 6]

    i = i + 1

t1 = t
i = 0
for x in o3:
    if (x == 0):
        o3 = np.delete(o3, i)
        t1 = np.delete(t1, i)
    else:
        i = i + 1

t2 = t
i = 0
for x in pm2_5:
    if (x == 0):
        pm2_5 = np.delete(pm2_5, i)
        t2 = np.delete(t2, i)
    else:
        i = i + 1

t3 = t
i = 0
for x in pm10:
    if (x == 0):
        pm10 = np.delete(pm10, i)
        t3 = np.delete(t3, i)
    else:
        i = i + 1

# %%
t = Valid("test.csv","test.csv", 10)
print("r value: ", t.r)
print("t value: ", t.t)
print("p value: ", t.p)
print("Test1 (Pearson RoT): ", t.valid1)
print("Test2 (T-test): ", t.valid2)