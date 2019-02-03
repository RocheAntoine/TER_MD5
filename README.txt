Le projet est séparé en deux parties :
    - MPI : programme compilé avec mpicc afin de lancer le programme en MPI
    - ICC : programme compilé avec icc afin de lancer le programme en séquentiel ou avec OMP

MPI :
    Exécutable : mainMPI
    arguments : mot prefixe
        mot : mot à hasher et à bruteforce (chaine de caractères)
        prefixe : taille des prefixes (entier)

    à exécuter via mpirun ou srun

ICC :
    Exécutable : mainICC
    arguments : mot prefixe [o/s]
        mot : mot à hasher et à bruteforce (chaine de caractères)
        prefixe : taille des prefixes (entier)
        o : Exécuter via OMP
        s : Exécuter de manière séquentiel

job.sh :
    Permet de lancer le programme sur Roméo via une soumission de job.
    Nous pouvons le lancer des 3 manières possibles ainsi qu'avec différents paramètres (tel que le nombre de coeurs utilisés).
    Ce script sert principalement à tester l'efficacité du code avec OMP et MPI, seuls les temps d'exécutions en fonctions du nombre de coeurs sont sauvegardés.

compile.sh
    Charge les modules nécessaires (intel et MPI), nettoie et compile les deux programmes.