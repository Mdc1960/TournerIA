#ifndef SOLUTION_INITIAL_H
#define SOLUTION_INITIAL_H

#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <vector>
#include "Instance.hpp"

using namespace std;

class SolutionInitiale{
private:
    Instance* instance;
    vector<int> hotel_Intermedaire = vector<int>();        
    vector<vector<int>> sequence_Id_Poi_Par_Jour = vector<vector<int>>();   
    vector<float> v_Date_Depart = vector<float>();                    
    int i_valeur_fonction_objectif = 0;
public:
    SolutionInitiale(Instance* instance);
    ~SolutionInitiale(){}

private:
    
    vector<float> stock_and_get(int poi_index, int id_jour);
    vector<float> distance_Hotel_and_all_Poi(int index_hotel, int length_poi);
    vector<int> find_Poi_Index_By_Condition(vector<float> distance, float condition);
    int max_Poi_index(vector<float> distance, int length_poi, vector<int> listIndex);
    int max_Index(vector<float> distance);
    int get_Best_Index_From_Poi_Poi_Distance(int index_curent_Poi);
    int find_Min_Index(vector<float> distance);
    

    


};


#endif