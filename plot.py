# Libraries
# NB!: pip install SciencePlots
# %%
import numpy as np
import pandas as pd
from matplotlib import pyplot as plt
plt.style.use(['science','no-latex', 'ieee', 'light'])
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

# %%
# Plot Grafana
# Note: Color schemes: https://personal.sron.nl/~pault/

plt.subplot(3,1,1)
plt.gcf().axes[0].yaxis.get_major_formatter().set_scientific(False)
plt.plot(t2, pm2_5, '#77aadd')
plt.xticks(fontsize=2, rotation=90)
plt.yticks(fontsize=2)
plt.ylabel("ug/m3", fontsize=5)
plt.legend(["PM2.5"], fontsize=5)

plt.subplot(3,1,2)
plt.gcf().axes[1].yaxis.get_major_formatter().set_scientific(False)
plt.plot(t3, pm10, '#ee8866')
plt.xticks(fontsize=2, rotation=90)
plt.yticks(fontsize=2)
plt.ylabel("ug/m3", fontsize=5)
plt.legend(["PM10"], fontsize=5)

plt.subplot(3,1,3)
plt.plot(t1, o3, '#44bb99')
plt.xticks(fontsize=4)
plt.yticks(fontsize=4)
plt.ylabel("PPB", fontsize=5)
plt.legend(["O3"], fontsize=5)
plt.xlabel("UTC+01", fontsize=4)

plt.subplots_adjust(hspace=0.5)
plt.show()

# %%
# Plot NILU
data = pd.read_csv("nilu.csv", header=3, parse_dates=True)
t = data.iloc[:, 0].to_numpy()
pm2_5 = data.iloc[:, 2]
pm10 = data.iloc[:, 5]

plt.plot(t, pm2_5)
plt.plot(t, pm10)

plt.xticks(fontsize=5)
plt.yticks(fontsize=5)
plt.ylabel("ug/m3", fontsize=6)
plt.xlabel("UTC+01", fontsize=5)
plt.legend(["PM2.5", "PM10"], fontsize=6)

plt.show()
