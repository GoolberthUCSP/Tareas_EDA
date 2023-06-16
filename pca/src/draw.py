import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys

fig = plt.figure()
ax = plt.axes(projection='3d')

# Leer puntos en 3D desde points.csv
df = pd.read_csv(sys.path[0]+'\\points.csv', header=None)

# Convertir a numpy array
points = df.to_numpy()

# Extraer el primer elemento= vector dirección
direction = points[0]
points = points[1:]

# Ordenar puntos de acuerdo a su factor de proyección
sorted_points = np.array(sorted(points, key=lambda x: np.dot(x, direction)/np.dot(direction, direction), reverse=True))

# Calcular las proyecciones
proyections= np.array([((np.dot(x, direction))/(np.dot(direction, direction)))*direction for x in sorted_points])

# Imprimir la recta dirección desde el menor punto proyectado al mayor
ax.plot3D(  [proyections[-1][0], proyections[0][0]],
            [proyections[-1][1], proyections[0][1]],
            [proyections[-1][2], proyections[0][2]], 'blue')


# Imprimiendo los puntos y sus proyecciones
ax.scatter(sorted_points[:,0], sorted_points[:,1], sorted_points[:,2])
#ax.scatter(proyections[:,0], proyections[:,1], proyections[:,2]) # Descomentar para imprimir las proyecciones

# Imprimiendo las proyecciones unidas por una línea punteada con los puntos originales
for i in range(len(sorted_points)):
    ax.plot3D(  [sorted_points[i][0], proyections[i][0]], 
                [sorted_points[i][1], proyections[i][1]], 
                [sorted_points[i][2], proyections[i][2]], 'red', linestyle='dashed')

plt.show()