// Ion separation scheme calculator
// Given a list of anions or cations in a solution, this program determines the process that uses the fewest steps to separate them;
// The solubility matrices were generated with another program.


#include<iostream>
#include<vector>
#include<string>
#include<queue>
#include<map>
#include "tables.h"


#define CATIONS 10
#define CDPLEN 1024
#define ANIONS 10
#define ADPLEN 1024


int c_precip[CATIONS]; // if >90% precipitate is formed when added in excess
int a_precip[ANIONS]; // if precipitate is formed when added in excess


std::map<std::string, int> anion_map;
std::map<std::string, int> cation_map;

std::vector<std::string> anion_list;
std::vector<std::string> cation_list;

std::string get_ions_list(bool cation_mode, int soln_subset){
    std::string ans = "";
    if(cation_mode){
        for(int i = 0; i < CATIONS; i++){
            if(soln_subset & 1 << i != 0){
                ans += cation_list[i];
                ans += " ";
            }
        }
    }
    else{
        for(int i = 0; i < ANIONS; i++){
            if(soln_subset & 1 << i != 0){
                ans += anion_list[i];
                ans += " ";
            }
        }
    }
    return ans;
}

int main(){

    int num_ions = CATIONS;
    std::vector<std::tuple<int, int, int>> visited; // data: path length, previous solution subset, ion used to precipitate the solution.
    std::map<std::string, int> ion_map;
    std::vector<int> opposite_ions;
    std::vector<std::string> opposite_ion_list;
    int n_oi;


    std::cout << "Ion separation scheme calculator developed by Bala Venkataraman for SCH4UP\nEnter A for anion solution and C for cation solution: ";
    std::string response;
    std::cin >> response;

    bool cation_mode = true; // true when separting cations, false when separating anions.
    if(response=="a" || response == "A"){
        cation_mode=false;
        for(int i = 0; i < ADPLEN; i++){
            std::tuple<int, int, int> t = {0,0,0};
            visited.push_back(t);
        }
        n_oi = CATIONS;
        for(int i = 0; i < CATIONS; i++){
            opposite_ions.push_back(c_precip[i]);
        }
        ion_map = anion_map;
        opposite_ion_list = cation_list;
    }
    else{
        for(int i = 0; i < CDPLEN; i++){
            std::tuple<int, int, int> t = {0,0,0};
            visited.push_back(t);
        }
        n_oi = ANIONS;
        for(int i = 0; i < ANIONS; i++){
            opposite_ions.push_back(a_precip[i]);
        }
        ion_map = cation_map;
        opposite_ion_list = anion_list;
    }

    

    std::queue<int> q;

    int current_solution = 0;
    int target_ion = 0;
    
    std::cout << "Enter each ion separated by a line break. Once you are done, type in END";
    while(true){
        std::string current_ion;
        std::cin >> current_ion;
        if(current_ion == "END"){
            break;
        }
        else{
            current_solution += 1 << ion_map[current_ion];
        }
    }
    std::cout << "Do you want to separate all ions or remove only one? If you want to remove only one, type its name. If not, press enter: "; 
    std::cin >> response;
    
    if(response.length() != 0){
        target_ion += 1 << ion_map[response];
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
            int prec = opposite_ions[i] &  i_sol;
            if(prec == target_ion && prec != 0){
                std::cout << "Optimal solution found";
                optimal_solution = i_sol^prec;
                std::tuple<int, int, int> v_i = {std::get<0>(visited[i_sol])+1,i_sol,i};
                visited[optimal_solution] = v_i;
                std::queue<int> empty;
                std::swap(q, empty);
                break;
            }
            else if(__builtin_popcount(prec) == 1 || (target_ion != 0 & prec&target_ion==0)){
                int new_sol = i_sol ^ prec;
                if(std::get<0>(visited[new_sol]) == 0 & new_sol != i_sol){
                    std::tuple<int, int, int> v_i = {std::get<0>(visited[i_sol])+1,i_sol,i};
                    visited[new_sol] = v_i;
                    q.push(new_sol);
                }   
            }
        }
    }
    
    int minimum_steps = 2 * visited.size();
    if(optimal_solution == 0){
        for(int i = 1; i < visited.size(); i++){
            int pathlen = std::get<0>(visited[i]);
            if(__builtin_popcount(i) == 1 && pathlen != 0 && pathlen < minimum_steps){
                minimum_steps = pathlen;
                optimal_solution = i;
            }

        }
    }
    if(optimal_solution == 0){
        std::cout << "The ions in the solution are inseparable\n";
    }
    else{
        while (optimal_solution != current_solution)
        {
            std::tuple<int, int, int> v = visited[optimal_solution];
            std::cout << "Add " + opposite_ion_list[std::get<2>(v)] + " To the solution, leaving " + get_ions_list(cation_mode, optimal_solution) + " in the supernatant\n";
            optimal_solution = std::get<1>(v);
        }
        std::cout << "Please read the above instructions in reverse";
    }
    }