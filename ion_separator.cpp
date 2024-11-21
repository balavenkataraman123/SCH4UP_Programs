// Ion separation scheme calculator
// Given a list of anions or cations in a solution, this program determines the process that uses the fewest steps to separate them;
// The solubility matrices were generated with another program.


#include<iostream>
#include<vector>
#include<string>
#include<queue>
#include<map>
#include <tuple>


#define CATIONS 9
#define CDPLEN 512
#define ANIONS 7
#define ADPLEN 128

using namespace std;

const int a_precip[] = {0b111111111, 0b000000111, 0b000000111, 0b000000111, 0b000101111, 0b000111111, 0b000111111, 0b000111111}; // if precipitate is formed when adding anion solution in excess to various cation solutions
const int c_precip[] = {0b1111111, 0b1111111, 0b1111111, 0b1111001, 0b1110001, 0b1111001, 0b0000001, 0b0000001, 0b0000001}; // if precipitate is formed when adding cation solutoin in excess to various anion solutions



vector<string> anion_list = {"NO3-", "PO43-", "SO32-", "CO32-", "S2-", "I-", "Br-", "Cl-"};
vector<string> cation_list = {"NH4+", "Li+", "Na+", "Sr2+", "Ca2+", "Ba2+", "Hg2+", "Ag+", "Pb2+"};

string get_ions_list(bool cation_mode, int soln_subset){
    string ans = "";
    if(cation_mode){
        for(int i = 0; i < CATIONS; i++){
            if((soln_subset & 1 << i) != 0){
                ans += cation_list[i];
                ans += " ";
            }
        }
    }
    else{
        for(int i = 0; i < ANIONS; i++){
            if((soln_subset & 1 << i) != 0){
                ans += anion_list[i];
                ans += " ";
            }
        }
    }
    return ans;
}

int main(){

    int num_ions = CATIONS;
    vector<tuple<int, int, int> > visited; // data: path length, previous solution subset, ion used to precipitate the solution.
    vector<string> ion_list;
    vector<int> opposite_ions;
    vector<string> opposite_ion_list;
    int n_oi;


    cout << "Ion separation scheme calculator developed by Bala Venkataraman for SCH4UP\nEnter A for anion solution and C for cation solution: ";
    string response;
    cin >> response;

    bool cation_mode = true; // true when separting cations, false when separating anions.
    if(response=="a" || response == "A"){
        cation_mode=false;
        for(int i = 0; i < ADPLEN; i++){
            tuple<int, int, int> t = {0,0,0};
            visited.push_back(t);
        }
        n_oi = CATIONS;
        for(int i = 0; i < CATIONS; i++){
            opposite_ions.push_back(c_precip[i]);
        }
        ion_list = anion_list;
        opposite_ion_list = cation_list;
    }
    else{
        for(int i = 0; i < CDPLEN; i++){
            tuple<int, int, int> t = {0,0,0};
            visited.push_back(t);
        }
        n_oi = ANIONS;
        for(int i = 0; i < ANIONS; i++){
            opposite_ions.push_back(a_precip[i]);
        }
        ion_list = cation_list;
        opposite_ion_list = anion_list;
    }

    

    queue<int> q;

    int current_solution = 0;
    int target_ion = 0;
    
    cout << "Enter each ion separated by a line break. Once you are done, type in \"END\"\n";
    while(true){
        string current_ion;
        cin >> current_ion;
        if(current_ion == "END"){
            break;
        }
        else{
            auto it = find(ion_list.begin(), ion_list.end(), current_ion);
            if(it == ion_list.end()){
                cout << "That ion is not available in the database\n";
            }
            else{
            current_solution += 1 << distance(ion_list.begin(), it);
            }
        }
    }
    cout << "Do you want to separate all ions or remove only one? If you want to remove only one, type its name. If not, type \"NONE\": "; 
    cin >> response;
    
    if(response != "NONE"){
        auto it = find(ion_list.begin(), ion_list.end(), response);
        if(it == ion_list.end()){
            cout << "That ion is not available in the database. Searching for procedure to separate entire solution instead\n";
        }
        else{        
        target_ion += 1 << distance(ion_list.begin(), it);
        if((target_ion & current_solution) == 0){
            cout << "That ion is not in the solution. Searching for procedure to separate entire solution instead\n";
            target_ion = 0;
        }
        }

    }
    q.push(current_solution);
    int optimal_solution = 0;
    
    while(q.size() > 0){
        
        int i_sol = q.front(); q.pop();
        if(i_sol == target_ion && i_sol != 0){
            optimal_solution = target_ion;
            break;
        }
        for(int i = 0; i < n_oi; i++){
            int prec = ((~opposite_ions[i]) &  i_sol);
            if(prec == target_ion && prec != 0){
                cout << "Optimal solution found";
                optimal_solution = i_sol^prec;
                tuple<int, int, int> v_i = {get<0>(visited[i_sol])+1,i_sol,i};
                visited[optimal_solution] = v_i;
                queue<int> empty;
                swap(q, empty);
                break;
            }
            else if(__builtin_popcount(prec) == 1 || (target_ion != 0 && (prec&target_ion) ==0)){
                int new_sol = i_sol ^ prec;
                if(get<0>(visited[new_sol]) == 0 && new_sol != i_sol){
                    tuple<int, int, int> v_i = {get<0>(visited[i_sol])+1,i_sol,i};
                    visited[new_sol] = v_i;
                    q.push(new_sol);
                }   
            }
        }
    }
    
    int minimum_steps = 2 * visited.size();
    if(optimal_solution == 0){
        for(int i = 1; i < visited.size(); i++){
            int pathlen = get<0>(visited[i]);
            if(__builtin_popcount(i) == 1 && pathlen != 0 && pathlen < minimum_steps){
                minimum_steps = pathlen;
                optimal_solution = i;
            }

        }
    }
    if(optimal_solution == 0){
        cout << "The ions in the solution are inseparable\n";
    }
    else{
        while (optimal_solution != current_solution)
        {
            tuple<int, int, int> v = visited[optimal_solution];
            cout << "Add " + opposite_ion_list[get<2>(v)] + " To the solution, leaving " + get_ions_list(cation_mode, optimal_solution) + " in the supernatant\n";
            optimal_solution = get<1>(v);
        }
        cout << "Please read the above instructions in reverse\n";
    }
    }