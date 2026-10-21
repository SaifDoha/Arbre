
#include <iostream>
#include <queue>
using namespace std;

struct Noeud {
    int valeur;
    Noeud* gauche;
    Noeud* droite;
    bool marque;
};


Noeud* rechercher(Noeud* racine, int valeur) {
    if (racine == nullptr)
        return nullptr;

    if (racine->valeur == valeur)
        return racine;

    Noeud* trouve = rechercher(racine->gauche, valeur);

    if (trouve != nullptr)
        return trouve;

    return rechercher(racine->droite, valeur);
}

void insererGauche(Noeud* racine, int parent, int valeur) {
    Noeud* p = rechercher(racine, parent);

    if (p == nullptr) {
        cout << "Parent " << parent << " introuvable !" << endl;
    }
    else if (p->gauche != nullptr) {
        cout << "Le fils gauche de " << parent
             << " existe deja !" << endl;
    }
    else {
        p->gauche = new Noeud(valeur);
    }
}

void insererDroite(Noeud* racine, int parent, int valeur) {
    Noeud* p = rechercher(racine, parent);

    if (p == nullptr) {
        cout << "Parent " << parent << " introuvable !" << endl;
    }
    else if (p->droite != nullptr) {
        cout << "Le fils droit de " << parent
             << " existe deja !" << endl;
    }
    else {
        p->droite = new Noeud(valeur);
    }
}

/// Parcours en largeur avec marquage

void parcoursLargeur(Noeud* racine) {
    //test si l arbre est vide
    if (racine == nullptr)
        return;

    queue<Noeud*> file;
  //on marque la racine comme vistiter et on l enfile
    racine->marque = true;
    file.push(racine);

    while (!file.empty()) {
        //recupere le premier element de la file
        Noeud* courant = file.front();
        //je l affiche
        cout << courant->valeur << " ";
        // j insere c est fils droite et gauche
        if (courant->gauche != nullptr && !courant->gauche->marque) {
            courant->gauche->marque = true;
            file.push(courant->gauche);
        }
        if (courant->droite != nullptr && !courant->droite->marque) {
            courant->droite->marque = true;
            file.push(courant->droite);
        }
        // je le defile
        file.pop();

    }
}

int main() {
    Noeud* racine = new Noeud(7);
    insererGauche(racine, 7, 9);
    insererDroite(racine, 7, 67);
    insererGauche(racine, 9, 1);
    insererDroite(racine, 9, 28);
    insererGauche(racine, 67, 8);
    insererDroite(racine, 67, 10);
    insererGauche(racine, 28, 29);
    insererDroite(racine, 28, 2);
    insererDroite(racine, 8, 57);
    insererGauche(racine, 10, 40);
    insererDroite(racine, 10, 6);

    cout << "Parcours en largeur : ";
    parcoursLargeur(racine);
    cout << endl;

    return 0;
}


