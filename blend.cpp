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
struct Tank {
  string number;
  double quantity;
  vector<string> wines_contenu;
  vector<double>volumeuse;
  bool aSubitransfer;
  double capacity;
};

// Structure to store remaining volumes for each wine in each tank
struct volumeRemaining {
  string wine;
  double volume;
  int tankIndex;
};

// Function to generate a key for the memoization
std::string generateMemoKey(double volumeRemaining, int index) {
  return std::to_string(volumeRemaining) + "_" + std::to_string(index);
}

// Function to find the best combination of tanks to fill the remaining volume
void trouverCombinaisontanks(vector<Tank>& tanks, double volumeRemaining, vector<int>& bestCombinaison, double& bestDifference, vector<int>& combinaisonActuelle, int index, unordered_map<string, pair<double, vector<int>>>& memo) {
  if (volumeRemaining < 0.1) {
    double difference = std::abs(volumeRemaining);
    if (difference < bestDifference) {
      bestDifference = difference;
      bestCombinaison = combinaisonActuelle;
    }
    return;
  }

 // Check if the combination has already been evaluated
  string memoKey = generateMemoKey(volumeRemaining, index);
  if (memo.find(memoKey) != memo.end()) {
    pair<double, vector<int>>& memoEntry = memo[memoKey];
    if (memoEntry.first < bestDifference) {
      bestDifference = memoEntry.first;
      bestCombinaison = memoEntry.second;
    }
    return;
  }
  // Iterate over the tanks
  for (int i = index; i < tanks.size(); i++) {
    if (tanks[i].quantity >= 0.1 && tanks[i].wines_contenu[0] == "/" && std::find(bestCombinaison.begin(), bestCombinaison.end(), i) == bestCombinaison.end()) {
      combinaisonActuelle.push_back(i);

      trouverCombinaisontanks(tanks, volumeRemaining - tanks[i].quantity, bestCombinaison, bestDifference, combinaisonActuelle, i + 1, memo);

      combinaisonActuelle.pop_back();
    }
  }

// Save the evaluated combination in the memoization
  memo[memoKey] = make_pair(bestDifference, bestCombinaison);
}

// Function to find the best combination of tanks to fill the remaining volume
bool trouverbestCombinaisontanks(vector<Tank>& tanks, double volumeRemaining, vector<int>& bestCombinaison, double& bestDifference) {
  vector<int> combinaisonActuelle;
  unordered_map<string, pair<double, vector<int>>> memo;
  trouverCombinaisontanks(tanks, volumeRemaining, bestCombinaison, bestDifference, combinaisonActuelle, 0, memo);

  return (bestCombinaison.size() > 0);
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

  vector<Tank> tanks;
  vector<string> wines;
  vector<double> pourcentages;
  vector<double> quantities;
  vector<int> tanks_invalides;
  vector<volumeRemaining> volumesRemainings;
  

unordered_set<string> tankIds;
string line;
int tankLineCount = 0; // Tank line counter

std::string cheminAcces = "/Users/robindebry/Documents/GitHub/2022-2023-project-5-algorithmics-Team-5/output.txt";
std::ofstream folder(cheminAcces);
if (folder.is_open()) {

bool wineExiste = false;
while (getline(config_file, line)) {
  if (line.empty() || line[0] == '!' || line[0] == '\r') {
    continue; // ignore empty lines and comments beginning with '!
  } else if (line[0] == '#') {
    size_t pos1 = line.find(';');
    size_t pos2 = line.find(';', pos1 + 1);

    if (pos1 != string::npos && pos2 != string::npos) {
      string tankId = line.substr(1, pos1 - 1);

      // Check if the tank ID has already been encountered
      if (tankIds.count(tankId) > 0) {
        cerr << "Error: Duplicate tank ID found: " << tankId << endl;
        return 1;
      }

      // Check if the tank ID is valid
      Tank tank;
      tank.number = tankId;
      tank.quantity = stod(line.substr(pos1 + 1, pos2 - pos1 - 1));
      tank.capacity = stod(line.substr(pos1 + 1, pos2 - pos1 - 1));
      if (tank.quantity <= 0 || tank.quantity > 1000) {
        cerr << "Error: Invalid tank volume for tank " << tank.number << ". Volume should be between 0 and 1000." << endl;
        return 1;
      }

      // Check if a second tank quantity is entered
      size_t pos3 = line.find(';', pos2 + 1);
      if (pos3 != string::npos) {
        cerr << "Error: Multiple tanks quantities specified for tank " << tank.number << ". Only the first quantity will be considered." << endl;
        return 1;
      }

      string tank_wine = line.substr(pos2 + 1, pos3 - pos2 - 1);
      string wine;
      bool found = false;

      // Check that the wine name matches exactly one of the wines in the list
      if (tank_wine == "champagne" || tank_wine == "/" || tank_wine == "Champagne") {
        wine = tank_wine;
        found = true;
      } else {
        for (const string& existing_wine : wines) {
          if (existing_wine == tank_wine) {
            found = true;
            wine = existing_wine;
            break;
          }
        }
      }
      // If the wine name is not found, throw an error
      if (!found) {
        cerr << "Error: Unknown wine specified in the tank: " << tank_wine << endl;
        return 1;
      }

      tank.wines_contenu.push_back(wine);
      tanks.push_back(tank);

      // Add tank ID to all IDs encountered
      tankIds.insert(tankId);

      tankLineCount++;
    }
  } 
  else if (isdigit(line[0])) {
    quantities.push_back(stoi(line));
    if (quantities.size() >= 2) {
      cerr << "Warning: Only the first quantity value will be considered. Ignoring additional values." << endl;
      return 1;
    }
  } else {
    size_t pos = line.find(';');
    if (pos != string::npos) {
      string wine = line.substr(0, pos);

      bool wine_existe = false;
      for (const string& existing_wine : wines) {
        if (existing_wine == wine) {
          wine_existe = true;
          break;
        }
      }
      if (!wine_existe) {
        // The wine name has not yet been encountered, add it to the wines list
        wines.push_back(wine);
        pourcentages.push_back(stod(line.substr(pos + 1)));
      } else {
        cerr << "Error: Duplicate wine name found: " << wine << endl;
        return 1;
      }
    }
  }
}

// Check that all required information is present
if (tanks.empty() || wines.empty() || pourcentages.empty() || quantities.empty()) {
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

for (auto& tank : tanks) {
  // Check if volume exceeds 1000
  if (tank.quantity > 1000) {
    cerr << "Error: Volume exceeds the maximum limit of 1000 for tank " << tank.number << endl;
    return 1;
  }

  for (int i = 0; i < wines.size(); i++) {
    if (find(tank.wines_contenu.begin(), tank.wines_contenu.end(), wines[i]) != tank.wines_contenu.end()) {
      continue;
    }
    if (pourcentages[i] <= ((double)tank.quantity / 100.0)) {
      tank.wines_contenu.push_back(wines[i]);
      tank.quantity -= pourcentages[i] * 100.0;
    }
  }
}

  cout << "Wine: " << endl;
  for (const string& wine : wines) {
    cout << wine << endl;
  }

  cout << "Percentage: " << endl;
  for (double pourcentage : pourcentages) {
    cout << pourcentage << "%" << endl;
  }

  cout << "Quantity needed: " << endl;
  for (double quantity : quantities) {
    cout << quantity << "hL" << endl;
  }

  // Calculate total volume of wine needed
double total_volume = 0;
for (int quantity : quantities) {
  total_volume += quantity;
}

// Calculate required volumes for each wine
vector<double> required_volumes(wines.size());
for (int i = 0; i < wines.size(); i++) {
  required_volumes[i] = total_volume * pourcentages[i] / 100;
}

// Calculate volumes of each wine in each tank
vector<vector<double>> tanks_volumes(tanks.size(), vector<double>(wines.size(), 0));
for (int i = 0; i < tanks.size(); i++) {
  for (int j = 0; j < wines.size(); j++) {
    tanks_volumes[i][j] = tanks[i].quantity * pourcentages[j] / 100;
  }
}

// Determine tanks to use for each wine
vector<vector<int>> tanks_to_use(wines.size(), vector<int>());
map<string, double> total_volumes;
vector<double> remaining_volumes(wines.size());

for (int i = 0; i < wines.size(); i++) {
  double remaining_volume = required_volumes[i];
  double total_available_volume = 0; // Total volume available in tanks without wine

  for (int j = 0; j < tanks.size() && remaining_volume > 0; j++) {
    if (tanks_volumes[j][i] > 0 && tanks[j].wines_contenu[0] == "/") {
      total_available_volume += tanks_volumes[j][i];
    }
  }

  if (remaining_volume > total_available_volume) {
    cerr << "The volume needed is more than the volume you have inside your empty tanks" << endl;
    return 1;
  }
  // Calculate the remaining volume of wine needed
  for (int j = 0; j < tanks.size() && remaining_volume > 0; j++) {
    if (tanks_volumes[j][i] > 0 && tanks[j].wines_contenu[0] == "/") {
      double volume_to_use = min(remaining_volume, tanks_volumes[j][i]);
      tanks_to_use[i].push_back(j);
      remaining_volume -= volume_to_use;
      total_volumes[wines[i]] += volume_to_use;
    }
  }
}

// Check if the volume used exceeds the desired total volume and remove the last tank if necessary
double total_used_volume = 0;
for (int i = 0; i < wines.size(); i++) {
  for (int j = 0; j < tanks_to_use[i].size(); j++) {
    int tank_index = tanks_to_use[i][j];
    total_used_volume += tanks_volumes[tank_index][i];
  }
}

if (total_used_volume > total_volume) {
  for (int i = 0; i < wines.size(); i++) {
    while (total_used_volume > total_volume && !tanks_to_use[i].empty()) {
      int last_tank_index = tanks_to_use[i].back();
      double last_tank_volume = tanks_volumes[last_tank_index][i];
      tanks_to_use[i].pop_back();
      total_used_volume -= last_tank_volume;
    }
  }
}

// Calculate total tanks volume for each wine
for (int i = 0; i < wines.size(); i++) {
  double total_tanks_volume = 0;
  double required_volume = 0;

  for (int j = 0; j < tanks.size(); j++) {
    if (find(tanks[j].wines_contenu.begin(), tanks[j].wines_contenu.end(), wines[i]) != tanks[j].wines_contenu.end())
      total_tanks_volume += tanks[j].quantity;
  }

  
  for (int j = 0; j < tanks_to_use[i].size(); j++) {
    int tank_index = tanks_to_use[i][j];
    required_volume += tanks_volumes[tank_index][i];
    remaining_volumes[i] = required_volumes[i] - required_volume;
  }

  cout << "Required volume for " << wines[i] << ": " << required_volume << "hL" << endl;

  if (total_tanks_volume < required_volume) {
    cerr << "The volume of " << wines[i] << " is not sufficient in all tanks for the mix" << endl;
    return 1;
  }

  double remaining_volume = total_tanks_volume - required_volume;

  if (remaining_volume < 0) {
    cout << "Impossible to use " << wines[i] << " you need " << (-remaining_volume) << " hL more" << endl;
  } else {
    cout << "Remaining volume for " << wines[i] << ": " << remaining_volume << endl;
  }
}
folder << "EN" << endl;
folder << "This is the output file of our solution. It contains the final formula applied, the quantities of leftovers, and all the steps taken to blend the wines in the tanks to achieve the desired mix." << endl;
folder << endl;
folder << "- The final formula has been adapted from the one given in the problem to reflect the reality of the tanks and the final mix." << endl;
folder << "- The leftovers are the quantities of wine remaining in the tanks after the final mix is complete and the tanks containing them." << endl;
folder << "- The steps are all the operations performed to achieve the final mix, and they are listed in order from first to last." << endl;
folder << endl;
folder << "------------" << endl;
folder << "------------" << endl;
folder << endl;
folder << "FR" << endl;
folder << "Ceci est le folder de sortie de la solution." << endl; 
folder << "Il contient la formule finale appliquée, les quantités de restes et toutes les étapes pour mélanger les vins dans les cuves pour obtenir le mélange désiré." <<endl;
folder << endl;
folder << "- La formule finale est adaptée de celle donnée dans le problème à la réalité des cuves et du mélange final." << endl;
folder << "- Les restes sont les quantités de vin restant dans les cuves après que le mélange final soit fait et les cuves les contenant." << endl;
folder << "- Les étapes sont toutes les opérations effectuées pour obtenir le mélange final, elles sont ordonnées de la première à la dernière." << endl;
folder << endl;
folder << "------------" << endl;
folder << "------------" << endl;
folder << endl;
std::vector<bool> tanks_already_write(tanks.size(), false); // Tableau pour suivre les tanks déjà écrites
folder << "FINAL FORMULA / FORMULE FINALE :" << std::endl;

std::map<std::string, double> transfers_champagne;
std::map<std::string, std::map<int, double>> volumes_transferes;

// Write the final formula
for (int i = 0; i < wines.size(); i++) {
  folder << std::endl;
  folder << "=============================================" << std::endl;
  folder << "For wine " << wines[i] << ":" << std::endl;
  folder << "=============================================" << std::endl;

  std::map<int, bool> tanks_already_write;

  for (int j = 0; j < tanks_to_use[i].size(); j++) {
    int tank_index = tanks_to_use[i][j];
    tanks[tank_index].wines_contenu[0] = "Champagne";  // Change wine name from "/" to "Champagne

    // Check if the tank has already been written
    if (!tanks_already_write[tank_index]) {
      folder << "Tank : " << tanks[tank_index].number << std::endl;
      tanks_already_write[tank_index] = true; // Mark tank as already written

      // Display wine percentages in this tank
      for (int k = 0; k < wines.size(); k++) {
        double percentage = tanks_volumes[tank_index][k] / tanks[tank_index].quantity * 100;
        double volume_equivalent = tanks_volumes[tank_index][k];
        folder << wines[k] << ": " << percentage << "% (" << volume_equivalent << "hL)" << std::endl;
      }

      // Add filling steps
      folder << std::endl;
      double volume_needed = tanks_volumes[tank_index][i];
      folder << "Fill steps for tank " << tanks[tank_index].number << " (" << wines[i] << "):" << std::endl;

      double remaining_volume = volume_needed;
      bool transfer_done = false;
      int k = 0;

      while (remaining_volume > 0 && k < tanks.size()) {
        if (tanks[k].wines_contenu[0] == wines[i] && tanks[k].quantity > 0 && tanks[k].quantity >= 0.001) {
          double available_volume = tanks[k].quantity;
          double transfer_volume = std::min(remaining_volume, available_volume);
          folder << "  - Transfer " << transfer_volume << "hL from tank " << tanks[k].number << std::endl;
          remaining_volume -= transfer_volume;
          tanks[k].quantity -= transfer_volume;
          transfer_done = true;
          // Check if the remaining volume in the tank is less than 0.001 and set it to 0
          if (tanks_volumes[k][i] < 0.001) {
            tanks_volumes[k][i] = 0.0;
          }
        }
        k++;
      }

      // Check if the transfer is done
      if (remaining_volume > 0 && !transfer_done) {
        folder << "- Insufficient available volume in other tanks of the same wine to fulfill the requirement." << std::endl;
      } else if (remaining_volume > 0) {
        folder << "  - Additional volume needed from other sources: " << remaining_volume << "hL" << std::endl;
      } else {
        folder << endl;
        
      }
    }
  }
}

// Calculate and display total volume for all wines used
double total_volume_all_wines = 0;
for (int i = 0; i < wines.size(); i++) {
  for (int j = 0; j < tanks_to_use[i].size(); j++) {
    int tank_index = tanks_to_use[i][j];
    total_volume_all_wines += tanks_volumes[tank_index][i];
  }
}
std::cout << "Total volume for all wines: " << total_volume_all_wines << "hL" << std::endl;


std::cout << "Summary:" << std::endl;
std::cout << "--------" << std::endl;
  
// Calculation of total volume used for each wine
vector<double> volumesuses(wines.size(), 0.0);
for (int i = 0; i < tanks.size(); i++) {
  Tank& tank = tanks[i];
  for (int j = 0; j < wines.size(); j++) {
    if (find(tank.wines_contenu.begin(), tank.wines_contenu.end(), wines[j]) != tank.wines_contenu.end()) {
      double volumeuse = min(volumesuses[j], static_cast<double>(tank.quantity));
      double volumeRemaining = max(tank.quantity - volumeuse, 0.0);
      if (volumeRemaining == 0) {
        tank.quantity = 0;
        tank.wines_contenu[0] = "/";
      } else {
        tank.quantity = volumeRemaining;
      }
      volumesuses[j] -= volumeuse;

      // Storage of the remaining volume for each wine and each tank
      struct volumeRemaining volume;
      volume.wine = wines[j];
      volume.tankIndex = i;
      volume.volume = volumeRemaining;
      volumesRemainings.push_back(volume);
    }
  }
}

folder << endl;
folder << "------------" << endl;
folder << "------------" << endl;
folder << endl;
folder << "STEPS / ETAPES" << endl;
folder << endl;
folder << "The steps are indicated by the tank ID from which the wine is taken, the wine name, the quantity of wine taken from the tank, and the tank ID where the wine is placed." << endl;
folder << "Les étapes sont indiquées par l'ID de la tank d'où le wine est pris, le nom du vin, la quantité de vin prise dans la cuve, et l'ID de la cuve où le vin est placé." << endl;
folder << endl;

for (int i = 0; i < tanks.size(); i++) {
  Tank& tank = tanks[i];

  if (tank.wines_contenu[0] != "/" && tank.quantity > 0 && tank.wines_contenu[0] != "Champagne" && tank.wines_contenu[0] != "champagne" && tank.wines_contenu[0] != "CHAMPAGNE" && !tank.aSubitransfer) {
    cout << "Tank  " << tank.number << " - Remaining wine :" << tank.wines_contenu[0] << endl;

    double volumeRemaining = 0.0;
    for (int j = 0; j < volumesRemainings.size(); j++) {
      if (volumesRemainings[j].wine == tank.wines_contenu[0] && volumesRemainings[j].tankIndex == i) {
        volumeRemaining = volumesRemainings[j].volume;
        break;
      }
    }
    cout << "Remaining volume for this wine : " << volumeRemaining << "hL" << endl;
    cout << "Looking for an empty tank with the same volume for the transfer..." << endl;

      

    int bestTankIndex = -1;
    double bestTankscore = std::numeric_limits<double>::max();
    double differenceMin = std::numeric_limits<double>::max();
    double epsilon = 0.0001;

    for (int j = 0; j < tanks.size(); j++) {
      if (j != i && tanks[j].quantity <= volumeRemaining && tanks[j].wines_contenu[0] == "/" && tanks[j].quantity < tanks[j].capacity) {
        double score = std::round(tanks[j].quantity * 10) / 10.0;
        if (std::abs(volumeRemaining - score) < epsilon) {     
          bestTankIndex = j;
          bestTankscore = score;
          differenceMin = std::abs(score - volumeRemaining);
          
        }
      }
    }

    if (bestTankIndex != -1) {
      tanks[bestTankIndex].wines_contenu[0] = tank.wines_contenu[0];
      folder << "transfer of " << tanks[bestTankIndex].quantity << "hL of " << tank.wines_contenu[0] << " of the tank " << tank.number << " to the tank " << tanks[bestTankIndex].number << endl;
      folder << endl;
      tank.wines_contenu[0] = "/";
      tank.aSubitransfer = true;
      tanks[bestTankIndex].aSubitransfer = true;
      tank.quantity -= tanks[bestTankIndex].quantity;
    } else if (tank.quantity != tank.capacity) {
      cout << "No empty tank with sufficient volume was found. Search for a combination of tanks..." << endl;

      double bestDifference = DBL_MAX;
      vector<int> bestCombinaison;
      bool combinaisonTrouvee = trouverbestCombinaisontanks(tanks, volumeRemaining, bestCombinaison, bestDifference);

if (combinaisonTrouvee) {
  cout << "Combination of tanks found :" << endl;

  unordered_set<int> tanksAlreadyTransfers;
  tanksAlreadyTransfers.reserve(bestCombinaison.size());

  for (auto it = bestCombinaison.cbegin(); it != bestCombinaison.cend(); ++it) {
    int index = *it;
    if (tanks[index].quantity >= 0.1 && tanksAlreadyTransfers.find(index) == tanksAlreadyTransfers.end()) {
      double transferVolume = std::min(tanks[index].quantity, volumeRemaining);
      if (transferVolume >= 0.1 && tank.quantity >= 0.1) {
        tank.quantity -= transferVolume;
         if (tank.quantity < 0) {
          tank.quantity += transferVolume;
          break;
        } 
          tanks[index].wines_contenu[0] = tank.wines_contenu[0];
          tanks[index].aSubitransfer = true;
          
          folder << "Transfer of " << transferVolume << "hL of " << tank.wines_contenu[0] << " of the tank " << tank.number << " to the tank " << tanks[index].number << endl;
          tanksAlreadyTransfers.emplace_hint(tanksAlreadyTransfers.end(), index);
        }
        
      }
    }
    folder << endl;
  }


  if (tank.quantity < 0.001) {
    tank.wines_contenu[0] = "/";
  }

  cout << endl;
}
    }
    }


for (int i = 0; i < tanks.size(); i++) {
  Tank& tank = tanks[i];
  if (tank.wines_contenu[0] == "/") {
   double volume = tank.quantity - tank.quantity;
  cout << "Tank " << tank.number << ": " << volume << "hL of " << tank.wines_contenu[0] << endl;
  } else {
  cout << "Tank " << tank.number << ": " << tank.quantity << "hL of " << tank.wines_contenu[0] << endl;
  }
}
folder << "------------" << endl;
folder << "------------" << endl;
folder << endl; 
folder << "LEFTOVERS / RESTES" << endl;
folder << endl;
folder << "The leftovers are indicated by the name of the wine, the quantity, and the tank ID." << endl;
folder << "Les restes sont indiqués par nom, quantité et ID de tank" << endl;
folder << endl;
for (int i = 0; i < tanks.size(); i++) {
  Tank& tank = tanks[i];
  if (tank.quantity > 0 && tank.quantity < tank.capacity) {
    folder << "Tank " << tank.number << ": " << tank.quantity << "hL of " << tank.wines_contenu[0] << std::endl;
  }
}

folder << endl;
folder << "------------" << endl;
folder << "------------" << endl;
folder << endl;
folder << "Volume tank containing champagne :" << std::endl;
for (int i = 0; i < tanks.size(); i++) {
  
  Tank& tank = tanks[i];
  if (tank.wines_contenu[0] == "Champagne" || tank.wines_contenu[0] == "champagne" || tank.wines_contenu[0] == "CHAMPAGNE") {
    folder << "Tank " << tank.number << ": " << tank.quantity << "hL of " << tank.wines_contenu[0] << std::endl;
   
  }
}
folder << "------------" << endl;
folder << "------------" << endl;
folder << endl;
folder << "Total champagne volume is " << total_volume_all_wines<< " hL"  << endl;

cout << "the file has been created successfully" << endl;
folder.close();
  
}
else {
  cout << "Problem with the file" << endl;
}
  
  return 0;
}