#ifndef SOLUTION_NEAREST_NEIGHBOR_H
#define SOLUTION_NEAREST_NEIGHBOR_H


/*!
* \file  LocalSearchSolution.h
* \brief Represent the first amelioration of the solution.
*
* \author  Mamadou COULIBALY
* \author  Radia   MERABTENE
* \version 1.0
* \date	   25/12/2025
*/

#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <vector>
#include "Instance.hpp"

#include "SolutionInitiale.hpp"
#include "Solution.hpp"



class NearestNeighbor : public SolutionInitiale{


public:

    NearestNeighbor(Instance* instance): SolutionInitiale(instance){}
    ~NearestNeighbor(){}

    

    ///

    //void nearest_neighbor();
    int best_poi_from_hotel(int hotel, float total_distance, int id_jour);
    int best_poi_from_poi(int poi_index, float total_distance, int id_jour);
    

    void heuristic_nearest_neighbor();
    vector<int> build_list_hotel_for_all_journey();

    int determine_objective_function_value(vector<int> sequence_poi);

    void order_poi_by_fermeture();

    void add_unvisited_poi_to_the_solution();

    void two_opts(int first_poi_index, int second_poi_index);

    float determine_distance_for_day_journey_except_a_poi(int hotel_depart, int hotel_arrive, vector<int> sequence_poi);

    void GRASP();

    ///


};



#endif