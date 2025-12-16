#include "SolutionInitiale.hpp"


SolutionInitiale::SolutionInitiale(Instance *instance)
{
    this->instance = instance;

    hotel_depart = this->instance->get_Id_Hotel_depart();

}

float SolutionInitiale::distance_to_next_poi_or_hotel(int index_current, int index_next_poi, int hotel_or_poi)
{
    switch(hotel_or_poi){
        case HOTEL:
            return this->instance->get_distance_Hotel_POI(index_current, index_next_poi);
        case POI:
            return this->instance->get_distance_POI_POI(index_current, index_next_poi);
        default:
            break;
    }
    return 0.0f;
}

bool SolutionInitiale::poi_is_accessible_from_current_position(int poi_index, float current_distance, int current_position, bool is_hotel)
{
    if (current_distance >= this->instance->get_POI_Heure_fermeture(poi_index)){
        return false;
    }
    float distance_to_poi = distance_to_next_poi_or_hotel(current_position, poi_index, is_hotel ? HOTEL : POI);
    float arrival_time = current_distance + distance_to_poi;

    return arrival_time >= this->instance->get_POI_Heure_ouverture(poi_index) && 
           arrival_time <= this->instance->get_POI_Heure_fermeture(poi_index);
}


int SolutionInitiale::best_poi_to_visit_from_current_position(int current_position, float current_distance, bool is_hotel)
{
    int best_poi_index = -1;
    float best_score_ratio = -1.0f;
    for (int poi_index = 0; poi_index < this->instance->get_Nombre_POI(); ++poi_index){
        if (!poi_already_visited(poi_index) && poi_is_accessible_from_current_position(poi_index, current_distance, current_position, is_hotel)){
            float distance_to_poi = distance_to_next_poi_or_hotel(current_position, poi_index, is_hotel ? HOTEL : POI);
            float score_ratio = this->instance->get_POI_Score(poi_index) / distance_to_poi;
            if (score_ratio > best_score_ratio){
                best_score_ratio = score_ratio;
                best_poi_index = poi_index;
            }
        }
    }
    return best_poi_index;
}

void SolutionInitiale::build_solution()
{
    
    int nombre_jour = this->instance->get_Nombre_Jour();
    
    bool isMovable = true;

    int depart = this->hotel_depart;
    int depart_or_intermediate_hotel = this->hotel_depart;
    cout << "Hotel Depart : " << depart << endl;
    
    for(int jour_index = 0; jour_index < nombre_jour; ++jour_index){
        
        float max_distance_jour = this->instance->get_POI_Duree_Max_Voyage(jour_index);
        float total_distance = 0.0f;
        vector<int> sequence_jour = vector<int>();
        


        while(isMovable){
            cout << "Current total distance : " << total_distance << " / " << max_distance_jour << endl;
            int poi_best_index = best_poi_to_visit_from_current_position(depart, total_distance, is_hotel);
            if (poi_best_index != -1){
                cout << "Visiting POI Index : " << poi_best_index << endl;

                total_distance += distance_to_next_poi_or_hotel(depart, poi_best_index, is_hotel ? HOTEL : POI);
                if (total_distance < max_distance_jour){

                    sequence_jour.push_back(poi_best_index);

                    add_to_poi_visited(poi_best_index);

                    
                    
                    depart = poi_best_index;
                    is_hotel = false;
                    cout << "Total distance after visit : " << total_distance << endl;
                } else {
                    cout << "Exceeded max distance after visit. Current : " << total_distance << " Max : " << max_distance_jour << endl;
                    // Remove last added POI
                    sequence_jour.pop_back();
                    // Remove from visited
                    poi_visited.pop_back();
                    total_distance -= distance_to_next_poi_or_hotel(depart, poi_best_index, is_hotel ? HOTEL : POI);
                    cout << "Reverted total distance : " << total_distance << endl;
                    // End of day

                    int dept = find_best_intermediate_hotel_from_poi(depart, total_distance, max_distance_jour);

                    float dist = this->instance->get_distance_Hotel_POI(depart_or_intermediate_hotel, sequence_jour[0]);

                    for (int k = 1; k < sequence_jour.size(); ++k){
                        float dist_tmp = this->instance->get_distance_POI_POI(sequence_jour[k-1], sequence_jour[k]);
                        dist += dist_tmp;
                    }
                    dist += this->instance->get_distance_Hotel_POI(dept,sequence_jour[sequence_jour.size()-1]);

                    cout << "### Distance calculation for day " << jour_index << " : " << dist << " / " << max_distance_jour << endl;

                    while(dist > max_distance_jour && !sequence_jour.empty()){
                        int removed_poi = sequence_jour.back();
                        sequence_jour.pop_back();
                        poi_visited.pop_back();
                        cout << "Removing POI " << removed_poi << " to fit distance." << endl;

                        // Recalculate distance
                        dist = this->instance->get_distance_Hotel_POI(depart_or_intermediate_hotel, sequence_jour[0]);

                        for (int k = 1; k < sequence_jour.size(); ++k){
                            float dist_tmp = this->instance->get_distance_POI_POI(sequence_jour[k-1], sequence_jour[k]);
                            dist += dist_tmp;
                        }
                        dist += this->instance->get_distance_Hotel_POI(dept,sequence_jour[sequence_jour.size()-1]);
                    }
                    


                    sequence_Id_Poi_Par_Jour.push_back(sequence_jour);

                    for (int poi_id : sequence_jour){
                        i_valeur_fonction_objectif += this->instance->get_POI_Score(poi_id);
                    }

                    isMovable = false;
                    cout << "Next Hotel Depart = " << depart << endl;
                    
                    
                }

                

            } else {
                sequence_Id_Poi_Par_Jour.push_back(sequence_jour);
                isMovable = false;
            }
        }
        cout << "End of Day " << jour_index << " - Total Distance : " << total_distance << endl;

        if (poi_visited.size() == this->instance->get_Nombre_POI()){
            cout << "All POIs visited." << endl;
            break;  
        }else {

            // Find best intermediate hotel
            depart = find_best_intermediate_hotel_from_poi(depart, total_distance, max_distance_jour);

            
            
            depart_or_intermediate_hotel = depart;
            cout << "Next Hotel Depart = " << depart << endl;
            add_to_intermadiate_hotel(depart);
            v_Date_Depart.push_back(0.0f);
            is_hotel = true;
            isMovable = true;
        }

        cout << "Jour " << jour_index << " completed. POIs visited so far : " << poi_visited.size() << endl;


    }
    cout << "Total POIs visited : " << poi_visited.size() << " out of " << this->instance->get_Nombre_POI() << endl;

    int last_intermediate_hotel = hotel_Intermedaire.empty() ? this->hotel_depart : hotel_Intermedaire[hotel_Intermedaire.size()-1];
    float dists = this->instance->get_distance_Hotel_POI(last_intermediate_hotel, sequence_Id_Poi_Par_Jour[sequence_Id_Poi_Par_Jour.size()-1][0]);
    for (int k = 1; k < sequence_Id_Poi_Par_Jour[sequence_Id_Poi_Par_Jour.size()-1].size(); ++k){
        float dist_tmp = this->instance->get_distance_POI_POI(sequence_Id_Poi_Par_Jour[sequence_Id_Poi_Par_Jour.size()-1][k-1], sequence_Id_Poi_Par_Jour[sequence_Id_Poi_Par_Jour.size()-1][k]);
        dists += dist_tmp;
    }
    dists += this->instance->get_distance_Hotel_POI(this->instance->get_Id_Hotel_Arrivee(),sequence_Id_Poi_Par_Jour[sequence_Id_Poi_Par_Jour.size()-1][sequence_Id_Poi_Par_Jour[sequence_Id_Poi_Par_Jour.size()-1].size()-1]);
    cout << "### Final Distance calculation to arrival hotel : " << dists << endl;

    

}

bool SolutionInitiale::poi_is_possible_to_visit_in_day(int poi_index, int current_position, bool is_hotel, float max_distance_jour)
{

    float distance_to_poi = distance_to_next_poi_or_hotel(current_position, poi_index, is_hotel ? HOTEL : POI);
    if (distance_to_poi + total_distance_for_trip < max_distance_jour){

        float ouveture = this->instance->get_POI_Heure_ouverture(poi_index);
        float fermeture = this->instance->get_POI_Heure_fermeture(poi_index);

        if (total_distance_for_trip + distance_to_poi >= fermeture){
            return false;
        }else{
            if (total_distance_for_trip + distance_to_poi > ouveture){
                this->total_distance_for_trip += distance_to_poi;
                if (is_hotel){
                    if (v_Date_Depart.size() < this->instance->get_Nombre_Jour()){
                        this->v_Date_Depart.push_back(0.0f);
                    }
                }
            }else{
                
                if (is_hotel){
                    if (v_Date_Depart.size() < this->instance->get_Nombre_Jour()){
                        this->v_Date_Depart.push_back(ouveture - (total_distance_for_trip + distance_to_poi));
                    }
                    
                }
                this->total_distance_for_trip += ouveture;
            }

            
            return true;
        }

        
    }

    return false;
}

// Hotel first approach
void SolutionInitiale::build_Intermediate_Hotel_List()
{
    int depart = this->instance->get_Id_Hotel_depart();
    int arrivee = this->instance->get_Id_Hotel_Arrivee();

    for (int jour_index = 0; jour_index < this->instance->get_Nombre_Jour() - 1; ++jour_index){
        
        for (int hotel_index = 0; hotel_index < this->instance->get_Nombre_Hotel(); ++hotel_index){
            
            if (hotel_index != depart && hotel_index != arrivee){
                
                int index_best_poi = best_poi_between_two_hotels(depart, hotel_index, this->instance->get_POI_Duree_Max_Voyage(jour_index));
                //cout << "After poi : " << index_best_poi << " H-index = " << hotel_index << " # Hotel : " << depart << " - " << hotel_index << endl;
                
                if (index_best_poi != -1){
                    if (!hotel_already_in_intermedaite_hotel(hotel_index)){
                        if (hotel_Intermedaire.size() < this->instance->get_Nombre_Jour() -1){
                            hotel_Intermedaire.push_back(hotel_index);

                            if (find(best_poi_from_hotel.begin(), best_poi_from_hotel.end(), index_best_poi) == best_poi_from_hotel.end()){
                                best_poi_from_hotel.push_back(index_best_poi);
                            }
                            
                        }
                            
                    }
                    
                }else{
                  cout << "No POI found between hotels " << depart << " and " << hotel_index << endl;
                }
                
            }
        }
        cout << "\nEnd of search for day " << jour_index << "\n" << endl;
    }

}

int SolutionInitiale::best_poi_between_two_hotels(int hotel_depart_index, int hotel_arrivee_index, float max_distance_jour)
{
    int min_poi_index = -1;
    float min_distance = numeric_limits<float>::max();
    
    for (int k = 0; k < this->instance->get_Nombre_POI(); ++k){

        float distance_to_poi_from_depart = this->instance->get_distance_Hotel_POI(hotel_depart_index, k);
        float distance_to_arrivee_from_poi = this->instance->get_distance_Hotel_POI(hotel_arrivee_index, k);
        
        if (distance_to_poi_from_depart + distance_to_arrivee_from_poi < max_distance_jour){

            if (distance_to_poi_from_depart < this->instance->get_POI_Heure_ouverture(k)){
                distance_to_poi_from_depart = this->instance->get_POI_Heure_ouverture(k);
            }

            if (distance_to_poi_from_depart < this->instance->get_POI_Heure_fermeture(k)){
                if (distance_to_arrivee_from_poi <= min_distance){
                    min_distance = distance_to_arrivee_from_poi;
                    min_poi_index = k;
                }
            }

            
            
            
        }
    }

    return min_poi_index;
}

//
void SolutionInitiale::solution_by_building_hotel_first()
{
    build_Intermediate_Hotel_List();
    

    int current_hotel = this->instance->get_Id_Hotel_depart();

    bool isMovable = true;

    int depart = this->hotel_depart;
    
    for (int j = 0; j < this->hotel_Intermedaire.size() + 1; ++j){

        
        float max_distance_jour = this->instance->get_POI_Duree_Max_Voyage(j);

        if (j <= this->hotel_Intermedaire.size()-1){
            // Add this POI at the end of the day sequence POI
            int last_poi_before_hotel = best_poi_between_two_hotels(current_hotel,this->hotel_Intermedaire[j],this->instance->get_POI_Duree_Max_Voyage(j));
            cout << "Last POI before hotel " << this->hotel_Intermedaire[j] << " is POI " << last_poi_before_hotel << endl;

            float distance_poi_to_last_hotel = this->instance->get_distance_Hotel_POI(this->hotel_Intermedaire[j], last_poi_before_hotel);
            cout << "Distance from last POI to next hotel : " << distance_poi_to_last_hotel << endl;

            vector<int> poi_sequence_for_day = vector<int>();

            while(isMovable){
                //cout << "Current total distance : " << total_distance_for_trip << " / " << max_distance_jour << endl;
                int poi_best_index = best_poi_to_visit_from_current_position(depart, total_distance_for_trip, is_hotel);

                //cout << "Best POI index to visit : " << poi_best_index << endl;

                if (poi_best_index != -1){
                    //cout << "Visiting POI Index : " << poi_best_index << endl;
                    if (poi_is_possible_to_visit_in_day(poi_best_index, depart, is_hotel, max_distance_jour)){

                        if (total_distance_for_trip <= this->instance->get_POI_Heure_ouverture(last_poi_before_hotel)){
                            poi_sequence_for_day.push_back(poi_best_index);

                            add_to_poi_visited(poi_best_index);

                            
                            
                            depart = poi_best_index;
                            is_hotel = false;
                            //cout << "Total distance after visit : " << total_distance_for_trip << endl;

                        }else{
                            depart = this->hotel_Intermedaire[j];
                            current_hotel = this->hotel_Intermedaire[j];
                            //cout << "Cannot visit more POIs today, last POI before hotel cannot be reached in time." << endl;
                            isMovable = false;
                        }
                        
                    } else {
                        //cout << "Exceeded max distance after visit. Current : " << total_distance_for_trip << " Max : " << max_distance_jour << endl;
                        
                        // End of day
                        depart = this->hotel_Intermedaire[j];
                        current_hotel = this->hotel_Intermedaire[j];
                        isMovable = false;
                        //cout << "Next Hotel Depart = " << depart << endl;
                        
                        
                    }

                } else {
                    depart = this->hotel_Intermedaire[j];
                    current_hotel = this->hotel_Intermedaire[j];
                    isMovable = false;
                }
            }

            poi_sequence_for_day.push_back(last_poi_before_hotel);
            add_to_poi_visited(last_poi_before_hotel);
            sequence_Id_Poi_Par_Jour.push_back(poi_sequence_for_day);
            total_distance_for_trip  = 0.0f;
            depart = this->hotel_Intermedaire[j];
            current_hotel = this->hotel_Intermedaire[j];

        }else{

            cout << "Last day " << j << " - " << this->instance->get_Nombre_Jour() << endl;
            int last_poi_before_last_hotel = best_poi_between_two_hotels(current_hotel, this->instance->get_Id_Hotel_Arrivee(),this->instance->get_POI_Duree_Max_Voyage(j));
            cout << "Last POI before last # hotel " << this->hotel_Intermedaire[j-1] << " is POI " << last_poi_before_last_hotel << endl;

            vector<int> poi_sequence_for_last_day = vector<int>();
            isMovable = true;
            total_distance_for_trip = 0.0f;
            float last_max_distance_day = this->instance->get_POI_Duree_Max_Voyage(j);
            vector<int> poi_not_visited = vector<int>();
            vector<float> ratio_score_distance = vector<float>();
            vector<float> ratio_score_distance_between_poi = vector<float>();

            

            for (int i = 0; i < this->instance->get_Nombre_POI(); ++i){
                
                if (!poi_already_visited(i)){
                    poi_not_visited.push_back(i);
                }
            }

            
            for (auto p : poi_not_visited){
                ratio_score_distance.push_back(this->instance->get_POI_Score(p)/this->instance->get_distance_Hotel_POI(this->hotel_Intermedaire[j-1],p));
            }

            is_hotel = true;
            int best = poi_not_visited[max_Index(ratio_score_distance)];
            cout << "? Max POI " << poi_not_visited[max_Index(ratio_score_distance)] << endl;
            
            if (this->distance_to_next_poi_or_hotel(this->hotel_Intermedaire[j-1],best,is_hotel?HOTEL:POI) + total_distance_for_trip < this->instance->get_POI_Heure_fermeture(best)){
                float check_distance = this->distance_to_next_poi_or_hotel(this->hotel_Intermedaire[j-1],best,is_hotel?HOTEL:POI) + total_distance_for_trip;
                float distance_poi_arrive_hotel = this->instance->get_distance_Hotel_POI(this->instance->get_Id_Hotel_Arrivee(),last_poi_before_last_hotel);
                if (last_max_distance_day - check_distance > distance_poi_arrive_hotel){
                    poi_sequence_for_last_day.push_back(best);
                    add_to_poi_visited(best);
                    total_distance_for_trip += this->distance_to_next_poi_or_hotel(this->hotel_Intermedaire[j-1],best,is_hotel?HOTEL:POI);
                    poi_not_visited.erase(poi_not_visited.begin() + max_Index(ratio_score_distance));
                }
              
            }

            is_hotel = false;
            int depart = best;

            float distance_poi_arrive_hotel = this->instance->get_distance_Hotel_POI(this->instance->get_Id_Hotel_Arrivee(),last_poi_before_last_hotel);

            while(poi_not_visited.size() > 1){
                float max_ratio = -1.0f;
                int index_max = -1;

                for (int i = 0; i < poi_not_visited.size(); ++i){
                    
                    if (max_ratio < this->instance->get_POI_Score(poi_not_visited[i]) /this->instance->get_distance_POI_POI(depart,poi_not_visited[i])){
                        max_ratio = this->instance->get_POI_Score(poi_not_visited[i]) /this->instance->get_distance_POI_POI(depart,poi_not_visited[i]);
                        index_max = i;
                    }
                }

                if (this->distance_to_next_poi_or_hotel(depart,poi_not_visited[index_max],is_hotel?HOTEL:POI) + total_distance_for_trip <= this->instance->get_POI_Heure_fermeture(poi_not_visited[index_max])){
                    float check_distance = this->distance_to_next_poi_or_hotel(depart,poi_not_visited[index_max],is_hotel?HOTEL:POI) + total_distance_for_trip;
                    if (last_max_distance_day - check_distance > distance_poi_arrive_hotel){

                        poi_sequence_for_last_day.push_back(poi_not_visited[index_max]);
                        add_to_poi_visited(poi_not_visited[index_max]);

                        total_distance_for_trip += this->distance_to_next_poi_or_hotel(depart,poi_not_visited[index_max],is_hotel?HOTEL:POI);

                        cout << "Index Max = " << index_max << endl;

                        cout << "Val ? : ? " << poi_not_visited[index_max] << endl;
                        depart = poi_not_visited[index_max];
                        poi_not_visited.erase(poi_not_visited.begin() + index_max);
                    }else{
                        cout << "What ?" << endl;
                        poi_not_visited = vector<int>();
                    }
                }else{
                    cout << "Which ?" << endl;
                    poi_not_visited = vector<int>();
                }

                
                
            }

            

            

            poi_sequence_for_last_day.push_back(last_poi_before_last_hotel);
            sequence_Id_Poi_Par_Jour.push_back(poi_sequence_for_last_day);
            

            
            

        }

        
        
    }

    
    
    

    for (int i = 0; i < sequence_Id_Poi_Par_Jour.size(); ++i){
        for (int j = 0; j < sequence_Id_Poi_Par_Jour[i].size(); ++j){
            int poi_id = sequence_Id_Poi_Par_Jour[i][j];
            i_valeur_fonction_objectif += this->instance->get_POI_Score(poi_id);
        }
    }   

    
    

}

int SolutionInitiale::poi_between_hotel_and_last_hotel(int hotel_depart_index, int hotel_arrivee_index, float max_distance_jour)
{

    int min_poi_index = -1;
    float min_distance = numeric_limits<float>::max();
    
    for (int k = 0; k < this->instance->get_Nombre_POI(); ++k){
        float distance_to_poi_from_depart = this->instance->get_distance_Hotel_POI(hotel_depart_index, k);
        float distance_to_arrivee_from_poi = this->instance->get_distance_Hotel_POI(hotel_arrivee_index, k);
        
        if (distance_to_poi_from_depart + distance_to_arrivee_from_poi < max_distance_jour){
            if (distance_to_poi_from_depart < this->instance->get_POI_Heure_fermeture(k)){
                if (distance_to_arrivee_from_poi <= min_distance){
                    min_distance = distance_to_arrivee_from_poi;
                    min_poi_index = k;
                }
            }
            
            
        }
    }

    return min_poi_index;

}

int SolutionInitiale::find_best_score_from_poi(vector<int> list_poi_index)
{
    int index = -1;
    int best_score = -1;
    for (int i = 0; i < list_poi_index.size(); ++i){
        int poi_id = list_poi_index[i];
        int poi_score = this->instance->get_POI_Score(poi_id);
        if (poi_score >= best_score){
            best_score = poi_score;
            index = i;
        }
    }
    return index;
}

int SolutionInitiale::max_Index(vector<float> distance)
{
    if (distance.empty()) return -1;

    float max_value = distance[0];
    int max_index = 0;

    for (int i = 1; i < distance.size(); ++i){
        
        if (max_value < distance[i]){
            //cout << "Max "<< max_value << " - " << distance[i] << endl;
            max_value = distance[i];
            max_index = i;
            
        }
    }
    

    return max_index;
}

int SolutionInitiale::find_Min_Index(vector<float> distance)
{
    if (distance.empty()) return -1;

    float min_value = distance[0];
    int min_index = 0;

    for (int i = 1; i < distance.size(); ++i){
        
        if (min_value > distance[i]){
            
            if (min_index != this->instance->get_Id_Hotel_Arrivee()){
                min_value = distance[i];
                min_index = i;
            }
        }
        
    }
    

    return min_index;
}

void SolutionInitiale::add_to_poi_visited(int index_poi)
{
    if (find(poi_visited.begin(),poi_visited.end(),index_poi) == poi_visited.end()){
        this->poi_visited.push_back(index_poi);
    }
}

bool SolutionInitiale::poi_already_visited(int index_poi)
{
    return find(poi_visited.begin(),poi_visited.end(),index_poi) != poi_visited.end();
}

void SolutionInitiale::add_to_intermadiate_hotel(int index)
{
    if (this->hotel_Intermedaire.size() >= this->instance->get_Nombre_Jour() -1){
        return;
    }
    if (find(hotel_Intermedaire.begin(),hotel_Intermedaire.end(),index) == hotel_Intermedaire.end()){
        this->hotel_Intermedaire.push_back(index);
    }
}

bool SolutionInitiale::hotel_already_in_intermedaite_hotel(int index)
{
    return find(hotel_Intermedaire.begin(),hotel_Intermedaire.end(),index) != hotel_Intermedaire.end();
}

int SolutionInitiale::find_best_intermediate_hotel_from_poi(int index_poi, float current_distance, float max_distance)
{
    vector<float> list_distance = vector<float>();

    for (int index = 0; index < this->instance->get_Nombre_Hotel(); ++index){
        if ( index != this->instance->get_Id_Hotel_depart() || current_distance + this->instance->get_distance_Hotel_POI(index,index_poi) < max_distance){
            cout << "Best hotel " << endl;
            list_distance.push_back(this->instance->get_distance_Hotel_POI(index,index_poi));
        }
        
    }

    bool b = (find_Min_Index(list_distance) != -1) ? find_Min_Index(list_distance) : this->instance->get_Id_Hotel_Arrivee();
    cout << "Best hotel index = " << b << endl;
    return b;
}



void SolutionInitiale::display_sequence_poi()
{
    cout << "Solution : ";
    for (int i = 0; i < sequence_Id_Poi_Par_Jour.size(); ++i){
        cout << i << " % ";
        for (int j = 0; j < sequence_Id_Poi_Par_Jour[i].size(); ++j){
            cout << sequence_Id_Poi_Par_Jour[i][j] << " --> ";
        }
        cout << " % ";
    }
    cout << endl;
}


