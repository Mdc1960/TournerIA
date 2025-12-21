#include "LocalSearchSolution.hpp"

void NearestNeighbor::best_nearest_neighbor_by_day(int hotel_depart, int hotel_arrive, vector<int> list_poi_not_visited, int id_jour)
{
    vector<int> sequence_poi_by_day = vector<int>();

    int nb_iteration = 10;
    


    if (id_jour < this->sequence_Id_Poi_Par_Jour.size()){
        float distance_by_day = 0.0f;
        float total_distance_by_day = this->instance->get_POI_Duree_Max_Voyage(id_jour);

        vector<int> sequence = this->sequence_Id_Poi_Par_Jour[id_jour];

        if (!sequence.empty()){
            int index_poi = sequence[sequence.size()-1];

            int counter = 0;
            bool is_hotel = true;
            int depart = hotel_depart;
            while(counter < nb_iteration && distance_by_day < total_distance_by_day){
                int best_poi_index = generate_best_poi_or_hotel_neighbor(index_poi, depart, hotel_arrive, list_poi_not_visited, id_jour, is_hotel);
                if (best_poi_index != -1){
                    cout << "@ - @ Found !" << best_poi_index << endl;
                    index_poi = best_poi_index;
                    is_hotel = false;
                    depart = index_poi;
                    counter++;
                }
            }

        }
    }

}

int NearestNeighbor::generate_best_poi_or_hotel_neighbor(int index_last_poi, int depart, int hotel_arrive, vector<int> list_poi_not_visited, int id_jour, float depart_is_hotel)
{
    float distance_max_by_day = this->instance->get_POI_Duree_Max_Voyage(id_jour);
    //float distance_hotel_depart_poi = this->instance->get_distance_Hotel_POI(depart,index_poi);
    float distance_hotel_arrive_poi = this->instance->get_distance_Hotel_POI(hotel_arrive,index_last_poi);

    float best_ratio = -1.0f;
    int best_poi_index = -1;

    for (auto p : list_poi_not_visited){

        float curent_distance = this->distance_to_next_poi_or_hotel(depart,p, depart_is_hotel? HOTEL:POI);
        float distance_poi_poi = this->instance->get_distance_POI_POI(index_last_poi, p);
        
        
        
        if (curent_distance < this->instance->get_POI_Heure_fermeture(p)){
            if (curent_distance < this->instance->get_POI_Heure_ouverture(p)){
                curent_distance = this->instance->get_POI_Heure_ouverture(p);
            }
            float total_distance = distance_poi_poi + distance_hotel_arrive_poi + curent_distance;
            if (total_distance < distance_max_by_day){
                if (best_ratio < this->instance->get_POI_Score(p)/curent_distance){
                    best_ratio = this->instance->get_POI_Score(p)/curent_distance;
                    best_poi_index = p;
                }
            }
        }

    }

    return best_poi_index;
}

/// Heuristique

void NearestNeighbor::nearest_neighbor()
{

    
    int maxIteration = this->instance->get_Nombre_Hotel() + this->instance->get_Nombre_POI() + 1;
    for (int id_jour = 0; id_jour < this->instance->get_Nombre_Jour(); ++id_jour){

        int iteration = 0;
        float distance_max_jour = this->instance->get_POI_Duree_Max_Voyage(id_jour);
        bool movable = true;
        int current_position = this->instance->get_Id_Hotel_depart();
        bool hotel = true;

        if (id_jour < this->hotel_Intermedaire.size()){

            
            int last_position = hotel_Intermedaire[id_jour];
            
            

            vector<int> saved_sequence_day = sequence_Id_Poi_Par_Jour[id_jour];

            

            float last_distance_poi_hotel = this->instance->get_distance_Hotel_POI(last_position,saved_sequence_day[saved_sequence_day.size()-1]);

            
            this->total_distance_for_trip = 0.0f;
            vector<int> sequence_jour = vector<int>();

            while(movable){
                

                if (hotel){
                    int index_best_poi = best_poi_from_hotel(current_position,total_distance_for_trip, id_jour);
                    if (index_best_poi != -1){
                        //cout << "# - # Current total distance : " << total_distance_for_trip << " / " << distance_max_jour << endl;
                        if (!this->poi_already_visited(index_best_poi) &&
                        find(sequence_jour.begin(),sequence_jour.end(),index_best_poi) == sequence_jour.end()){

                        
                            float distance_hote_poi = this->instance->get_distance_Hotel_POI(current_position, index_best_poi);
                            if (distance_hote_poi < this->instance->get_POI_Heure_fermeture(index_best_poi)){
                                if (distance_hote_poi < this->instance->get_POI_Heure_ouverture(index_best_poi)){
                                    distance_hote_poi = this->instance->get_POI_Heure_ouverture(index_best_poi);
                                }
                                

                                if (distance_hote_poi + last_distance_poi_hotel <= distance_max_jour){

                                    if (!this->poi_already_visited(index_best_poi)){
                                        sequence_jour.push_back(index_best_poi);
                                        add_to_poi_visited(index_best_poi);

                                        hotel = false;
                                        current_position = index_best_poi;
                                        this->total_distance_for_trip += distance_hote_poi;
                                    }
                                    


                                }
                            }

                        }
                    }else{
                        cout << "Best Poi not found !" << endl;
                    }
                    

                }else{
                    //cout << "# - # Total distance ! " << total_distance_for_trip << endl;
                    //cout << "# - # Curent poi ! " << current_position << endl;
                    int index_best_poi = best_poi_from_poi(current_position,total_distance_for_trip, id_jour);
                    //cout << "# - # Best POI ! " << index_best_poi << endl;

                    if (index_best_poi != -1){

                        float distance_between_poi = total_distance_for_trip + this->instance->get_distance_POI_POI(current_position,index_best_poi);

                        if (distance_between_poi < distance_max_jour){
                            if(distance_between_poi < this->instance->get_POI_Heure_fermeture(index_best_poi)){
                                if (distance_between_poi < this->instance->get_POI_Heure_ouverture(index_best_poi)){
                                    distance_between_poi = this->instance->get_POI_Heure_ouverture(index_best_poi);
                                }
                                if (!this->poi_already_visited(index_best_poi)){
                                    sequence_jour.push_back(index_best_poi);

                                    add_to_poi_visited(index_best_poi);
                                    current_position = index_best_poi;
                                    
                                    total_distance_for_trip = distance_between_poi;
                                }
                            }
                        }

                        

                        
                    }else{
                        cout << "# - # Not Found POI" << endl;
                    }

                }
                iteration++;
                if (iteration == maxIteration){
                    movable = false;
                }
                

            }

            for (int j = 0; j < sequence_jour.size(); ++j){
                int poi_id = sequence_jour[j];
                i_valeur_fonction_objectif += this->instance->get_POI_Score(poi_id);
            }

            sequence_jour.push_back(saved_sequence_day[saved_sequence_day.size()-1]);
            sequence_Id_Poi_Par_Jour[id_jour] = sequence_jour;

            

            

        }else{
            cout << "Compute last day action" << endl;
            current_position = hotel_Intermedaire[id_jour-1];
            int last_position = this->instance->get_Id_Hotel_Arrivee();
            vector<int> saved_sequence_day = sequence_Id_Poi_Par_Jour[id_jour];

            float last_distance_poi_hotel = this->instance->get_distance_Hotel_POI(last_position,saved_sequence_day[saved_sequence_day.size()-1]);
            this->total_distance_for_trip = 0.0f;
            vector<int> sequence_jour = vector<int>();

            while(movable){

                if (hotel){
                    int index_best_poi = best_poi_from_hotel(current_position,total_distance_for_trip, id_jour);
                    if (index_best_poi != -1){
                        
                        //cout << "# - # Current total distance : " << total_distance_for_trip << " / " << distance_max_jour << endl;
                        if (!this->poi_already_visited(index_best_poi) &&
                        find(sequence_jour.begin(),sequence_jour.end(),index_best_poi) == sequence_jour.end()){
                            //cout << "# - # Index of best POI : " << index_best_poi << endl;

                            float distance_hotel_poi = this->instance->get_distance_Hotel_POI(current_position, index_best_poi);
                            if (distance_hotel_poi < this->instance->get_POI_Heure_fermeture(index_best_poi)){
                                if (distance_hotel_poi < this->instance->get_POI_Heure_ouverture(index_best_poi)){
                                    distance_hotel_poi = this->instance->get_POI_Heure_ouverture(index_best_poi);
                                }
                                
                                float distance_poi_last_poi = this->instance->get_distance_POI_POI(saved_sequence_day[saved_sequence_day.size()-1],index_best_poi);

                                if (distance_hotel_poi + distance_poi_last_poi + last_distance_poi_hotel <= distance_max_jour){
                                    if (distance_hotel_poi + distance_poi_last_poi < this->instance->get_POI_Heure_fermeture(saved_sequence_day[saved_sequence_day.size()-1])){
                                        
                                        if (!this->poi_already_visited(index_best_poi)){
                                            sequence_jour.push_back(index_best_poi);
                                            add_to_poi_visited(index_best_poi);

                                            hotel = false;
                                            current_position = index_best_poi;
                                            this->total_distance_for_trip += distance_hotel_poi;
                                        }
                                    }
                                    
                                    


                                }
                            }


                        }
                    }else{
                        cout << "Best Poi not found !" << endl;
                    }
                }else{
                    //cout << "# - # About POI" << endl;

                    //cout << "# - # Total distance ! " << total_distance_for_trip << endl;
                    //cout << "# - # Curent poi ! " << current_position << endl;
                    int index_best_poi = best_poi_from_poi(current_position,total_distance_for_trip, id_jour);
                    //cout << "# - # Best POI ! " << index_best_poi << endl;

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

                                    if (distance_between_poi + distance_poi_last_poi + last_distance_poi_hotel <= distance_max_jour){

                                        if (!this->poi_already_visited(index_best_poi)){
                                            sequence_jour.push_back(index_best_poi);

                                            add_to_poi_visited(index_best_poi);
                                            current_position = index_best_poi;
                                            
                                            total_distance_for_trip = distance_between_poi;
                                        }
                                    }

                                    
                                }
                            }
                        }

                        

                        

                        
                    }else{
                        cout << "# - # Not Found POI" << endl;
                    }

                }
                
                
                iteration++;
                if (iteration == maxIteration){
                    movable = false;
                }
            }

            for (int j = 0; j < sequence_jour.size(); ++j){
                int poi_id = sequence_jour[j];
                i_valeur_fonction_objectif += this->instance->get_POI_Score(poi_id);
            }

            sequence_jour.push_back(saved_sequence_day[saved_sequence_day.size()-1]);
            sequence_Id_Poi_Par_Jour[id_jour] = sequence_jour;


            
        }

        
    }
    
    //TODO : Intensifier la recherche locale Pour obtenir une solution optimale.

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

            if (distance_hotel_poi < this->instance->get_POI_Heure_fermeture(p)){

                if (distance_hotel_poi < this->instance->get_POI_Heure_ouverture(p)){
                    distance_hotel_poi = this->instance->get_POI_Heure_ouverture(p);
                }

                float corrected_distance = distance_hotel_poi + total_distance;

                if (corrected_distance <= distance_max_by_day){
                    if (best_ratio < this->instance->get_POI_Score(p)/(corrected_distance)){
                        best_ratio = this->instance->get_POI_Score(p)/(corrected_distance);
                        best_poi_index = p;
                        
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
            
            float distance_poi_poi = this->instance->get_distance_POI_POI(poi_index,p);
            if (total_distance + distance_poi_poi < this->instance->get_POI_Heure_fermeture(p)){

                if (total_distance + distance_poi_poi < this->instance->get_POI_Heure_ouverture(p)){
                    distance_poi_poi = this->instance->get_POI_Heure_ouverture(p);
                }

                

                float corrected_distance = distance_poi_poi + total_distance;

                if (corrected_distance < distance_max_by_day){
                    if (best_ratio <= this->instance->get_POI_Score(p)/(distance_poi_poi)){
                        
                        best_ratio = this->instance->get_POI_Score(p)/(distance_poi_poi);
                        best_poi_index = p;
                        //cout << "# - # POI : " << p << " - ScoreRatio : " << best_ratio  << " - Score : " << this->instance->get_POI_Score(p) << " - Distance : " << distance_poi_poi << endl;
                    }
                    
                }

            }
        }

    }

    return best_poi_index;
}
