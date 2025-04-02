# Guide d'utilisation du template `iritTemplateFunction.typ`

Ce document explique comment utiliser le template `iritTemplateFunction.typ` pour générer un document avec une mise en page standardisée.

## 1. Présentation du Template

Le template permet de structurer un document en définissant :
- Les couleurs principales (bleu et orange IRIT)
- Les auteurs et leur affichage en pied de page
- Une en-tête avec logo et description
- Un titre principal et un sous-titre
- Une mise en page justifiée
- Des sections numérotées avec des niveaux de titres
- Une table des matières

## 2. Utilisation

### 2.1. Importation du template

Dans votre fichier `.typ`, commencez par importer le template :

```typst
#import "iritTemplateFunction.typ": *
```

### 2.2. Définition des variables

Définissez les variables principales, notamment les auteurs et la description de l'en-tête :

```typst
#let auteur = ("clement.aldebert31@gmail.com", "laurentpolzin@gmail.com")
#let description = "Aldebert Clément et Polzin Laurent\nÉtudiants à l'université Paul Sabatier"
```

### 2.3. Initialisation du document

Appelez la fonction `project.with` pour initialiser le document avec ses paramètres :

```typst
#show: project.with(
  title: "Rapport",
  subtitle: "Sous-titre",
  authors: auteur,
  description : description
)
```

### 2.4. Ajout d'une table des matières

Pour générer une table des matières automatique :

```typst
#outline()
```

### 2.5. Saut de page

Ajoutez un saut de page si nécessaire :

```typst
#pagebreak()
```

### 2.6. Création de sections

Utilisez la fonction `titre` pour créer des sections avec différents niveaux :

```typst
#titre("Titre de niveau 1", 1) \
#lorem(600)

#titre("Titre de niveau 2", 2) \
#lorem(600)

#titre("Titre de niveau 3", 3) \
#lorem(600)

#titre("Titre de niveau 4", 4) \
#lorem(600)
```

## 3. Résumé

En suivant ces étapes, vous pouvez générer un document bien structuré avec une mise en page cohérente et un formatage standardisé. Ce template permet une personnalisation facile des titres, auteurs, descriptions et contenus.

N'hésitez pas à adapter et enrichir le modèle selon vos besoins !

