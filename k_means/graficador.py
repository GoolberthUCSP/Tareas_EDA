from matplotlib import pyplot as plt
import numpy as np
import pandas as pd
import os

# 2 casos: k=2 y k=5
# 2 gráficos: k=2 y k=5
# Archivos: k2.csv, k5.csv
# Columnas: x, y, cluster

# Obtener directorios
dir_k2= os.getcwd() + "\Tareas_EDA\k_means\data\k2.csv"
dir_k5= os.getcwd() + "\Tareas_EDA\k_means\data\k5.csv"

# Cargar datos
k2 = pd.read_csv(dir_k2)
k5 = pd.read_csv(dir_k5)

# Graficar k=2 y k=5 en 2 gráficos distintos, cada linea tiene x y cluster, cada cluster tiene un color
figure, axes = plt.subplots(1, 2, figsize=(10, 5))
axes[0].set_title("K=2")
axes[1].set_title("K=5")
axes[0].set_xlabel("X")
axes[1].set_xlabel("X")
axes[0].set_ylabel("Y")
axes[1].set_ylabel("Y")
# Disminuir tamaño de puntos
axes[0].scatter(k2['x'], k2['y'], c=k2['cluster'], cmap='rainbow', s=1)
axes[1].scatter(k5['x'], k5['y'], c=k5['cluster'], cmap='rainbow', s=1)
plt.show()

# Borrar ficheros temporales
os.remove(dir_k2)
os.remove(dir_k5)