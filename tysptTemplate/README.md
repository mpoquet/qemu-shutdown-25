# README - Template Typst IRIT

## Description
Ce projet contient un template Typst pour la rédaction de rapports à l'Institut de Recherche en Informatique de Toulouse (IRIT). Il est basé sur le template LaTeX réalisé par M. Poquet pour l'IRIT, présent dans le projet QEMU disponible sur GitLab : [irit-latex-pandoc-template](https://gitlab.irit.fr/poquet/irit-latex-pandoc-template.git). Ce template permet de structurer un document avec une mise en page spécifique intégrant le logo de l'IRIT et une présentation des auteurs.

## Structure du projet
Le projet est composé des fichiers suivants :

- **`irit-logo.svg`** : Le logo officiel de l'IRIT qui sera affiché sur la page de titre.
- **`main.typ`** : Le fichier principal contenant la structure du document et le texte.

## Explication du template

Le fichier `main.typ` définit les éléments suivants :

1. **Définition des couleurs**
   ```typst
   #let blue-irit = rgb(0, 86, 112)
   #let orange-irit = rgb(255, 76, 0)
   ```
   Ces variables définissent les couleurs officielles de l'IRIT utilisées dans le document.

2. **Configuration du texte**
   ```typst
   #set text(lang: "fr", font: "New Computer Modern Sans", size: 12pt)
   ```
   Cette ligne configure la langue, la police et la taille du texte.

3. **Disposition du logo et des informations des auteurs**
   ```typst
   #grid(columns: (25%, 60%),
     gutter: 5em,
     align: horizon,
     grid.cell(image("irit-logo.svg")),
     grid.cell(
       text()[
           *#text(fill: blue-irit, "Institut de Recherche en Informatique de Toulouse")* \
           Aldebert Clément et Polzin Laurent\
           Étudiants à l'université Paul Sabatier \
           Université Paul Sabatier, IRIT \
           118 Route de Narbonne, 31062 Toulouse Codex 9, France \
           *#text(fill: orange-irit, "clement.aldebert31@gmail.com")*
           *#text(fill: orange-irit, "laurentpolzin@gmail.com")*
         ])
   )
   ```
   Cette section crée une grille avec deux colonnes : l'une contenant le logo et l'autre contenant les informations des auteurs.

4. **Ajout d'une ligne décorative**
   ```typst
   #align(top + center)[
     #line(length: 120%)
   ]
   ```
   Cette ligne horizontale permet de séparer la section des auteurs du reste du document.

5. **Titre et sous-titre**
   ```typst
   #align(center)[
     = #text(fill: blue-irit,"Title")
     
     === #text(fill: blue-irit, "Sub-title")
   ]
   ```
   Cette section centre et stylise le titre et le sous-titre en utilisant la couleur de l'IRIT.

6. **Pied de page**
   ```typst
   #align(bottom)[
     #line(length: 100%, stroke: orange-irit)
     #text(orange-irit)[clement.aldebert31\@gmail.com]
     #text(orange-irit)[laurentpolzin\@gmail.com]
   ]
   ```
   Cette partie ajoute une ligne colorée et affiche les adresses e-mail des auteurs en bas de la page.

7. **Saut de page**
   ```typst
   #pagebreak()
   ```
   Permet d'assurer une séparation claire entre la couverture et le contenu du rapport.

## Utilisation
1. Installer Typst si ce n'est pas déjà fait.
2. Ouvrir le fichier `main.typ` avec un éditeur Typst.
3. Compiler le document pour générer le rapport.

## Auteurs
- Clément Aldebert ([clement.aldebert31@gmail.com](mailto:clement.aldebert31@gmail.com))
- Laurent Polzin ([laurentpolzin@gmail.com](mailto:laurentpolzin@gmail.com))

---
Ce template est conçu pour simplifier la création de rapports académiques avec une mise en page professionnelle et institutionnelle, en reprenant l'esthétique du template LaTeX de M. Poquet.


