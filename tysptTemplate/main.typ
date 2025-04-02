// import du template
#import "iritTemplateFunction.typ": *
// mail des auteurs pour les pieds de page
#let auteur = ("clement.aldebert31@gmail.com", "laurentpolzin@gmail.com")
// description dans l'en-tête
#let description = "Aldebert Clément et Polzin Laurent\nÉtudiants à l'université Paul Sabatier"
// appel du template avec les paramètres
#show: project.with(
  title: "Rapport",
  subtitle: "Sous-titre",
  authors: auteur, description : description
)

// Table des matières
#outline()
// saut de page
#pagebreak()

// Exemple de contenue
#titre("Titre de niveau 1", 1) \
#lorem(600)

#titre("Titre de niveau 2", 2) \
#lorem(600)

#titre("Titre de niveau 3", 3) \
#lorem(600)

#titre("Titre de niveau 4", 4) \
#lorem(600)

#titre("Titre de niveau 1", 1) \
#lorem(600)

#titre("Titre de niveau 2", 2) \
#lorem(600)

#titre("Titre de niveau 3", 3) \
#lorem(600)

#titre("Titre de niveau 4", 4) \
#lorem(600)
