# Matplotlib
# NB!: pip install SciencePlots
# %%
import numpy as np
import pandas as pd
from matplotlib import pyplot as plt
# plt.style.use('science')
plt.style.use(['science','no-latex', 'ieee', 'light'])
# %%

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
data = pd.read_csv("nilu.csv", header=3, parse_dates=True)
t = data.iloc[:, 0].to_numpy()
pm2_5 = data.iloc[:, 2]
pm10 = data.iloc[:, 5]

plt.plot(t, pm2_5)
plt.plot(t, pm10)

plt.xticks(fontsize=5)
plt.yticks(fontsize=5)
plt.ylabel("ug/m3", fontsize=6)
plt.xlabel("UTC+02", fontsize=5)
plt.legend(["PM2.5", "PM10"], fontsize=6)

plt.show()
# %%
# Plot

plt.plot(t2, pm2_5)
plt.plot(t3, pm10)
plt.xticks(fontsize=2, rotation=90)
plt.yticks(fontsize=2)
plt.ylabel("ug/m3", fontsize=5)
plt.legend(["PM2.5", "PM10"], fontsize=5)
plt.xlabel("UTC+02", fontsize=4)
# plt.plot(t1, )

# ax = plt.gca()
# ax.axes.xaxis.set_visible(False)
plt.show()

# def ppk():
#     # %%
#     data = pd.read_csv("")