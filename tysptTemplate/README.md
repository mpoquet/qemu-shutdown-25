# User Guide for the `iritTemplateFunction.typ` Template

This document explains how to use the `iritTemplateFunction.typ` template to generate a document with standardized layout.

## 1. Template Overview

The template allows structuring a document by defining:
- Main colors (IRIT blue and orange)
- Authors and their display in the footer
- A header with logo and description
- A main title and a subtitle
- Justified layout
- Numbered sections with heading levels
- A table of contents

## 2. Usage

### 2.1. Importing the Template

In your `.typ` file, start by importing the template:

```typst
#import "iritTemplateFunction.typ": *
````

### 2.2. Defining Variables

Define the main variables, notably the authors and the header description:

```typst
#let auteur = ("clement.aldebert31@gmail.com", "laurentpolzin@gmail.com")
#let description = "Aldebert Clément et Polzin Laurent\nÉtudiants à l'université Paul Sabatier"
```

### 2.3. Initializing the Document

Call the `project.with` function to initialize the document with its parameters:

```typst
#show: project.with(
  title: "Rapport",
  subtitle: "Sous-titre",
  authors: auteur,
  description : description
)
```

### 2.4. Adding a Table of Contents

To automatically generate a table of contents:

```typst
#outline()
```

### 2.5. Page Break

Add a page break if needed:

```typst
#pagebreak()
```

### 2.6. Creating Sections

Use the `titre` function to create sections with different levels:

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

## 3. Summary

By following these steps, you can generate a well-structured document with a coherent layout and standardized formatting. This template allows easy customization of titles, authors, descriptions, and content.

