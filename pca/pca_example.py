# Carlos Atencio Torres
# Universidad Catolica San Pablo
# Arequipa, 2023
# Laboratorio de Estructura de Datos A.

import numpy as np
from sklearn.decomposition import PCA
from sklearn.datasets import make_blobs
#from mpl_toolkits import mplot3d
import matplotlib.pyplot as plt
from skspatial.objects import Point,Line
from skspatial.plotting import plot_3d

# Genera una figura, parecida a dos telaranhas
def generarFigura(n=200):
    x = [e for e in range(-n//2,n//2)]
    eps = 2.2204460492
    y = [(e)*(np.sin( np.random.uniform(0, 2*np.pi) )) for e in x ]
    z = [(e)*(np.sin( np.random.uniform(0, 2*np.pi) )) for e in x ]
    
    return np.hstack( (np.c_[x], np.c_[y], np.c_[z]))

def generarBlob(n=200):
    X, _ = make_blobs(
        n_samples=n,
        centers=1,
        n_features=3,
        cluster_std=0.40)
    return X


# Generado por Chatgpt
def plano_ortogonal(vector, punto_plano=[0, 0, 0], n=5):
    x_range=(-5, 5)
    y_range=(-5, 5)
    
    # Encontrar dos vectores linealmente independientes al vector dirección
    if vector[0] != 0:
        v1 = np.array([-vector[1] - vector[2], vector[0], vector[0]])
    else:
        v1 = np.array([vector[1], -vector[0] - vector[2], vector[1]])
    v2 = np.cross(vector, v1)

    # Calcular el vector normal al plano ortogonal al vector dirección
    normal = np.cross(v1, v2)

    # Generar los puntos del plano
    x = np.linspace(x_range[0], x_range[1], n)
    y = np.linspace(y_range[0], y_range[1], n)
    X, Y = np.meshgrid(x, y)
    Z = (-normal[0] * X - normal[1] * Y + np.dot(normal, punto_plano)) / normal[2]

    return X, Y, Z

##################################
###########   MAIN   #############
##################################
X = generarFigura(500)

# Para utilizar PCA, debemos de normalizar
X_mean = np.mean(X, axis=0)
X_std = np.std(X, axis=0)
X_normalized = (X - X_mean) / X_std

pca = PCA(n_components=1)
pca.fit(X)

direction = pca.components_[0] #Éis su eigenvector

# Calculando las proyecciones normales
proy = []
for x in X_normalized:
    vector_unitario = direction / np.linalg.norm(direction)
    proyection = np.dot(x, vector_unitario) * vector_unitario
    proy.append( proyection ) 

# Calculando el punto medio de dichas proyecciones
x_proy_mean = np.mean(np.array(proy), axis=0)

# Calculando el plano ortogonal
HX,HY,HZ = plano_ortogonal(direction, x_proy_mean )

##########################
### Zona de impresión  ###
##########################
fig = plt.figure()
ax = plt.axes(projection='3d')

# Imprimiendo los puntos
ax.scatter(X_normalized[:,0], X_normalized[:,1], X_normalized[:,2])

# Imprimiendo la dirección
ax.plot3D(  [-4*direction[0], 4*direction[0]], 
            [-4*direction[1], 4*direction[1]], 
            [-4*direction[2], 4*direction[2]], 'blue')

# Imprimiendo las normales
for i in range(len(X)):
    xt,yt,zt = X_normalized[i]
    ax.plot3D( [xt,proy[i][0]], [yt,proy[i][1]], [zt,proy[i][2]], 'red' )

# Imprimiendo el plano ortogonal (bugueado)
#ax.plot_surface(HX, HY, HZ, alpha=0.5) #Plano

plt.show()
