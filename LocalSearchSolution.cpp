#include "LocalSearchSolution.hpp"
#include "Solution.hpp"



/// Heuristique
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

        for (int j = 0; j < sequence_jour.size(); ++j){
            int poi_id = sequence_jour[j];
            i_valeur_fonction_objectif += this->instance->get_POI_Score(poi_id);
        }

        sequence_jour.push_back(saved_sequence_day[saved_sequence_day.size()-1]);
        sequence_Id_Poi_Par_Jour[id_jour] = sequence_jour;

        


    }

  

}

vector<int> NearestNeighbor::build_list_hotel_for_all_journey()
{

    vector<int> build_all_hotel = vector<int>();
    build_all_hotel.push_back(this->instance->get_Id_Hotel_depart());

    for (auto h: hotel_Intermedaire){
        build_all_hotel.push_back(h);
    }
    build_all_hotel.push_back(this->instance->get_Id_Hotel_Arrivee());

    cout << "All Hotel : ";
    for (auto h : build_all_hotel){
        cout << h << " --> ";
    }
    cout << endl;
    return build_all_hotel;
}

int NearestNeighbor::determine_objective_function_value(vector<int> sequence_poi)
{
    int sum = 0;
    for (auto s : sequence_poi){
        sum += s;
    }
    return sum;
}

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

void NearestNeighbor::GRASP()
{

}



// Works
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
