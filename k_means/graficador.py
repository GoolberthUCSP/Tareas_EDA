from matplotlib import pyplot as plt
import numpy as np
import pandas as pd

# 2 casos: k=2 y k=5
# Ambos casos ejecución paralela y secuencial
# 4 gráficos comparando los 4 casos
# Ficheros: k2_secuencial.csv, k2_paralelo.csv, k5_secuencial.csv, k5_paralelo.csv
# Columnas: x, y, cluster

# Cargar datos
k2_secuencial = pd.read_csv('k2_secuencial.csv')
k2_paralelo = pd.read_csv('k2_paralelo.csv')
k5_secuencial = pd.read_csv('k5_secuencial.csv')
k5_paralelo = pd.read_csv('k5_paralelo.csv')

# Gráficos
fig, axs = plt.subplots(2, 2)
fig.suptitle('Comparación de ejecución secuencial y paralela')
axs[0, 0].scatter(k2_secuencial['x'], k2_secuencial['y'], c=k2_secuencial['cluster'])
axs[0, 0].set_title('k=2, secuencial')
axs[0, 1].scatter(k2_paralelo['x'], k2_paralelo['y'], c=k2_paralelo['cluster'])
axs[0, 1].set_title('k=2, paralelo')
axs[1, 0].scatter(k5_secuencial['x'], k5_secuencial['y'], c=k5_secuencial['cluster'])
axs[1, 0].set_title('k=5, secuencial')
axs[1, 1].scatter(k5_paralelo['x'], k5_paralelo['y'], c=k5_paralelo['cluster'])
axs[1, 1].set_title('k=5, paralelo')