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
    bool is_hotel = true;                    
    int i_valeur_fonction_objectif = 0;
public:
    SolutionInitiale(Instance* instance);
    ~SolutionInitiale(){}
    void display_sequence_poi();
    void load_sequence();
    void fill_objective_function_value();
    float get_objective_function_value(){ return (float)i_valeur_fonction_objectif; }
    vector<int> get_intermediate_hotel(){ return hotel_Intermedaire; }
    vector<vector<int>> get_sequence_poi_par_jour(){ return sequence_Id_Poi_Par_Jour; }
    vector<float> get_date_depart(){ return v_Date_Depart; }
    int get_objective_function_value_int(){ return i_valeur_fonction_objectif; }
    void show_poi_visited();


    //

    float distance_to_next_poi_or_hotel(int index_current, int index_next_poi, int hotel_or_poi);
    bool poi_is_accessible_from_current_position(int poi_index, float current_distance, int current_position, bool is_hotel);
    int best_poi_to_visit_from_current_position(int current_position, float current_distance, bool is_hotel);
    void build_solution();

    //

private:

    
    
    vector<int> stock_and_get(int id_jour);
    vector<float> distance_Hotel_and_all_Poi(int index_hotel, int length_poi,int id_jour);
    
    int max_Index(vector<float> distance);
    int get_Best_Index_From_Poi_Poi_Distance(int index_curent_Poi,float distance,int id_jour);
    int find_Min_Index(vector<float> distance);
    void add_to_poi_visited(int index_poi);
    bool poi_already_visited(int index_poi);
    void add_to_intermadiate_hotel(int index);
    bool hotel_already_in_intermedaite_hotel(int index);
    int find_best_intermediate_hotel_from_poi(int index_poi, float current_distance, float max_distance);
    bool belongs_to_prohibited_poi(int index_poi);
    

    
    
    
    

    


};


#endif