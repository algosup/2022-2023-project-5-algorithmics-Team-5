#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

struct Cuve {
  string number;
  int quantite;
  vector<string> vins_contenu;
};

int main() {
  string config_file_path;

  cout << "Drag and drop your config file here and press Enter:" << endl;
  getline(cin, config_file_path);

  // Remove apostrophes from file path
  config_file_path.erase(remove(config_file_path.begin(), config_file_path.end(), '\''), config_file_path.end());

  cout << "Reading config file: " << config_file_path << endl;

  ifstream config_file(config_file_path);

  if (!config_file.is_open()) {
    cerr << "Could not open config file: " << config_file_path << endl;
    return 1;
  }

  vector<Cuve> cuves;
  vector<string> vins;
  vector<double> pourcentages;
  vector<int> quantites;

  string line;
  while (getline(config_file, line)) {
    if (line.empty() || line[0] == '!' || line[0] == '\r') {
      continue; // ignore empty lines and comments starting with '!'
    }
    else if (line[0] == '#') {
      size_t pos1 = line.find(';');
      size_t pos2 = line.find(';', pos1 + 1);
      if (pos1 != string::npos && pos2 != string::npos) {
        Cuve cuve;
        cuve.number = line.substr(1, pos1 - 1);
        cuve.quantite = stoi(line.substr(pos1 + 1, pos2 - pos1 - 1));
        cuve.vins_contenu.push_back(line.substr(pos2 + 1));
        cuves.push_back(cuve);
      }
    }
    else if (isdigit(line[0])) {
      quantites.push_back(stoi(line));
    }
    else {
      size_t pos = line.find(';');
      if (pos != string::npos) {
        vins.push_back(line.substr(0, pos));
        pourcentages.push_back(stod(line.substr(pos + 1)));
      }
    }
  }

  for (auto& cuve : cuves) {
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

  cout << "Cuves: " << endl;
  for (const Cuve& cuve : cuves) {
    cout << "Cuve " << cuve.number << " : " << cuve.quantite << "hL";
    if (!cuve.vins_contenu.empty()) {
      cout << " contains ";
      for (int i = 0; i < cuve.vins_contenu.size(); i++) {
        if (i > 0) {
          cout << ", ";
        }
        cout << cuve.vins_contenu[i];
      }
    }
    cout << endl;
  }

  cout << "Vins: " << endl;
  for (const string& vin : vins) {
    cout << vin << endl;
  }

  cout << "Pourcentages: " << endl;
  for (double pourcentage : pourcentages) {
    cout << pourcentage << "%" << endl;
  }

  cout << "Quantity needed: " << endl;
  for (int quantite : quantites) {
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

  // Sort cuves by descending volume
  sort(cuves.begin(), cuves.end(), [](const Cuve& cuve1, const Cuve& cuve2) {
    return cuve1.quantite > cuve2.quantite;
  });

  // Calculate volumes of each wine in each cuve
  vector<vector<double>> cuves_volumes(cuves.size(), vector<double>(vins.size(), 0));
  for (int i = 0; i < cuves.size(); i++) {
    for (int j = 0; j < vins.size(); j++) {
      cuves_volumes[i][j] = cuves[i].quantite * pourcentages[j] / 100;
    }
  }

  // Determine cuves to use for each wine
  vector<vector<int>> cuves_to_use(vins.size(), vector<int>());
  map<string, double> total_volumes;

  for (int i = 0; i < vins.size(); i++) {
    double remaining_volume = required_volumes[i];
    double total_available_volume = 0; // Total volume available in cuves without wine
    for (int j = 0; j < cuves.size() && remaining_volume > 0; j++) {
      if (cuves_volumes[j][i] > 0 && cuves[j].vins_contenu[0] == "/") {
        total_available_volume += cuves_volumes[j][i];
      }
    }

    if (remaining_volume > total_available_volume) {
      cerr << "The volume needed is more than the volume you have inside your empty tanks" << endl;
      return 1;
    }

    for (int j = 0; j < cuves.size() && remaining_volume > 0; j++) {
      if (cuves_volumes[j][i] > 0 && cuves[j].vins_contenu[0] == "/") {
        double volume_to_use = min(remaining_volume, cuves_volumes[j][i]);
        cuves_to_use[i].push_back(j);
        remaining_volume -= volume_to_use;
        total_volumes[vins[i]] += volume_to_use;
      }
    }
  }

  // Calculate total cuves volume for each wine
  for (int i = 0; i < vins.size(); i++) {
    double total_cuves_volume = 0;
    for (int j = 0; j < cuves.size(); j++) {
      if (find(cuves[j].vins_contenu.begin(), cuves[j].vins_contenu.end(), vins[i]) != cuves[j].vins_contenu.end()) {
        total_cuves_volume += cuves[j].quantite;
      }
    }

    cout << "Total volume for " << vins[i] << ": " << total_cuves_volume << endl;
    double required_volume = 0;
for (int j = 0; j < cuves_to_use[i].size(); j++) {
  int cuve_index = cuves_to_use[i][j];
  required_volume += cuves_volumes[cuve_index][i];
}

cout << "Required volume for " << vins[i] << ": " << required_volume << "hL" << endl;

    if (total_cuves_volume < required_volumes[i]) {
      cerr << "The volume of " << vins[i] << " is not sufficient in all cuves for the mix" << endl;
      return 1;
    }
    double remaining_volume = total_cuves_volume;
for (int j = 0; j < cuves_to_use[i].size(); j++) {
  int cuve_index = cuves_to_use[i][j];
  remaining_volume -= cuves_volumes[cuve_index][i];
}

    if (remaining_volume < 0){
      cout << "Impossible to use " << vins[i] << " you need " << abs(remaining_volume) << " hL more" <<endl;
    }
    else{
    cout << "Remaining volume for " << vins[i] << ": " << remaining_volume << endl;
    }
  }
    
  


  // Output cuves to use for each wine
  for (int i = 0; i < vins.size(); i++) {
    cout << "For wine " << vins[i] << ":" << endl;
    for (int j = 0; j < cuves_to_use[i].size(); j++) {
      int cuve_index = cuves_to_use[i][j];
      cout << "- Cuve " << cuves[cuve_index].number << " (" << cuves_volumes[cuve_index][i] << "hL)" << endl;
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
  cout << "Total volume for all wines used: " << total_volume_all_wines << "hL" << endl;


  return 0;
}