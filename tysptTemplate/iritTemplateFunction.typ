// Définition des couleurs et auteurs
#let blue-irit = rgb(0, 86, 112)
#let orange-irit = rgb(255, 76, 0)
#let project(title: "", subtitle: "", authors: (), description: (), body) = {
  // Paramètres globaux
  set text(lang: "fr", font: "New Computer Modern Sans", size: 12pt)
  set heading(numbering: "I - 1")
  set page(
    footer: context [
      #line(length: 100%, stroke: orange-irit)
      #for a in authors [
        #text(fill: orange-irit, a)
      ] #h(1fr) #counter(page).display()
    ]
  )
  // En-tête
  grid(
    columns: (25%, 60%),
    gutter: 5em,
    align: horizon,
    grid.cell(image("irit-logo.svg")),
    grid.cell(
      text()[
        *#text(fill: blue-irit, "Institut de Recherche en Informatique de Toulouse")*
        #description
        Université Paul Sabatier, IRIT
        118 Route de Narbonne, 31062 Toulouse Cedex 9, France \ 
        #for a in authors [
          *#text(fill: orange-irit, a)*
        ]
      ]
    )
  )

  text(
    align(top + center)[
      \
      #line(length: 120%)
      \
    ]
  )
  // Titre principal
  align(center)[
    *#text(fill: blue-irit, title, size: 20pt)* \ \
    #text(fill: blue-irit, subtitle, size: 15pt) \ \
  ]
  // Contenu principal
  set par(justify: true)
  body
}

// Fonction pour les titres des sections
#let titre(titre, niveau) = {
  heading(level: niveau, text(fill: blue-irit, titre))
}


