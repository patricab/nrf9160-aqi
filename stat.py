
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
    def __init__(self, file1, file2):
        # Read CSV files, extract values column into numpy array
        df1 = pd.read_csv(file1, header=0, parse_dates=True)
        df2 = pd.read_csv(file2, header=0, parse_dates=True)
        val1 = df1.iloc[:, 6].to_numpy()
        val2 = df2.iloc[:, 6].to_numpy()

        # Set class 'r' and 'p' values
        # print(st.pearsonr(val, val))
        self.r = st.pearsonr(val1, val2)[0]
        self.p = st.t.sf(abs(r), df=2)

        if (self.p < 0.05): # Check if p value is less than alpha level => rejects null hypothesis
            self.valid = True
        else: # Check if p value is greater than alpha level => accept null hypothesis
            self.valid = False
            
# %%
t = Valid("test.csv","test.csv")
print(t.r)
print(t.p)
print(t.valid)