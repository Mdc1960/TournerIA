#include "NearestNeighbor.hpp"
#include "Solution.hpp"



/// Heuristique to find a good solution but not the optimal one.
void NearestNeighbor::heuristic_nearest_neighbor()
{
    solution_by_building_hotel_first();
    cout << "@ - @ Intermadiate Hotel : " << hotel_Intermedaire.size() + 1 << " - Jour : " << this->instance->get_Nombre_Jour() << endl;
    
    vector<int> all_hotel = build_list_hotel_for_all_journey();

    int maxIteration = this->instance->get_Nombre_Hotel() + this->instance->get_Nombre_POI() + 1;

    for (int id_jour = 0; id_jour < this->instance->get_Nombre_Jour(); ++id_jour){

        
        int hotel_depart = all_hotel[id_jour];
        int hotel_destination = all_hotel[id_jour + 1];

        int iteration = 0;
        
        float distance_max_jour = this->instance->get_POI_Duree_Max_Voyage(id_jour);
        
        vector<int> sequence_jour = vector<int>();

        bool hotel = true;
        this->total_distance_for_trip = 0.0f;

        vector<int> saved_sequence_day = sequence_Id_Poi_Par_Jour[id_jour];
        if (saved_sequence_day.empty()) continue;
        add_to_poi_visited(saved_sequence_day[saved_sequence_day.size()-1]);
        
        float last_distance_poi_hotel_destination = this->instance->get_distance_Hotel_POI(hotel_destination,saved_sequence_day[saved_sequence_day.size()-1]);
        int current_position = hotel_depart;
        int index_best_poi = -1;
        //cout << "$$ -- $$" << endl;
        while(iteration < maxIteration){

            if (hotel){
                index_best_poi = best_poi_from_hotel(current_position,total_distance_for_trip, id_jour);
                
                if (index_best_poi != -1){
                    if (!this->poi_already_visited(index_best_poi) &&
                        find(sequence_jour.begin(),sequence_jour.end(),index_best_poi) == sequence_jour.end()){

                        
                    
                        float distance_hotel_poi = this->instance->get_distance_Hotel_POI(current_position, index_best_poi);
                        if (distance_hotel_poi < this->instance->get_POI_Heure_fermeture(index_best_poi)){
                            if (distance_hotel_poi < this->instance->get_POI_Heure_ouverture(index_best_poi)){
                                distance_hotel_poi = this->instance->get_POI_Heure_ouverture(index_best_poi);
                            }

                            float distance_poi_last_poi = this->instance->get_distance_POI_POI(saved_sequence_day[saved_sequence_day.size()-1],index_best_poi);
                            
                            float distance_until_now = distance_hotel_poi + distance_poi_last_poi;

                            if (distance_until_now < this->instance->get_POI_Heure_fermeture(saved_sequence_day[saved_sequence_day.size()-1])){
                                if (distance_until_now < this->instance->get_POI_Heure_ouverture(saved_sequence_day[saved_sequence_day.size()-1])){
                                    distance_until_now = this->instance->get_POI_Heure_ouverture(saved_sequence_day[saved_sequence_day.size()-1]);
                                }

                                if (distance_until_now + last_distance_poi_hotel_destination < distance_max_jour){
                                    add_to_poi_visited(index_best_poi);
                                    sequence_jour.push_back(index_best_poi);
                                    

                                    hotel = false;
                                    current_position = index_best_poi;
                                    this->total_distance_for_trip = distance_hotel_poi;
                                
                                }

                                
                            }

                            
                        }

                    }
                }
            }else{
                index_best_poi = best_poi_from_poi(current_position,total_distance_for_trip, id_jour);
                if (index_best_poi != -1){
                    
                    if (!this->poi_already_visited(index_best_poi) &&
                        find(sequence_jour.begin(),sequence_jour.end(),index_best_poi) == sequence_jour.end()){
                            
                        float distance_between_poi = total_distance_for_trip + this->instance->get_distance_POI_POI(current_position,index_best_poi);

                        if (distance_between_poi < distance_max_jour){
                            if(distance_between_poi < this->instance->get_POI_Heure_fermeture(index_best_poi)){

                                if (distance_between_poi < this->instance->get_POI_Heure_ouverture(index_best_poi)){
                                    distance_between_poi = this->instance->get_POI_Heure_ouverture(index_best_poi);
                                }

                                float distance_poi_last_poi = this->instance->get_distance_POI_POI(saved_sequence_day[saved_sequence_day.size()-1],index_best_poi);

                                if (distance_poi_last_poi < this->instance->get_POI_Heure_fermeture(saved_sequence_day[saved_sequence_day.size()-1])){
                                    
                                    if (distance_between_poi + distance_poi_last_poi + last_distance_poi_hotel_destination < distance_max_jour){
                                    
                                        add_to_poi_visited(index_best_poi);
                                        sequence_jour.push_back(index_best_poi);

                                        current_position = index_best_poi;
                                        
                                        total_distance_for_trip = distance_between_poi;

                                    }
                                    
                                }

                                

                                
                            }
                        }

                    }
                    
                }
            }
            



            iteration++;
        }
        //cout << "oo -- oo : " << iteration << " - o - " << maxIteration << endl;

        for (int j = 0; j < sequence_jour.size(); ++j){
            int poi_id = sequence_jour[j];
            i_valeur_fonction_objectif += this->instance->get_POI_Score(poi_id);
        }

        
        sequence_jour.push_back(saved_sequence_day[saved_sequence_day.size()-1]);
        sequence_Id_Poi_Par_Jour[id_jour] = sequence_jour;

        


    }

  

}

// Build hotel for all the journey.
vector<int> NearestNeighbor::build_list_hotel_for_all_journey()
{

    vector<int> build_all_hotel = vector<int>();
    build_all_hotel.push_back(this->instance->get_Id_Hotel_depart());

    for (auto h: hotel_Intermedaire){
        build_all_hotel.push_back(h);
    }
    build_all_hotel.push_back(this->instance->get_Id_Hotel_Arrivee());

    return build_all_hotel;
}

// Determine the objective fonction value for day trip.
int NearestNeighbor::determine_objective_function_value(vector<int> sequence_poi)
{
    int sum = 0;
    for (auto s : sequence_poi){
        sum += this->instance->get_POI_Score(s);
    }
    return sum;
}

// Order poi depending on the value of poi close time.
void NearestNeighbor::order_poi_by_fermeture()
{
    vector<int> list_poi = vector<int>();
    
    for (int p = 0; p < this->instance->get_Nombre_POI(); ++p){
        list_poi.push_back(p);
    }

    sort(list_poi.begin(),list_poi.end(), [this](int a, int b){
        return (this->instance->get_POI_Score(a) > this->instance->get_POI_Score(b));
    });

    

    for (int p = 0; p < list_poi.size(); ++p){
        cout << "# - # Ordered POI : " << list_poi[p] << "  @ - @ " << this->instance->get_POI_Heure_ouverture(list_poi[p]) << " - " << this->instance->get_POI_Heure_fermeture(list_poi[p]) << endl;
    }

}

// Add unvisited poi to the solution.
void NearestNeighbor::add_unvisited_poi_to_the_solution()
{
    vector<int> all_hotel = build_list_hotel_for_all_journey();

    //cout << "oo -- oo SIZE = " << sequence_Id_Poi_Par_Jour.size() << " - " << this->instance->get_Nombre_Jour() << endl;

    for(int sequence = 0; sequence < this->sequence_Id_Poi_Par_Jour.size(); ++sequence){


        int hotel_depart = all_hotel[sequence];
        int hotel_arrive = all_hotel[sequence+1];

        vector<int> current_sequence = this->sequence_Id_Poi_Par_Jour[sequence];

        //cout << "# - # Total distance : " << determine_distance_for_day_journey_except_a_poi(hotel_depart, hotel_arrive, current_sequence) << " - " << this->instance->get_POI_Duree_Max_Voyage(sequence) << endl;

        vector<int> visited = this->get_unvisited_poi();
        for (auto poi : visited){

            int pos = -1;
            int best_objective_value = -1;
            float best_ratio = -1.0f;
            
            vector<int> tmp_sequence;
            for (int index = 0; index < current_sequence.size() - 1; ++index){

                tmp_sequence = vector<int>(current_sequence.size()+1,-1);

                float distance_first_poi = this->instance->get_distance_POI_POI(current_sequence[index],poi);
                float distance_second_poi = this->instance->get_distance_POI_POI(current_sequence[index + 1],poi);

                if (distance_first_poi <= this->instance->get_POI_Heure_fermeture(poi) && 
                    distance_second_poi <= this->instance->get_POI_Heure_fermeture(current_sequence[index + 1])){
                    
                    if (distance_first_poi < this->instance->get_POI_Heure_ouverture(poi)){
                        distance_first_poi = this->instance->get_POI_Heure_ouverture(poi);
                    }
                    if (distance_second_poi < this->instance->get_POI_Heure_ouverture(current_sequence[index + 1])){
                        distance_second_poi = this->instance->get_POI_Heure_ouverture(current_sequence[index + 1]);
                    }

                    tmp_sequence[index+1] = poi;
                    for (int i = 0; i < current_sequence.size(); ++i){
                        if (i <= index){
                            tmp_sequence[i] = current_sequence[i];
                        }else{
                            tmp_sequence[i+1] = current_sequence[i];
                        }
                    }
                    


                    if (determine_distance_for_day_journey(hotel_depart, hotel_arrive,tmp_sequence) < this->instance->get_POI_Duree_Max_Voyage(sequence)){
                        //cout << poi << " - " << current_sequence[index] << " : " << this->instance->get_POI_Score(current_sequence[index])/distance_first_poi << " ( - ) : " << determine_distance_for_day_journey_except_a_poi(hotel_depart, hotel_arrive,tmp_sequence) << " - " << this->instance->get_POI_Duree_Max_Voyage(sequence) << endl;

                        if (best_ratio <= this->instance->get_POI_Score(current_sequence[index])/distance_first_poi){
                            best_ratio = this->instance->get_POI_Score(current_sequence[index])/distance_first_poi;
                            pos = index;
                            break;
                        }
                    
                    }
                    


                }
                
            }

            if (pos != -1){
                
                vector<vector<int>> all_poi_sequence = sequence_Id_Poi_Par_Jour;
                

                all_poi_sequence[sequence] = tmp_sequence;

                bool valid = sequence_is_valid(all_poi_sequence);

                

                if (valid){

                    sequence_Id_Poi_Par_Jour[sequence] = tmp_sequence;
                    current_sequence = tmp_sequence;
                    i_valeur_fonction_objectif = 0;
                    for(auto s : sequence_Id_Poi_Par_Jour){
                        i_valeur_fonction_objectif += determine_objective_function_value(s);
                    }

                    add_to_poi_visited(poi);

                    visited.erase(remove(visited.begin(),visited.end(),poi),visited.end());

                    

                }


            }

        }
    }
}

// 2-OPT
void NearestNeighbor::two_opt()
{
    vector<int> all_hotel = build_list_hotel_for_all_journey();

    for(int sequence = 0; sequence < this->sequence_Id_Poi_Par_Jour.size(); ++sequence){

        int hotel_depart = all_hotel[sequence];
        int hotel_arrive = all_hotel[sequence+1];

        vector<int> current_sequence = this->sequence_Id_Poi_Par_Jour[sequence];

        cout << "@@ -- @@ : " << this->instance->get_distance_POI_POI(0,2) << " - " << this->instance->get_distance_POI_POI(5, 9) << endl;

        cout << "@@ -- @@ : " << this->instance->get_distance_POI_POI(0,5) << " - " << this->instance->get_distance_POI_POI(2, 9) << endl;

        cout << "# - # Sequence " << sequence << " : " << endl;
        for (int i = 1; i < current_sequence.size() - 2; ++i){
            for (int j = i + 1; j < current_sequence.size() - 1; ++j){

                float former_first_distance = this->instance->get_distance_POI_POI(current_sequence[i - 1], current_sequence[i]) ;
                float former_second_distance = this->instance->get_distance_POI_POI(current_sequence[i], current_sequence[j]);
                float former_third_distance = this->instance->get_distance_POI_POI(current_sequence[j], current_sequence[j + 1]);

                cout << "@ - @ Former distance = " << former_first_distance + former_third_distance << endl;


                float new_first_distance = this->instance->get_distance_POI_POI(current_sequence[i - 1], current_sequence[j]);
                float new_second_distance = this->instance->get_distance_POI_POI(current_sequence[j], current_sequence[i]);
                float new_third_distance = this->instance->get_distance_POI_POI(current_sequence[i], current_sequence[j + 1]);

                cout << "@ - @ New distance = " << new_first_distance + new_third_distance << endl;


                float delta_distance = (former_first_distance + former_third_distance) - (new_first_distance + new_third_distance);


                //if (delta_distance > 0) continue;
                cout << "( " << current_sequence[i - 1] << ", " << current_sequence[j] << " )" << " <--> " << "( " << current_sequence[i] << ", " << current_sequence[j + 1] << " )" << endl;
            }
        }
        cout << endl;

    }
}

// SWAP
void NearestNeighbor::swap()
{
    if (sequence_is_valid(sequence_Id_Poi_Par_Jour)){
        vector<int> all_hotel = build_list_hotel_for_all_journey();

        for(int sequence = 0; sequence < this->sequence_Id_Poi_Par_Jour.size(); ++sequence){
            int hotel_depart = all_hotel[sequence];
            int hotel_arrive = all_hotel[sequence+1];

            vector<int> current_sequence = this->sequence_Id_Poi_Par_Jour[sequence];

            
            for (int i = 0; i < current_sequence.size() - 2; ++i){

                float first_distance = this->instance->get_distance_POI_POI(current_sequence[i], current_sequence[i + 1]);
                float second_distance = this->instance->get_distance_POI_POI(current_sequence[i], current_sequence[i + 2]);
                
                float third_distance = this->instance->get_distance_POI_POI(current_sequence[i + 1], current_sequence[i + 2]);

                if (first_distance <= this->instance->get_POI_Heure_fermeture(current_sequence[i]) &&
                    second_distance <= this->instance->get_POI_Heure_fermeture(current_sequence[i + 2])){


                    vector<vector<int>> tmp_all_sequence = this->sequence_Id_Poi_Par_Jour;
                    vector<int> tmp_sequence = current_sequence;

                    std::swap(tmp_sequence[i], tmp_sequence[i + 1]);
                    

                    tmp_all_sequence[sequence] = tmp_sequence;

                    bool b = sequence_is_valid(tmp_all_sequence);

                    if (b){

                        if (this->instance->get_POI_Score(current_sequence[i]) <= this->instance->get_POI_Score(current_sequence[i + 1])){
                        
                            this->sequence_Id_Poi_Par_Jour = tmp_all_sequence;

                            i_valeur_fonction_objectif = 0;
                            for(auto s : sequence_Id_Poi_Par_Jour){
                                i_valeur_fonction_objectif += determine_objective_function_value(s);
                            }
                        
                        }
                        break;
                    }


                }


                
            }
            

        }
    }
}

// Check the validity of a sequence
bool NearestNeighbor::sequence_is_valid(vector<vector<int>> all_sequence)
{

    int obj = 0;
                
    for (auto seq : all_sequence){
        obj += determine_objective_function_value(seq);
    }

    Checker* solution = new Checker();

    vector<int> v_i_tmp ;

    v_i_tmp.clear();

    for (int i = 0; i < get_intermediate_hotel().size(); ++i){
        solution->v_Id_Hotel_Intermedaire.push_back(get_intermediate_hotel()[i]);
    }

    for (int i = 0; i < get_date_depart().size(); ++i){
        solution->v_Date_Depart.push_back(get_date_depart()[i]);
    }

    for (int i = 0; i < all_sequence.size(); ++i){
        
        v_i_tmp = vector<int>();
        for (int j = 0; j < all_sequence[i].size(); ++j){
            v_i_tmp.push_back(all_sequence[i][j]);
            
        }
        solution->v_v_Sequence_Id_Par_Jour.push_back(v_i_tmp);
        
    }

    solution->i_valeur_fonction_objectif = (int)obj;

    bool b = solution->Verification_Solution(this->instance);

    return b;
}

// Determine the distance for the trip.
float NearestNeighbor::determine_distance_for_day_journey(int hotel_depart, int hotel_arrive, vector<int> sequence_poi)
{

    float distance = 0.0f;

    float first_distance = this->instance->get_distance_Hotel_POI(hotel_depart, sequence_poi[0]);
    float second_distance = this->instance->get_distance_Hotel_POI(hotel_arrive, sequence_poi[sequence_poi.size()-1]);

    
    distance = first_distance + second_distance;

    for(int index = 0; index < sequence_poi.size() - 1; ++index){
        float current_distance = this->instance->get_distance_POI_POI(sequence_poi[index], sequence_poi[index + 1]);
        
        //cout << sequence_poi[index] << " - " << sequence_poi[index+1] << " $$ -- $$ : " << current_distance << endl;
        distance += current_distance;
    }

   

    return distance;
}

// Improvement of the solution by using the Metaheuristic GRASP.
void NearestNeighbor::GRASP()
{

    heuristic_nearest_neighbor();

    int MaxIteration = this->instance->get_Nombre_Jour();

    int iteration = 0;

    while (iteration < MaxIteration){
        add_unvisited_poi_to_the_solution();
        swap();

        iteration++;
    }

}



// Best poi from a hotel
int NearestNeighbor::best_poi_from_hotel(int hotel, float total_distance, int id_jour)
{

    float best_ratio = -1.0f;
    int best_poi_index = -1;


    float distance_max_by_day = this->instance->get_POI_Duree_Max_Voyage(id_jour);
    
    for (int p = 0; p < this->instance->get_Nombre_POI(); ++p){
        if (!poi_already_visited(p)){
            float distance_hotel_poi = this->instance->get_distance_Hotel_POI(hotel,p);

            if (distance_hotel_poi < this->instance->get_POI_Duree_Max_Voyage(id_jour)){
                if (distance_hotel_poi <= this->instance->get_POI_Heure_fermeture(p)){

                    if (distance_hotel_poi < this->instance->get_POI_Heure_ouverture(p)){
                        distance_hotel_poi = this->instance->get_POI_Heure_ouverture(p);
                    }

                    float corrected_distance = distance_hotel_poi + total_distance;

                    if (corrected_distance <= distance_max_by_day){
                        if (best_ratio < this->instance->get_POI_Score(p)/(distance_hotel_poi)){
                            best_ratio = this->instance->get_POI_Score(p)/(distance_hotel_poi);
                            best_poi_index = p;
                            
                        }
                    }

                }
            }

            
        }
        

    }

    return best_poi_index;
}

// Best poi from a specific poi
int NearestNeighbor::best_poi_from_poi(int poi_index, float total_distance, int id_jour)
{

    float best_ratio = -1.0;
    int best_poi_index = -1;

    float distance_max_by_day = this->instance->get_POI_Duree_Max_Voyage(id_jour);

    for (int p = 0; p < this->instance->get_Nombre_POI(); ++p){

        if (!poi_already_visited(p) && p != poi_index){
            
            float distance_poi_poi = total_distance + this->instance->get_distance_POI_POI(poi_index,p);
            if (distance_poi_poi < distance_max_by_day){
                if (distance_poi_poi < this->instance->get_POI_Heure_fermeture(p)){

                    
                    if (best_ratio < this->instance->get_POI_Score(p)/(this->instance->get_distance_POI_POI(poi_index,p))){
                            
                        best_ratio = this->instance->get_POI_Score(p)/(this->instance->get_distance_POI_POI(poi_index,p));
                        best_poi_index = p;
                        
                    }

                }
            }
            
        }

    }

    return best_poi_index;
}
