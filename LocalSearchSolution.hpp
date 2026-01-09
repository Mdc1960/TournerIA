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
#include "Checker.hpp"



/*!
* \class NearestNeighbor extends the class SolutionInitiale
* \brief  Class used to build definitive solution for the Touristic Circuit Problem.
*
*  This class allows use to generate an optimal solution for the Touristic Circuit Problem.
*  This solution is intended to be the optimal one if all the requirements are realised.
*/
class NearestNeighbor : public SolutionInitiale{


public:

    /*!
    * Confort constractor for the class NearestNeighbor.
    * \param instance : Pointer towards an Instance.
    */
    NearestNeighbor(Instance* instance): SolutionInitiale(instance){}

    /*!
    * Destructor to free space allocated during the creation of an instance of NearestNeighbor.
    */
    ~NearestNeighbor(){}

    /*!
    * Method used to change a poi in a sequence by anoyher which not viseted yet.
    */
    void change_poi_by_unvised_poi();

    

    

    ///

    /*!
    * Method used to determine the best poi from a specific hotel.
    * @param hotel : int, id for an hotel.
    * @param total_distance : float, distance until now.
    * @param id_jour : int, id for a day.
    * @return The of the best hotel or -1 if no hotel is found.
    */
    int best_poi_from_hotel(int hotel, float total_distance, int id_jour);

    /*!
    * Method used to determine the best poi from a specific poi.
    * @param poi_index : int, id for a poi.
    * @param total_distance : float, distance until now.
    * @param id_jour : int, id for a day.
    * @return The of the best poi or -1 if no poi is found.
    */
    int best_poi_from_poi(int poi_index, float total_distance, int id_jour);
    

    /*!
    * Heuristic use to find solution which cann later be improve by the Metaheuristic GRASP.
    */
    void heuristic_nearest_neighbor();

    /*!
    * Build the list of all hotel to visit during the journey. From the first day to the last one.
    * @return List of all hotel for the journey.
    */
    vector<int> build_list_hotel_for_all_journey();

    /*!
    * Method used to determine the objective fontion value for a specific sequence.
    * @param sequence_poi : vector<int>, sequence of visited poi for a day.
    * @return The value of the objective fonction related to the given sequence.
    */
    int determine_objective_function_value(vector<int> sequence_poi);

    /*!
    * Method used to order poi depending on the value of poi close time.
    */
    void order_poi_by_fermeture();

    /*!
    * Method used to add unvisited poi to the solution found by the heuristic method.
    */
    void add_unvisited_poi_to_the_solution();

    /*!
    * 2-OPT Algorithm to improve the solution found by the heuristic method.
    */
    void two_opt();

    /*!
    * SWAP Algorithm to improve the solution found by the heuristic method.
    */
    void swap();

    /*!
    * Method used to check if the solution found is a valid one.
    * @param all_sequence : vector<vector<int>>, sequence representing a solution.
    * @return true if the solution is valid one, otherwise false is returned.
    */
    bool sequence_is_valid(vector<vector<int>> all_sequence);

    /*!
    * Method used to determine the distance for a specific day trip.
    * @param hotel_depart : int, id the departure hotel.
    * @param hotel_arrive : int, id the arrival hotel.
    * @param sequence_poi : vector<int>, sequence of visited poi during a specific day.
    * @return The total distance for a day trip.
    */
    float determine_distance_for_day_journey(int hotel_depart, int hotel_arrive, vector<int> sequence_poi);

    /*!
    * Metaheuristic GRASP to find the optimal solution for the Touristic Circuit Problem.
    */
    void GRASP();

    ///


};



#endif