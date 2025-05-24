import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import sys
import pandas as pd
from os.path import isfile, join
from os import listdir

# from file to float array
"""
    permet de lire un fichier
    contenant que des floats
    et de tous les mettre dans une liste
"""
def tab_from_file(nom_fichier):
    with open(nom_fichier, 'r') as f:
        contenu = f.read()
        liste_floats = [float(val) for val in contenu.split()]
    
    return liste_floats

"""
    Aligne 2 listes :
        2 possibilités : 
        - on garde la plus grande et pour la plus petite on ajoute des 0
            ou on repete un terme par exemple
        - on coupe la plus grande pour qu'elle ait la 
            meme taille que la petite (ce que j'ai fait ici)
"""
def align_lists(list1, list2):
    min_len = min(len(list1), len(list2))
    list1 = list1[:min_len]
    list2 = list2[:min_len]

    return list1, list2

"""
    Trouve quand la moyenne se stabilise et retourne 
    la moyenne avant et après la stabilisation
    
    fonction qui semblait etre utile pour mon cas,
    mais ce n'était que lié aux problèmes engendrés
    de faire tourner QEMU sur une VM Linux (gros pics 
    de puissance au debut et se calme apres, supposition :
    la phase de boot consommait beaucoup). Au final, 
    en faisant tourner le programme chez Clément, 
    il n'y a pas de "phase de boot"
"""
def moyenne_avant_apres_boot(liste_wfi):
    moyenne_cur = 0
    
    for i in range(len(liste_wfi)):
        moyenne_cur = np.mean(np.array(liste_wfi[i:]))
        if ((liste_wfi[i] - moyenne_cur) <= 0.07): # lorsque ca se stabilise
            index = i
            break

    wfi_moy_avant = np.mean(liste_wfi[:index])
    wfi_moy_apres = np.mean(liste_wfi[index:])

    return wfi_moy_avant, wfi_moy_apres 

# transformation en graphe
"""
    Lit les fichiers transformés (_filtered) et les transformes
    en interval de confiance plutot qu'une moyenne simple

    Permet d'observer la différence entre WFE et WFI
"""
def to_graph(tps, nb_l):
    directory = "./cpu_datas"
    files = [f for f in listdir(directory) if isfile(join(directory, f))]
    data = []
    wfi_mean = 0
    wfe_mean = 0

    for file in files:
        if "wfi_filtered" in file.lower():
            label = "WFI"
        elif "wfe_filtered" in file.lower():
            label = "WFE"
        else:
            continue  # raw perf stat records

        filepath = join(directory, file)
        values = tab_from_file(filepath)
        data.append((label, values))
        if label == "WFE":
            wfe_mean += np.mean(values)
        else:
            wfi_mean += np.mean(values)

    if not data:
        print("Aucune donnée trouvée.")
        return

    # longueur minimale
    min_len = min(len(d[1]) for d in data)
    x = np.linspace(0, int(tps), min_len)

    df_list = []
    for label, values in data:
        values = values[:min_len]
        df_temp = pd.DataFrame({
            "Temps (s)": x,
            "Charge des coeurs": values,
            "Programme": label
        })
        df_list.append(df_temp)

    df = pd.concat(df_list, ignore_index=True)


    plt.figure(figsize=(15, 8))
    sns.set_theme(style="whitegrid", font_scale=1.5)

    ax = sns.lineplot(data=df, x="Temps (s)", y="Charge des coeurs", hue="Programme", errorbar=('ci', 95))
    ax.set_xlabel("Temps (s)", fontsize=14)
    ax.set_ylabel("Charge des coeurs", fontsize=14)
    ax.set_title(f"Utilisation CPU : WFE vs WFI, moyenne avec interval de confiance à 95% sur {nb_l} lancements pendants {tps} secondes.", fontsize=16)

    ax.grid(True)

    plt.ylim(0, 5)
    plt.savefig("graph_cpu_usage.png")
    plt.close() 

    wfe_mean /= nb_l
    wfi_mean /= nb_l
    diff_error = np.abs(wfe_mean - wfi_mean)

    print("Veuillez vérifier le graphe : graph_cpu_usage.png")
    print(f"Il y a environ {diff_error:.2f} coeurs supplémentaires qui sont utilisés pour le programme avec WFE")


if __name__ == "__main__":
    to_graph(int(sys.argv[1]), int(sys.argv[2]))
