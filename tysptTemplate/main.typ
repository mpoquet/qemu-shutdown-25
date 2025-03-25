#let blue-irit = rgb(0, 86, 112)
#let orange-irit = rgb(255, 76, 0)
#set text(lang: "fr", font: "New Computer Modern Sans", size: 12pt)
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

\

#align(top + center)[
  #line(length: 120%)
]

\

#align(center)[
  = #text(fill: blue-irit,"Title")
  
  === #text(fill: blue-irit, "Sub-title")
  \
]
\

text ici

#align(bottom)[
  #line(length: 100%, stroke: orange-irit)
  #text(orange-irit)[clement.aldebert31\@gmail.com]
  #text(orange-irit)[laurentpolzin\@gmail.com]
]
#pagebreak()
