#ifndef SOLUTION_INITIAL_H
#define SOLUTION_INITIAL_H


/*!
* \file  SolutionInitiale.h
* \brief Represent the initial solution for our problem.
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

/*!
* Id for Hotel.
*/
#define HOTEL 1

/*!
* Id for POI.
*/
#define POI 2

using namespace std;

/*!
* \class SolutionInitiale
* \brief  Class representing an initial solution for the Touristic Circuit Problem.
*
*  This class allows use to generate an initiale solution for the Touristic Circuit Problem. The first thing
*  we did is to create intemadiate hotel list by making sure there are at leat one POI to visit between two Hotels.
*  This solution is a minimal solution and represent the enter point for the resolution of the problem.
*/
class SolutionInitiale{
protected:

    /*!
     * An instance for the class Instance which stock all the data for the problem.
     */
    Instance* instance;

    /*!
     * Starting point. This corresponds to the Hotel called started hotel.
     */
    int hotel_depart;

    /*!
     * List that carry all vidited POI so far.
     */
    vector<int> poi_visited = vector<int>();     

    /*!
     * Container for all intermediate hotel for the journey.
     */
    vector<int> hotel_Intermedaire = vector<int>();       
    
    /*!
     * Matrix that contain all the sub-sequence of POI visited by day.
     */
    vector<vector<int>> sequence_Id_Poi_Par_Jour = vector<vector<int>>();  
    
    /*!
     * Container for all departure time.
     */
    vector<float> v_Date_Depart = vector<float>();

    /*!
     * Total distance for the trip related to a specific day. All day has its own total distance.
     */
    float total_distance_for_trip = 0.0f;
    

    /*!
    * A boolean to identify if the current position is an Hotel or a POI.
    */
    bool is_hotel = true;      
    
    /*!
    * The value of our objective function. The goal is to maximise this values by visiting hight qualities POI.
    */
    int i_valeur_fonction_objectif = 0;


public:

    /*!
    * Confort contructor for the class SolutionInitiale.
    * \param instance : Pointer towards an Instance.
    */
    SolutionInitiale(Instance* instance);

    /*!
    * Destructor to free space allocated during the creation of an instance of SolutionInitiale.
    */
    ~SolutionInitiale(){}
    
    /*!
    * Getter for total day tripe distance.
    * @return total_distance_for_trip : float
    */
    float get_total_distance_for_trip(){ return total_distance_for_trip; }

    /*!
    * Methods used to display all POI visited until the last day.
    */
    void display_sequence_poi();

    /*!
    * Getter for objective fonction value.
    * @return i_valeur_fonction_objectif : float
    */
    float get_objective_function_value(){ return (float)i_valeur_fonction_objectif; }

    /*!
    * Getter for all intermadiate hotel list.
    * @return hotel_Intermedaire : vector<int>
    */
    vector<int> get_intermediate_hotel(){ return hotel_Intermedaire; }

    /*!
    * Getter for all sequence from the starting day to the end day.
    * @return sequence_Id_Poi_Par_Jour : vector<vector<int>>
    */
    vector<vector<int>> get_sequence_poi_par_jour(){ return sequence_Id_Poi_Par_Jour; }

    /*!
    * Getter for the starting time for all day.
    * @return v_Date_Depart : vector<int>
    */
    vector<float> get_date_depart(){ return v_Date_Depart; }

    
    /*!
    * Methods used to get distance between current position and the POI depending on the Id of hotel_or_poi.
    * @param index_current : int, Current position.
    * @param index_next_poi : int, Next POI to visit.
    * @param hotel_or_poi : int, Id for Hotel or POI
    * @return Distance between Hotel-POI or POI-POI
    */
    float distance_to_next_poi_or_hotel(int index_current, int index_next_poi, int hotel_or_poi);


    /*!
    * Methods used to check if the POI is accessible from the current position.
    * @param poi_index : int, POI we want to visit.
    * @param current_distance : float, Distance until the POI we want to visit.
    * @param current_position : int, Current position.
    * @param is_hotel : bool, true if it's an Hotel false otherwise.
    * @return true if the POI is accessible false otherwise.
    */
    bool poi_is_accessible_from_current_position(int poi_index, float current_distance, int current_position, bool is_hotel);
    
    /*!
    * Methods used to build intermadiate hotel list.
    */
    void build_Intermediate_Hotel_List();

    /*!
    * Methods used to find the best POI between two hotels.
    * @param hotel_depart_index : int, Departure hotel.
    * @param hotel_arrivee_index : int, Destination hotel.
    * @param max_distance_jour : float, Maximun distance allows by day.
    * @return The POI found or -1 if there is not best POI between the two hotels.
    */
    int best_poi_between_two_hotels(int hotel_depart_index, int hotel_arrivee_index, float max_distance_jour);

    /*!
    * Methods used to build the initial solution starting with the construction of intermediate hotel.
    */
    void solution_by_building_hotel_first();


    /*!
    * Methods used to find the best POI between an hotel and the last one.
    * @param hotel_depart_index : int, Departure hotel.
    * @param hotel_arrivee_index : int, Destination hotel.
    * @param max_distance_jour : float, Maximun distance allows by day.
    * @return The POI found or -1 if there is not best POI between an hotel and the last one.
    */
    int poi_between_hotel_and_last_hotel(int hotel_depart_index, int hotel_arrivee_index, float max_distance_jour);

    /*!
    * Methods used to find the nearest hotel from the current hotel.
    * @param index_current_hotel : int, current hotel.
    * @param index_jour : int, Id of the corresponding day.
    * @return The Hotel found or -1 if there is not nearest Hotel from the current Hotel.
    */
    int find_nearest_hotel_index(int index_current_hotel, int index_jour);

    
    

    //

protected:
    
    /*!
    * Find the index of the maximun value in the distance list.
    * @param distance : vector<float>
    * @return int, The index of the maximun value in the list.
    */
    int max_Index(vector<float> distance);

    /*!
    * Find the index of the minimun value in the distance list.
    * @param distance : vector<float>
    * @return int, The index of the minimun value in the list.
    */
    int find_Min_Index(vector<float> distance);

    /*!
    * Methods used to add a POI to the POI visited list.
    * @param index_poi : int, POI to add to the POI visited list.
    */
    void add_to_poi_visited(int index_poi);

    /*!
    * Methods used to check if a POI is already in the POI visited list.
    * @param index_poi : int, POI to add to the POI visited list.
    * @return true if the POI already exist false otherwise.
    */
    bool poi_already_visited(int index_poi);

    /*!
    * Methods used to add a Hotel to the intermadiate hotel list.
    * @param index : int, Hotel to add to the intermadiate hotel list.
    */
    void add_to_intermadiate_hotel(int index);
    
    
    
    
    

    


};


#endif