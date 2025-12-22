#ifndef SOLUTION_NEAREST_NEIGHBOR_H
#define SOLUTION_NEAREST_NEIGHBOR_H

#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <vector>
#include "Instance.hpp"

#include "SolutionInitiale.hpp"


class NearestNeighbor : public SolutionInitiale{


public:

    NearestNeighbor(Instance* instance): SolutionInitiale(instance){}
    ~NearestNeighbor(){}

    void best_nearest_neighbor_by_day(int hotel_depart, int hotel_arrive, vector<int> list_poi_not_visited, int id_jour);
    int generate_best_poi_or_hotel_neighbor(int index_last_poi, int depart, int hotel_arrive, vector<int> list_poi_not_visited, int id_jour, float depart_is_hotel);


    ///

    void nearest_neighbor();
    int best_poi_from_hotel(int hotel, float total_distance, int id_jour);
    int best_poi_from_poi(int poi_index, float total_distance, int id_jour);
    int find_nearest_poi_not_visited(int poi_index, float total_distance, int id_jour);

    void heuristic_nearest_neighbor();
    vector<int> build_list_hotel_for_all_journey();

    ///


};



#endif