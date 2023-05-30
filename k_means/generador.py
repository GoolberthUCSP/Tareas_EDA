from sklearn.cluster import kmeans_plusplus
from sklearn.datasets import make_blobs
import matplotlib.pyplot as plt
import os

# Generate sample data
n_samples = 40000
list_components = [2, 5, 10, 20, 50, 100]

# Create data for clustering with 2, 5, 10, 20, 50, 100 centers
for n_components in list_components:
    X, y_true = make_blobs(
        n_samples=n_samples, centers=n_components, cluster_std=0.60, random_state=0
    )

    # Save data
    with open(os.getcwd() + f"\Tareas_EDA\k_means\data\puntos_{n_components}_bloques.txt", "w") as f:
        for i in range(n_samples):
            f.write(f"{X[i][0]} {X[i][1]}\n")