from matplotlib import pyplot as plt
import numpy as np
import pandas as pd
import os

# 2 casos: k=2 y k=5
# 2 gráficos: k=2 y k=5
# Archivos: k2.csv, k5.csv
# Columnas: x, y, cluster
#data= [2, 5, 10, 20, 50, 100]
data= [2, 5]
row= 1
col= 2

# Obtener directorios
absolutepath = os.path.abspath(__file__)
absolutepath= os.path.dirname(absolutepath)
dir= os.path.join(absolutepath, "data")
dirs= [os.path.join(dir, f"k{i}.csv") for i in data]

# Cargar datos
csvs= [pd.read_csv(dir) for dir in dirs]

# Graficar k=2 y k=5 en 2 gráficos distintos, cada linea tiene x y cluster, cada cluster tiene un color
figure, axes = plt.subplots(row, col, figsize=(10, 5))
for i, ax in enumerate(axes):
    if i < len(csvs):
        ax.set_title(f"K={data[i]}")
        ax.set_xlabel("X")
        ax.set_ylabel("Y")
        ax.scatter(csvs[i]['x'], csvs[i]['y'], c=csvs[i]['cluster'], cmap='rainbow', s=1)
# Disminuir tamaño de puntos
for i, ax in enumerate(axes):
    if i < len(csvs):
        ax.scatter(csvs[i]['x'], csvs[i]['y'], c=csvs[i]['cluster'], cmap='rainbow', s=1)
plt.show()

# Borrar ficheros temporales
for dir in dirs:
    os.remove(dir)