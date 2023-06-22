#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <cfloat>
#include <cmath>
#include <numeric>
#include <set>


using namespace std;

// Structure to store the tank information
struct Cuve {
  string number;
  double quantite;
  vector<string> vins_contenu;
  vector<double>volumeUtilise;
  bool aSubiTransfert;
  double capacite;
};

// Structure to store remaining volumes for each wine in each tank
struct VolumeRestant {
  string vin;
  double volume;
  int cuveIndex;
};

// Function to generate a key for the memoization
std::string generateMemoKey(double volumeRestant, int index) {
  return std::to_string(volumeRestant) + "_" + std::to_string(index);
}

// Function to find the best combination of tanks to fill the remaining volume
void trouverCombinaisonCuves(vector<Cuve>& cuves, double volumeRestant, vector<int>& meilleureCombinaison, double& meilleureDifference, vector<int>& combinaisonActuelle, int index, unordered_map<string, pair<double, vector<int>>>& memo) {
  if (volumeRestant < 0.1) {
    double difference = std::abs(volumeRestant);
    if (difference < meilleureDifference) {
      meilleureDifference = difference;
      meilleureCombinaison = combinaisonActuelle;
    }
    return;
  }

 // Check if the combination has already been evaluated
  string memoKey = generateMemoKey(volumeRestant, index);
  if (memo.find(memoKey) != memo.end()) {
    pair<double, vector<int>>& memoEntry = memo[memoKey];
    if (memoEntry.first < meilleureDifference) {
      meilleureDifference = memoEntry.first;
      meilleureCombinaison = memoEntry.second;
    }
    return;
  }
  // Iterate over the tanks
  for (int i = index; i < cuves.size(); i++) {
    if (cuves[i].quantite >= 0.1 && cuves[i].vins_contenu[0] == "/" && std::find(meilleureCombinaison.begin(), meilleureCombinaison.end(), i) == meilleureCombinaison.end()) {
      combinaisonActuelle.push_back(i);

      trouverCombinaisonCuves(cuves, volumeRestant - cuves[i].quantite, meilleureCombinaison, meilleureDifference, combinaisonActuelle, i + 1, memo);

      combinaisonActuelle.pop_back();
    }
  }

// Save the evaluated combination in the memoization
  memo[memoKey] = make_pair(meilleureDifference, meilleureCombinaison);
}

// Function to find the best combination of tanks to fill the remaining volume
bool trouverMeilleureCombinaisonCuves(vector<Cuve>& cuves, double volumeRestant, vector<int>& meilleureCombinaison, double& meilleureDifference) {
  vector<int> combinaisonActuelle;
  unordered_map<string, pair<double, vector<int>>> memo;
  trouverCombinaisonCuves(cuves, volumeRestant, meilleureCombinaison, meilleureDifference, combinaisonActuelle, 0, memo);

  return (meilleureCombinaison.size() > 0);
}
int main() {
  string config_file_path;

  cout << "Drag and drop your config file here and press Enter:" << endl;
  getline(cin, config_file_path);

  // Remove apostrophes from file
  config_file_path.erase(remove(config_file_path.begin(), config_file_path.end(), '\''), config_file_path.end());

 

  // Open the config file
  ifstream config_file(config_file_path);
  if (!config_file.is_open()) {
    cerr << "Impossible to open the config file selected or you have forgot to drag and drop it" << config_file_path << endl;
    return 1;
  }
  else {
     cout << "Reading config file: " << config_file_path << endl;
     
  }

  vector<Cuve> cuves;
  vector<string> vins;
  vector<double> pourcentages;
  vector<double> quantites;
  vector<int> cuves_invalides;
  vector<VolumeRestant> volumesRestants;
  

unordered_set<string> cuveIds;
string line;
int cuveLineCount = 0; // Tank line counter

std::string cheminAcces = "/Users/robindebry/Documents/GitHub/2022-2023-project-5-algorithmics-Team-5/output.txt";
std::ofstream fichier(cheminAcces);
if (fichier.is_open()) {

bool vinExiste = false;
while (getline(config_file, line)) {
  if (line.empty() || line[0] == '!' || line[0] == '\r') {
    continue; // ignore empty lines and comments beginning with '!
  } else if (line[0] == '#') {
    size_t pos1 = line.find(';');
    size_t pos2 = line.find(';', pos1 + 1);

    if (pos1 != string::npos && pos2 != string::npos) {
      string cuveId = line.substr(1, pos1 - 1);

      // Check if the tank ID has already been encountered
      if (cuveIds.count(cuveId) > 0) {
        cerr << "Error: Duplicate tank ID found: " << cuveId << endl;
        return 1;
      }

      // Check if the tank ID is valid
      Cuve cuve;
      cuve.number = cuveId;
      cuve.quantite = stod(line.substr(pos1 + 1, pos2 - pos1 - 1));
      cuve.capacite = stod(line.substr(pos1 + 1, pos2 - pos1 - 1));
      if (cuve.quantite <= 0 || cuve.quantite > 1000) {
        cerr << "Error: Invalid tank volume for tank " << cuve.number << ". Volume should be between 0 and 1000." << endl;
        return 1;
      }

      // Check if a second tank quantity is entered
      size_t pos3 = line.find(';', pos2 + 1);
      if (pos3 != string::npos) {
        cerr << "Error: Multiple tanks quantities specified for tank " << cuve.number << ". Only the first quantity will be considered." << endl;
        return 1;
      }

      string cuve_vin = line.substr(pos2 + 1, pos3 - pos2 - 1);
      string vin;
      bool found = false;

      // Check that the wine name matches exactly one of the wines in the list
      if (cuve_vin == "champagne" || cuve_vin == "/" || cuve_vin == "Champagne") {
        vin = cuve_vin;
        found = true;
      } else {
        for (const string& existing_vin : vins) {
          if (existing_vin == cuve_vin) {
            found = true;
            vin = existing_vin;
            break;
          }
        }
      }
      // If the wine name is not found, throw an error
      if (!found) {
        cerr << "Error: Unknown wine specified in the tank: " << cuve_vin << endl;
        return 1;
      }

      cuve.vins_contenu.push_back(vin);
      cuves.push_back(cuve);

      // Add tank ID to all IDs encountered
      cuveIds.insert(cuveId);

      cuveLineCount++;
    }
  } 
  else if (isdigit(line[0])) {
    quantites.push_back(stoi(line));
    if (quantites.size() >= 2) {
      cerr << "Warning: Only the first quantity value will be considered. Ignoring additional values." << endl;
      return 1;
    }
  } else {
    size_t pos = line.find(';');
    if (pos != string::npos) {
      string vin = line.substr(0, pos);

      bool vin_existe = false;
      for (const string& existing_vin : vins) {
        if (existing_vin == vin) {
          vin_existe = true;
          break;
        }
      }
      if (!vin_existe) {
        // The wine name has not yet been encountered, add it to the wines list
        vins.push_back(vin);
        pourcentages.push_back(stod(line.substr(pos + 1)));
      } else {
        cerr << "Error: Duplicate wine name found: " << vin << endl;
        return 1;
      }
    }
  }
}

// Check that all required information is present
if (cuves.empty() || vins.empty() || pourcentages.empty() || quantites.empty()) {
  cerr << "Error: Missing information in the config file" << endl;
  return 1;
}

// Check for negative percentages
double total_percentage = 0.0;
for (double pourcentage : pourcentages) {
  if (pourcentage <= 0) {
    cerr << "Error: Negative percentage values are not allowed." << endl;
    return 1;
  }
  total_percentage += pourcentage;
}

if (total_percentage < 100.0) {
  cerr << "Error: Total percentage is less than 100. Required: " << (100.0 - total_percentage) << " more." << endl;
  return 1;
}

if (total_percentage > 100.0) {
  cerr << "Error: Total percentage exceeds 100. Required: " << (total_percentage - 100.0) << " less." << endl;
  return 1;
}

for (auto& cuve : cuves) {
  // Check if volume exceeds 1000
  if (cuve.quantite > 1000) {
    cerr << "Error: Volume exceeds the maximum limit of 1000 for tank " << cuve.number << endl;
    return 1;
  }

  for (int i = 0; i < vins.size(); i++) {
    if (find(cuve.vins_contenu.begin(), cuve.vins_contenu.end(), vins[i]) != cuve.vins_contenu.end()) {
      continue;
    }
    if (pourcentages[i] <= ((double)cuve.quantite / 100.0)) {
      cuve.vins_contenu.push_back(vins[i]);
      cuve.quantite -= pourcentages[i] * 100.0;
    }
  }
}

  cout << "Wine: " << endl;
  for (const string& vin : vins) {
    cout << vin << endl;
  }

  cout << "Percentage: " << endl;
  for (double pourcentage : pourcentages) {
    cout << pourcentage << "%" << endl;
  }

  cout << "Quantity needed: " << endl;
  for (double quantite : quantites) {
    cout << quantite << "hL" << endl;
  }

  // Calculate total volume of wine needed
double total_volume = 0;
for (int quantite : quantites) {
  total_volume += quantite;
}

// Calculate required volumes for each wine
vector<double> required_volumes(vins.size());
for (int i = 0; i < vins.size(); i++) {
  required_volumes[i] = total_volume * pourcentages[i] / 100;
}

// Calculate volumes of each wine in each cuve
vector<vector<double>> cuves_volumes(cuves.size(), vector<double>(vins.size(), 0));
for (int i = 0; i < cuves.size(); i++) {
  for (int j = 0; j < vins.size(); j++) {
    cuves_volumes[i][j] = cuves[i].quantite * pourcentages[j] / 100;
  }
}

// Determine tanks to use for each wine
vector<vector<int>> cuves_to_use(vins.size(), vector<int>());
map<string, double> total_volumes;
vector<double> remaining_volumes(vins.size());

for (int i = 0; i < vins.size(); i++) {
  double remaining_volume = required_volumes[i];
  double total_available_volume = 0; // Total volume available in tanks without wine

  for (int j = 0; j < cuves.size() && remaining_volume > 0; j++) {
    if (cuves_volumes[j][i] > 0 && cuves[j].vins_contenu[0] == "/") {
      total_available_volume += cuves_volumes[j][i];
    }
  }

  if (remaining_volume > total_available_volume) {
    cerr << "The volume needed is more than the volume you have inside your empty tanks" << endl;
    return 1;
  }
  // Calculate the remaining volume of wine needed
  for (int j = 0; j < cuves.size() && remaining_volume > 0; j++) {
    if (cuves_volumes[j][i] > 0 && cuves[j].vins_contenu[0] == "/") {
      double volume_to_use = min(remaining_volume, cuves_volumes[j][i]);
      cuves_to_use[i].push_back(j);
      remaining_volume -= volume_to_use;
      total_volumes[vins[i]] += volume_to_use;
    }
  }
}

// Check if the volume used exceeds the desired total volume and remove the last cuve if necessary
double total_used_volume = 0;
for (int i = 0; i < vins.size(); i++) {
  for (int j = 0; j < cuves_to_use[i].size(); j++) {
    int cuve_index = cuves_to_use[i][j];
    total_used_volume += cuves_volumes[cuve_index][i];
  }
}

if (total_used_volume > total_volume) {
  for (int i = 0; i < vins.size(); i++) {
    while (total_used_volume > total_volume && !cuves_to_use[i].empty()) {
      int last_cuve_index = cuves_to_use[i].back();
      double last_cuve_volume = cuves_volumes[last_cuve_index][i];
      cuves_to_use[i].pop_back();
      total_used_volume -= last_cuve_volume;
    }
  }
}

// Calculate total tanks volume for each wine
for (int i = 0; i < vins.size(); i++) {
  double total_cuves_volume = 0;
  double required_volume = 0;

  for (int j = 0; j < cuves.size(); j++) {
    if (find(cuves[j].vins_contenu.begin(), cuves[j].vins_contenu.end(), vins[i]) != cuves[j].vins_contenu.end())
      total_cuves_volume += cuves[j].quantite;
  }

  
  for (int j = 0; j < cuves_to_use[i].size(); j++) {
    int cuve_index = cuves_to_use[i][j];
    required_volume += cuves_volumes[cuve_index][i];
    remaining_volumes[i] = required_volumes[i] - required_volume;
  }

  cout << "Required volume for " << vins[i] << ": " << required_volume << "hL" << endl;

  if (total_cuves_volume < required_volume) {
    cerr << "The volume of " << vins[i] << " is not sufficient in all tanks for the mix" << endl;
    return 1;
  }

  double remaining_volume = total_cuves_volume - required_volume;

  if (remaining_volume < 0) {
    cout << "Impossible to use " << vins[i] << " you need " << (-remaining_volume) << " hL more" << endl;
  } else {
    cout << "Remaining volume for " << vins[i] << ": " << remaining_volume << endl;
  }
}
fichier << "EN" << endl;
fichier << "This is the output file of our solution. It contains the final formula applied, the quantities of leftovers, and all the steps taken to blend the wines in the tanks to achieve the desired mix." << endl;
fichier << endl;
fichier << "- The final formula has been adapted from the one given in the problem to reflect the reality of the tanks and the final mix." << endl;
fichier << "- The leftovers are the quantities of wine remaining in the tanks after the final mix is complete and the tanks containing them." << endl;
fichier << "- The steps are all the operations performed to achieve the final mix, and they are listed in order from first to last." << endl;
fichier << endl;
fichier << "------------" << endl;
fichier << "------------" << endl;
fichier << endl;
fichier << "FR" << endl;
fichier << "Ceci est le fichier de sortie de la solution." << endl; 
fichier << "Il contient la formule finale appliquée, les quantités de restes et toutes les étapes pour mélanger les vins dans les cuves pour obtenir le mélange désiré." <<endl;
fichier << endl;
fichier << "- La formule finale est adaptée de celle donnée dans le problème à la réalité des cuves et du mélange final." << endl;
fichier << "- Les restes sont les quantités de vin restant dans les cuves après que le mélange final soit fait et les cuves les contenant." << endl;
fichier << "- Les étapes sont toutes les opérations effectuées pour obtenir le mélange final, elles sont ordonnées de la première à la dernière." << endl;
fichier << endl;
fichier << "------------" << endl;
fichier << "------------" << endl;
fichier << endl;
std::vector<bool> cuves_deja_ecrites(cuves.size(), false); // Tableau pour suivre les cuves déjà écrites
fichier << "FINAL FORMULA / FORMULE FINALE :" << std::endl;

std::map<std::string, double> transferts_champagne;
std::map<std::string, std::map<int, double>> volumes_transférés;

// Write the final formula
for (int i = 0; i < vins.size(); i++) {
  fichier << std::endl;
  fichier << "=============================================" << std::endl;
  fichier << "For wine " << vins[i] << ":" << std::endl;
  fichier << "=============================================" << std::endl;

  std::map<int, bool> cuves_deja_ecrites;

  for (int j = 0; j < cuves_to_use[i].size(); j++) {
    int cuve_index = cuves_to_use[i][j];
    cuves[cuve_index].vins_contenu[0] = "Champagne";  // Change wine name from "/" to "Champagne

    // Check if the tank has already been written
    if (!cuves_deja_ecrites[cuve_index]) {
      fichier << "Tank : " << cuves[cuve_index].number << std::endl;
      cuves_deja_ecrites[cuve_index] = true; // Mark tank as already written

      // Display wine percentages in this tank
      for (int k = 0; k < vins.size(); k++) {
        double percentage = cuves_volumes[cuve_index][k] / cuves[cuve_index].quantite * 100;
        double volume_equivalent = cuves_volumes[cuve_index][k];
        fichier << vins[k] << ": " << percentage << "% (" << volume_equivalent << "hL)" << std::endl;
      }

      // Add filling steps
      fichier << std::endl;
      double volume_needed = cuves_volumes[cuve_index][i];
      fichier << "Fill steps for tank " << cuves[cuve_index].number << " (" << vins[i] << "):" << std::endl;

      double remaining_volume = volume_needed;
      bool transfer_done = false;
      int k = 0;

      while (remaining_volume > 0 && k < cuves.size()) {
        if (cuves[k].vins_contenu[0] == vins[i] && cuves[k].quantite > 0 && cuves[k].quantite >= 0.001) {
          double available_volume = cuves[k].quantite;
          double transfer_volume = std::min(remaining_volume, available_volume);
          fichier << "  - Transfer " << transfer_volume << "hL from tank " << cuves[k].number << std::endl;
          remaining_volume -= transfer_volume;
          cuves[k].quantite -= transfer_volume;
          transfer_done = true;
          // Check if the remaining volume in the tank is less than 0.001 and set it to 0
          if (cuves_volumes[k][i] < 0.001) {
            cuves_volumes[k][i] = 0.0;
          }
        }
        k++;
      }

      // Check if the transfer is done
      if (remaining_volume > 0 && !transfer_done) {
        fichier << "- Insufficient available volume in other tanks of the same wine to fulfill the requirement." << std::endl;
      } else if (remaining_volume > 0) {
        fichier << "  - Additional volume needed from other sources: " << remaining_volume << "hL" << std::endl;
      } else {
        fichier << endl;
        
      }
    }
  }
}

// Calculate and display total volume for all wines used
double total_volume_all_wines = 0;
for (int i = 0; i < vins.size(); i++) {
  for (int j = 0; j < cuves_to_use[i].size(); j++) {
    int cuve_index = cuves_to_use[i][j];
    total_volume_all_wines += cuves_volumes[cuve_index][i];
  }
}
std::cout << "Total volume for all wines: " << total_volume_all_wines << "hL" << std::endl;


std::cout << "Summary:" << std::endl;
std::cout << "--------" << std::endl;
  
// Calculation of total volume used for each wine
vector<double> volumesUtilises(vins.size(), 0.0);
for (int i = 0; i < cuves.size(); i++) {
  Cuve& cuve = cuves[i];
  for (int j = 0; j < vins.size(); j++) {
    if (find(cuve.vins_contenu.begin(), cuve.vins_contenu.end(), vins[j]) != cuve.vins_contenu.end()) {
      double volumeUtilise = min(volumesUtilises[j], static_cast<double>(cuve.quantite));
      double volumeRestant = max(cuve.quantite - volumeUtilise, 0.0);
      if (volumeRestant == 0) {
        cuve.quantite = 0;
        cuve.vins_contenu[0] = "/";
      } else {
        cuve.quantite = volumeRestant;
      }
      volumesUtilises[j] -= volumeUtilise;

      // Storage of the remaining volume for each wine and each tank
      VolumeRestant volume;
      volume.vin = vins[j];
      volume.cuveIndex = i;
      volume.volume = volumeRestant;
      volumesRestants.push_back(volume);
    }
  }
}

fichier << endl;
fichier << "------------" << endl;
fichier << "------------" << endl;
fichier << endl;
fichier << "STEPS / ETAPES" << endl;
fichier << endl;
fichier << "The steps are indicated by the tank ID from which the wine is taken, the wine name, the quantity of wine taken from the tank, and the tank ID where the wine is placed." << endl;
fichier << "Les étapes sont indiquées par l'ID de la cuve d'où le vin est pris, le nom du vin, la quantité de vin prise dans la cuve, et l'ID de la cuve où le vin est placé." << endl;
fichier << endl;

for (int i = 0; i < cuves.size(); i++) {
  Cuve& cuve = cuves[i];

  if (cuve.vins_contenu[0] != "/" && cuve.quantite > 0 && cuve.vins_contenu[0] != "Champagne" && cuve.vins_contenu[0] != "champagne" && cuve.vins_contenu[0] != "CHAMPAGNE" && !cuve.aSubiTransfert) {
    cout << "Tank  " << cuve.number << " - Remaining wine :" << cuve.vins_contenu[0] << endl;

    double volumeRestant = 0.0;
    for (int j = 0; j < volumesRestants.size(); j++) {
      if (volumesRestants[j].vin == cuve.vins_contenu[0] && volumesRestants[j].cuveIndex == i) {
        volumeRestant = volumesRestants[j].volume;
        break;
      }
    }
    cout << "Remaining volume for this wine : " << volumeRestant << "hL" << endl;
    cout << "Looking for an empty tank with the same volume for the transfer..." << endl;

      

    int meilleureCuveIndex = -1;
    double meilleureCuveScore = std::numeric_limits<double>::max();
    double differenceMin = std::numeric_limits<double>::max();
    double epsilon = 0.0001;

    for (int j = 0; j < cuves.size(); j++) {
      if (j != i && cuves[j].quantite <= volumeRestant && cuves[j].vins_contenu[0] == "/" && cuves[j].quantite < cuves[j].capacite) {
        double score = std::round(cuves[j].quantite * 10) / 10.0;
        if (std::abs(volumeRestant - score) < epsilon) {     
          meilleureCuveIndex = j;
          meilleureCuveScore = score;
          differenceMin = std::abs(score - volumeRestant);
          
        }
      }
    }

    if (meilleureCuveIndex != -1) {
      cuves[meilleureCuveIndex].vins_contenu[0] = cuve.vins_contenu[0];
      fichier << "Transfert of " << cuves[meilleureCuveIndex].quantite << "hL of " << cuve.vins_contenu[0] << " of the tank " << cuve.number << " to the tank " << cuves[meilleureCuveIndex].number << endl;
      fichier << endl;
      cuve.vins_contenu[0] = "/";
      cuve.aSubiTransfert = true;
      cuves[meilleureCuveIndex].aSubiTransfert = true;
      cuve.quantite -= cuves[meilleureCuveIndex].quantite;
    } else if (cuve.quantite != cuve.capacite) {
      cout << "No empty tank with sufficient volume was found. Search for a combination of tanks..." << endl;

      double meilleureDifference = DBL_MAX;
      vector<int> meilleureCombinaison;
      bool combinaisonTrouvee = trouverMeilleureCombinaisonCuves(cuves, volumeRestant, meilleureCombinaison, meilleureDifference);

if (combinaisonTrouvee) {
  cout << "Combination of tanks found :" << endl;

  unordered_set<int> cuvesDejaTransferees;
  cuvesDejaTransferees.reserve(meilleureCombinaison.size());

  for (auto it = meilleureCombinaison.cbegin(); it != meilleureCombinaison.cend(); ++it) {
    int index = *it;
    if (cuves[index].quantite >= 0.1 && cuvesDejaTransferees.find(index) == cuvesDejaTransferees.end()) {
      double transfertVolume = std::min(cuves[index].quantite, volumeRestant);
      if (transfertVolume >= 0.1 && cuve.quantite >= 0.1) {
        cuve.quantite -= transfertVolume;
         if (cuve.quantite < 0) {
          cuve.quantite += transfertVolume;
          break;
        } 
          cuves[index].vins_contenu[0] = cuve.vins_contenu[0];
          cuves[index].aSubiTransfert = true;
          
          fichier << "Transfer of " << transfertVolume << "hL of " << cuve.vins_contenu[0] << " of the tank " << cuve.number << " to the tank " << cuves[index].number << endl;
          cuvesDejaTransferees.emplace_hint(cuvesDejaTransferees.end(), index);
        }
        
      }
    }
    fichier << endl;
  }


  if (cuve.quantite < 0.001) {
    cuve.vins_contenu[0] = "/";
  }

  cout << endl;
}
    }
    }


for (int i = 0; i < cuves.size(); i++) {
  Cuve& cuve = cuves[i];
  if (cuve.vins_contenu[0] == "/") {
   double volume = cuve.quantite - cuve.quantite;
  cout << "Tank " << cuve.number << ": " << volume << "hL of " << cuve.vins_contenu[0] << endl;
  } else {
  cout << "Tank " << cuve.number << ": " << cuve.quantite << "hL of " << cuve.vins_contenu[0] << endl;
  }
}
fichier << "------------" << endl;
fichier << "------------" << endl;
fichier << endl; 
fichier << "LEFTOVERS / RESTES" << endl;
fichier << endl;
fichier << "The leftovers are indicated by the name of the wine, the quantity, and the tank ID." << endl;
fichier << "Les restes sont indiqués par nom, quantité et ID de cuve" << endl;
fichier << endl;
for (int i = 0; i < cuves.size(); i++) {
  Cuve& cuve = cuves[i];
  if (cuve.quantite > 0 && cuve.quantite < cuve.capacite) {
    fichier << "Tank " << cuve.number << ": " << cuve.quantite << "hL de " << cuve.vins_contenu[0] << std::endl;
  }
}

fichier << endl;
fichier << "------------" << endl;
fichier << "------------" << endl;
fichier << endl;
fichier << "Volume tank containing champagne :" << std::endl;
for (int i = 0; i < cuves.size(); i++) {
  
  Cuve& cuve = cuves[i];
  if (cuve.vins_contenu[0] == "Champagne" || cuve.vins_contenu[0] == "champagne" || cuve.vins_contenu[0] == "CHAMPAGNE") {
    fichier << "Tank " << cuve.number << ": " << cuve.quantite << "hL de " << cuve.vins_contenu[0] << std::endl;
   
  }
}
fichier << "------------" << endl;
fichier << "------------" << endl;
fichier << endl;
fichier << "Total champagne volume is " << total_volume_all_wines<< " hL"  << endl;

cout << "the file has been created successfully" << endl;
fichier.close();
  
}
else {
  cout << "Problem with the file" << endl;
}
  
  return 0;
}