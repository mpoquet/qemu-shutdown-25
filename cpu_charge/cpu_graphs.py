import numpy as np
import matplotlib.pyplot as plt

# from file to float array
def tab_from_file(nom_fichier):
    with open(nom_fichier, 'r') as f:
        contenu = f.read()
        liste_floats = [float(val) for val in contenu.split()]
    
    return liste_floats

# même taille de liste
def align_lists(list1, list2):
    max_len = max(len(list1), len(list2))
    list1 += [0] * (max_len - len(list1))
    list2 += [0] * (max_len - len(list2))
    return list1, list2

# transformation en graphe
def to_graph():
    values_wfi = tab_from_file("wfi_moyenne.data")
    values_wfe = tab_from_file("wfe_moyenne.data")

    nb_values = len(values_wfe)
    
    y = values_wfe + values_wfi
    x = np.arange(len(y))

    plt.figure(figsize=(15,8))
    plt.plot(x, y, "b-", label="Wfe vs wfi sur l'utilisation des 4 coeurs sur 10 secondes")

    plt.axvline(x=0, color='red', linestyle='--')
    plt.text(0, max(y)*0.95, 'WFE', color='red', fontsize=12)

    plt.axvline(x=nb_values, color='red', linestyle='--')
    plt.text(nb_values + 5, max(y)*0.95, 'WFI', color='red', fontsize=12)

    plt.xlabel("Temps")
    plt.ylabel("Charge des coeurs")
    plt.grid()
    #plt.show() # empeche l'enregistrement de l'image

    plt.savefig("graph_cpu_usage.png")
    print("Veuillez vérifier le graphe : graph_cpu_usage.png")

    erreur = np.abs(np.mean(values_wfe) - np.mean(values_wfi))
    print(f"Il y a environ {erreur} coeurs supplémentaires qui sont utilisés pour le programme avec WFE")

if __name__ == "__main__":
    to_graph()
