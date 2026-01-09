### Projet : Optimisation de tournées touristiques par l’IA

### Problème : 
Pour une instance donnée contenant :

- une liste d’hôtels  
- une liste de points d’intérêt (POI) avec leurs caractéristiques  
- un nombre de jours  

Nous devons construire une solution optimale pour chaque jour, tout en respectant les contraintes suivantes :

- Chaque journée dispose d’une durée maximale de déplacement à ne pas dépasser.

- Chaque point d’intérêt ne peut être visité qu’une seule fois sur l’ensemble du séjour.  

L’objectif est de maximiser le score global de la solution proposée.


### Binôme : 
Mamadou COULIBALY
Radia MERABTENE



### Démarche : 
- 1. Solution initiale

Nous avons commencé par concevoir une solution initiale capable de fournir une solution faisable pour les 27 instances fournies dans les fichiers de test.

L’idée principale est la suivante :

Déterminer d’abord l’itinéraire des hôtels.  
Ensuite, remplir chaque journée (entre deux hôtels successifs) avec le nombre maximal de points d’intérêt possibles, tout en respectant la contrainte de temps.  

Cette approche présente cependant certaines limites :

- Elle restreint fortement le choix des POI entre deux hôtels.  
- Elle limite la possibilité de repasser par un même hôtel plusieurs fois, ce qui peut conduire à des solutions moins optimales.

- 2. Amélioration de la solution initiale

Afin d’améliorer la qualité de la solution initiale, nous avons appliqué des techniques de recherche locale, en alternant deux phases :  
-  Diversification  
-  Intensification

Les opérateurs de voisinage utilisés sont :

Swap : échange de deux points d’intérêt afin d’explorer de nouvelles configurations.

2-opt : inversion de l’ordre de visite d’une sous-liste de POI pour améliorer le chemin parcouru.

### Outils de dévloppement : 
C++ et librairie STL.