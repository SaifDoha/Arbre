#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <cstdlib>

using namespace std;
using Horloge = chrono::high_resolution_clock;

long long nbComparaisons = 0;

struct Comparateur {
    bool operator()(int a, int b) const {
        nbComparaisons++;
        return a < b;
    }
};

struct Noeud {
    int valeur;
    Noeud* gauche;
    Noeud* droite;
};

// ---------------- ABR (versions sans affichage) ----------------
Noeud* inserer(Noeud* racine, int valeur) {
    if (racine == nullptr)
        return new Noeud{valeur, nullptr, nullptr};
    if (valeur < racine->valeur)
        racine->gauche = inserer(racine->gauche, valeur);
    else if (valeur > racine->valeur)
        racine->droite = inserer(racine->droite, valeur);
    return racine;
}

Noeud* rechercher(Noeud* racine, int valeur, int& parcourus) {
    if (racine == nullptr) return nullptr;
    parcourus++;
    if (racine->valeur == valeur) return racine;
    if (valeur < racine->valeur)
        return rechercher(racine->gauche, valeur, parcourus);
    return rechercher(racine->droite, valeur, parcourus);
}

Noeud* supprimer(Noeud* racine, int valeur, bool& supprime) {
    if (racine == nullptr) return nullptr;
    if (valeur < racine->valeur)
        racine->gauche = supprimer(racine->gauche, valeur, supprime);
    else if (valeur > racine->valeur)
        racine->droite = supprimer(racine->droite, valeur, supprime);
    else {
        supprime = true;
        if (racine->gauche == nullptr && racine->droite == nullptr) {
            delete racine;
            return nullptr;
        }
        if (racine->gauche == nullptr) {
            Noeud* temp = racine->droite;
            delete racine;
            return temp;
        }
        if (racine->droite == nullptr) {
            Noeud* temp = racine->gauche;
            delete racine;
            return temp;
        }
        Noeud* successeur = racine->droite;
        while (successeur->gauche != nullptr)
            successeur = successeur->gauche;
        racine->valeur = successeur->valeur;
        bool temp = false;
        racine->droite = supprimer(racine->droite, successeur->valeur, temp);
    }
    return racine;
}

void liberer(Noeud* racine) {
    if (racine != nullptr) {
        liberer(racine->gauche);
        liberer(racine->droite);
        delete racine;
    }
}

double ms(Horloge::time_point d, Horloge::time_point f) {
    return chrono::duration<double, milli>(f - d).count();
}

// ---------------- Un test complet (insertion, recherche, suppression) ----------------
struct Ligne {
    long long n, nDist;
    double insABR, insMAP;
    int valR;  int noeudsABR;  long long compMAP;  double rechABR, rechMAP;  bool trouve;
    int valS;  double suppABR, suppMAP;  bool supprime;
};

Ligne tester(const vector<int>& vals, long long n, int valR, int valS) {
    Ligne L;
    L.n = n; L.valR = valR; L.valS = valS;

    Noeud* racine = nullptr;
    map<int, int, Comparateur> m;

    // Insertion
    auto d = Horloge::now();
    for (long long i = 0; i < n; i++) racine = inserer(racine, vals[i]);
    L.insABR = ms(d, Horloge::now());

    d = Horloge::now();
    for (long long i = 0; i < n; i++) m.emplace(vals[i], vals[i]);
    L.insMAP = ms(d, Horloge::now());
    L.nDist = (long long)m.size();

    // Recherche
    int parcourus = 0;
    d = Horloge::now();
    Noeud* t = rechercher(racine, valR, parcourus);
    L.rechABR = ms(d, Horloge::now());
    L.noeudsABR = parcourus;
    L.trouve = (t != nullptr);

    nbComparaisons = 0;
    d = Horloge::now();
    m.find(valR);
    L.rechMAP = ms(d, Horloge::now());
    L.compMAP = nbComparaisons;

    // Suppression
    bool sup = false;
    d = Horloge::now();
    racine = supprimer(racine, valS, sup);
    L.suppABR = ms(d, Horloge::now());
    L.supprime = sup;

    d = Horloge::now();
    m.erase(valS);
    L.suppMAP = ms(d, Horloge::now());

    liberer(racine);
    return L;
}

// ---------------- Affichage ----------------
string avecEtoile(int v, bool ok) { return to_string(v) + (ok ? "" : "*"); }

void afficher(const string& titre, const vector<Ligne>& R, bool avecDist) {
    int largeur = avecDist ? 114 : 104;
    cout << "\n" << string(largeur, '=') << "\n " << titre << "\n" << string(largeur, '=') << "\n";
    cout << right << setw(10) << "n";
    if (avecDist) cout << setw(7) << "dist.";
    cout << " |" << setw(12) << "ins ABR" << setw(12) << "ins MAP"
         << " |" << setw(7) << "val" << setw(8) << "noeuds" << setw(6) << "comp"
         << setw(9) << "ABR" << setw(9) << "MAP"
         << " |" << setw(7) << "val" << setw(9) << "ABR" << setw(9) << "MAP" << "\n";
    cout << setw(10) << "";
    if (avecDist) cout << setw(7) << "";
    cout << " |" << setw(24) << "INSERTION (ms)"
         << " |" << setw(7) << "" << setw(14) << "RECHERCHE" << setw(7) << "(ms)" << setw(11) << ""
         << " |" << setw(7) << "" << setw(18) << "SUPPRESSION (ms)" << "\n";
    cout << string(largeur, '-') << "\n";

    cout << fixed << setprecision(4);
    for (const Ligne& L : R) {
        cout << setw(10) << L.n;
        if (avecDist) cout << setw(7) << L.nDist;
        cout << " |" << setw(12) << L.insABR << setw(12) << L.insMAP
             << " |" << setw(7) << avecEtoile(L.valR, L.trouve) << setw(8) << L.noeudsABR << setw(6) << L.compMAP
             << setw(9) << L.rechABR << setw(9) << L.rechMAP
             << " |" << setw(7) << avecEtoile(L.valS, L.supprime) << setw(9) << L.suppABR << setw(9) << L.suppMAP << "\n";
    }
    cout << string(largeur, '-') << "\n";
    cout << " * = valeur absente de l'arbre (pas de suppression / valeur introuvable)\n";
}

struct Cas { long long n; int valR; int valS; };

int main() {
    cout << "Tests du rapport : ABR contre std::map (calcul en cours, environ 1 minute)...\n" << flush;

    // ===== 1. Valeurs contigues 0, 1, ..., n-1 =====
    vector<Cas> casC = {
        {50, 25, 13}, {100, 60, 98}, {100, 47, 20}, {500, 477, 99},
        {1000, 799, 289}, {5000, 4767, 292}, {10000, 7677, 38},
        {10000, 8989, 2882}, {10000, 9999, 7288}, {20000, 5775, 3993},
        {30000, 4567, 9999}
    };
    vector<int> contigu(30000);
    for (int i = 0; i < 30000; i++) contigu[i] = i;

    vector<Ligne> resC;
    for (const Cas& c : casC)
        resC.push_back(tester(contigu, c.n, c.valR, c.valS));

    // ===== 2. Valeurs aleatoires dans [1,1000] (meme suite pour toutes les tailles) =====
    vector<Cas> casA = {
        {100, 772, 770}, {1000, 189, 579}, {10000, 565, 682},
        {50000, 923, 892}, {100000, 244, 765}, {500000, 910, 881},
        {1000000, 792, 872}, {10000000, 872, 872}, {100000000, 847, 884}
    };
    vector<int> alea(100000000);
    srand(1);
    for (size_t i = 0; i < alea.size(); i++) alea[i] = rand() % 1000 + 1;

    vector<Ligne> resA;
    for (const Cas& c : casA)
        resA.push_back(tester(alea, c.n, c.valR, c.valS));

    // ===== Affichage =====
    afficher("VALEURS CONTIGUES (0..n-1) : insertion, recherche, suppression", resC, false);
    afficher("VALEURS ALEATOIRES [1,1000] : insertion, recherche, suppression", resA, true);
    cout << "\nnoeuds = noeuds visites (ABR) | comp = comparaisons (MAP) | dist. = elements distincts\n";
    return 0;
}
