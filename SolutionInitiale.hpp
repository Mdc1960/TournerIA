#ifndef SOLUTION_INITIAL_H
#define SOLUTION_INITIAL_H

#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <vector>
#include "Instance.hpp"

#define HOTEL 1
#define POI 2

using namespace std;

class SolutionInitiale{
private:
    Instance* instance;
    int hotel_depart;
    vector<int> poi_visited = vector<int>();     
    vector<int> hotel_Intermedaire = vector<int>();        
    vector<vector<int>> sequence_Id_Poi_Par_Jour = vector<vector<int>>();   
    vector<float> v_Date_Depart = vector<float>();
    vector<int> prohibited_poi = vector<int>();
    float total_distance_for_trip = 0.0f;
    bool is_hotel = true;                    
    int i_valeur_fonction_objectif = 0;
public:
    SolutionInitiale(Instance* instance);
    ~SolutionInitiale(){}
    void display_sequence_poi();
    float get_objective_function_value(){ return (float)i_valeur_fonction_objectif; }
    vector<int> get_intermediate_hotel(){ return hotel_Intermedaire; }
    vector<vector<int>> get_sequence_poi_par_jour(){ return sequence_Id_Poi_Par_Jour; }
    vector<float> get_date_depart(){ return v_Date_Depart; }
    int get_objective_function_value_int(){ return i_valeur_fonction_objectif; }


    //

    float distance_to_next_poi_or_hotel(int index_current, int index_next_poi, int hotel_or_poi);
    bool poi_is_accessible_from_current_position(int poi_index, float current_distance, int current_position, bool is_hotel);
    int best_poi_to_visit_from_current_position(int current_position, float current_distance, bool is_hotel);
    void build_solution();

    bool poi_is_possible_to_visit_in_day(int poi_index, int current_position, bool is_hotel, float max_distance_jour);

    //

private:
    
    int max_Index(vector<float> distance);
    int find_Min_Index(vector<float> distance);
    void add_to_poi_visited(int index_poi);
    bool poi_already_visited(int index_poi);
    void add_to_intermadiate_hotel(int index);
    bool hotel_already_in_intermedaite_hotel(int index);
    int find_best_intermediate_hotel_from_poi(int index_poi, float current_distance, float max_distance);
    

    
    
    
    

    


};


#endif