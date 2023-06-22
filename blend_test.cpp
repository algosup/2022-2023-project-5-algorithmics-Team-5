#include <iostream>
#include <vector>
#include <cmath>
#include <limits>

using namespace std;

class Cuve {
public:
    Cuve(double volume) : volume_(volume) {}
    double getVolume() const { return volume_; }

private:
    double volume_;
};

string generateMemoKey(double volumeRestant, int index) {
    return to_string(volumeRestant) + "_" + to_string(index);
}

double calculMeilleureCombinaison(vector<Cuve>& cuves, double volumeRestant, int index, vector<int>& memo) {
    if (index < 0 || volumeRestant <= 0) {
        return 0;
    }

    string memoKey = generateMemoKey(volumeRestant, index);
    if (memo[index] != -1) {
        return memo[index];
    }

    double meilleureDifference = numeric_limits<double>::max();
    int meilleureCombinaison = 0;

    for (int i = 0; i <= index; i++) {
        double difference = abs(volumeRestant - cuves[i].getVolume());
        double prochaineCombinaison = calculMeilleureCombinaison(cuves, volumeRestant - cuves[i].getVolume(), i - 1, memo);

        if (difference < meilleureDifference) {
            meilleureDifference = difference;
            meilleureCombinaison = prochaineCombinaison + 1;
        }
    }

    memo[index] = meilleureCombinaison;
    return meilleureCombinaison;
}

void testCalculMeilleureCombinaison() {
    vector<Cuve> cuves = {
        Cuve(10.0),
        Cuve(20.0),
        Cuve(30.0)
    };

    double volumeRestant = 50.0;
    int index = cuves.size() - 1;
    vector<int> memo(index + 1, -1);

    int resultat = calculMeilleureCombinaison(cuves, volumeRestant, index, memo);

    // Scénario 1
    int resultatAttendu1 = 2; // Le résultat attendu est 2
    if (resultat == resultatAttendu1) {
        cout << "Scénario 1 : Le test a réussi !" << endl;
    } else {
        cout << "Scénario 1 : Le test a échoué. Résultat obtenu : " << resultat << ", Résultat attendu : " << resultatAttendu1 << endl;
    }

    // Scénario 2
    cuves = {
        Cuve(15.0),
        Cuve(25.0),
        Cuve(35.0),
        Cuve(40.0)
    };
    volumeRestant = 60.0;
    index = cuves.size() - 1;
    memo = vector<int>(index + 1, -1);
    int resultatAttendu2 = 2; // Le résultat attendu est 2
    resultat = calculMeilleureCombinaison(cuves, volumeRestant, index, memo);
    if (resultat == resultatAttendu2) {
        cout << "Scénario 2 : Le test a réussi !" << endl;
    } else {
        cout << "Scénario 2 : Le test a échoué. Résultat obtenu : " << resultat << ", Résultat attendu : " << resultatAttendu2 << endl;
    }

    // Scénario 3
    cuves = {
        Cuve(5.0),
        Cuve(15.0),
        Cuve(25.0),
        Cuve(35.0)
    };
    volumeRestant = 45.0;
    index = cuves.size() - 1;
    memo = vector<int>(index + 1, -1);
    int resultatAttendu3 = 3; // Le résultat attendu est 3
    resultat = calculMeilleureCombinaison(cuves, volumeRestant, index, memo);
    if (resultat == resultatAttendu3) {
        cout << "Scénario 3 : Le test a réussi !" << endl;
    } else {
        cout << "Scénario 3 : Le test a échoué. Résultat obtenu : " << resultat << ", Résultat attendu : " << resultatAttendu3 << endl;
    }
}

void testGenerateMemoKey() {
    // Scénario 1
    double volumeRestant1 = 10.0;
    int index1 = 2;
    string resultatAttendu1 = "10_2"; // Le résultat attendu est "10_2"
    string resultat1 = generateMemoKey(volumeRestant1, index1);
    if (resultat1 == resultatAttendu1) {
        cout << "Scénario 1 : Le test a réussi !" << endl;
    } else {
        cout << "Scénario 1 : Le test a échoué. Résultat obtenu : " << resultat1 << ", Résultat attendu : " << resultatAttendu1 << endl;
    }

    // Scénario 2
    double volumeRestant2 = 25.0;
    int index2 = 3;
    string resultatAttendu2 = "25_3"; // Le résultat attendu est "25_3"
    string resultat2 = generateMemoKey(volumeRestant2, index2);
    if (resultat2 == resultatAttendu2) {
        cout << "Scénario 2 : Le test a réussi !" << endl;
    } else {
        cout << "Scénario 2 : Le test a échoué. Résultat obtenu : " << resultat2 << ", Résultat attendu : " << resultatAttendu2 << endl;
    }

    // Scénario 3
    double volumeRestant3 = 40.0;
    int index3 = 1;
    string resultatAttendu3 = "40_1"; // Le résultat attendu est "40_1"
    string resultat3 = generateMemoKey(volumeRestant3, index3);
    if (resultat3 == resultatAttendu3) {
        cout << "Scénario 3 : Le test a réussi !" << endl;
    } else {
        cout << "Scénario 3 : Le test a échoué. Résultat obtenu : " << resultat3 << ", Résultat attendu : " << resultatAttendu3 << endl;
    }
}

int main() {
    testCalculMeilleureCombinaison();
    testGenerateMemoKey();

    return 0;
}