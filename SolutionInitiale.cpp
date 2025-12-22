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
                /*if (is_hotel){
                    if (v_Date_Depart.size() < this->instance->get_Nombre_Jour()){
                        this->v_Date_Depart.push_back(0.0f);
                    }
                }*/
            }else{
                
                /*if (is_hotel){
                    if (v_Date_Depart.size() < this->instance->get_Nombre_Jour()){
                        this->v_Date_Depart.push_back(ouveture - (total_distance_for_trip + distance_to_poi));
                    }
                    
                }*/
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
            
            if (hotel_index != this->instance->get_Id_Hotel_depart() && hotel_index != arrivee){
                
                int index_best_poi = best_poi_between_two_hotels(depart, hotel_index, this->instance->get_POI_Duree_Max_Voyage(jour_index));
                
                
                if (index_best_poi != -1){
                    if (hotel_Intermedaire.size() < this->instance->get_Nombre_Jour() - 1){
                        hotel_Intermedaire.push_back(hotel_index);
                    }
                    
                    depart = hotel_index;

                                        
                }else{
                    //cout << "Find nearet neighbor" << endl;
                    int hotel_index = find_nearest_hotel_index(depart, jour_index);
                    if (hotel_index != -1){
                        
                        if (hotel_Intermedaire.size() < this->instance->get_Nombre_Jour() -1){
                            hotel_Intermedaire.push_back(hotel_index);
                            depart = hotel_index;
                        }
                    }else{
                        if (hotel_Intermedaire.size() < this->instance->get_Nombre_Jour() -1){
                            hotel_Intermedaire.push_back(depart);
                            break;
                        }
                    }
                }
                
            }
            
        }
        
    }
    

}

int SolutionInitiale::best_poi_between_two_hotels(int hotel_depart_index, int hotel_arrivee_index, float max_distance_jour)
{
    int best_poi_index = -1;
    float max_distance = -1.0f;
    float max_ratio = -1;
    
    for (int k = 0; k < this->instance->get_Nombre_POI(); ++k){

        float distance_to_poi_from_depart = this->instance->get_distance_Hotel_POI(hotel_depart_index, k);
        float distance_to_arrivee_from_poi = this->instance->get_distance_Hotel_POI(hotel_arrivee_index, k);

        if (distance_to_poi_from_depart < this->instance->get_POI_Heure_fermeture(k)){
            if (distance_to_poi_from_depart < this->instance->get_POI_Heure_ouverture(k)){
                distance_to_poi_from_depart = this->instance->get_POI_Heure_ouverture(k);
            }
            float total_distance = distance_to_poi_from_depart + distance_to_arrivee_from_poi;
            if (total_distance <= max_distance_jour){
                if (max_ratio <= this->instance->get_POI_Score(k)/distance_to_arrivee_from_poi){
                    max_ratio = this->instance->get_POI_Score(k)/distance_to_arrivee_from_poi;
                    best_poi_index = k;
                    
                }
            }
        }
        
        /*if (distance_to_poi_from_depart + distance_to_arrivee_from_poi <= max_distance_jour){

            if (distance_to_poi_from_depart < this->instance->get_POI_Heure_fermeture(k)){
                if (distance_to_poi_from_depart < this->instance->get_POI_Heure_ouverture(k)){
                    distance_to_poi_from_depart = this->instance->get_POI_Heure_ouverture(k);
                }
                if (distance_to_arrivee_from_poi <= max_distance){
                    if (max_ratio < this->instance->get_POI_Score(k)/distance_to_poi_from_depart){
                        max_ratio = this->instance->get_POI_Score(k)/distance_to_poi_from_depart;
                        max_distance = distance_to_arrivee_from_poi;
                        best_poi_index = k;
                    }
                    
                }
            }
 
        }*/
    }

    return best_poi_index;
}

//
void SolutionInitiale::solution_by_building_hotel_first()
{
    build_Intermediate_Hotel_List();

    cout << "# - # Size : - : " << hotel_Intermedaire.size() + 1 << " - " << this->instance->get_Nombre_Jour() << endl;

    cout << "@ - @ Hotel ";
    for(auto h : hotel_Intermedaire){
        cout << h << " --> ";
    }
    cout << endl;

    int current_hotel = this->instance->get_Id_Hotel_depart();

    v_Date_Depart.push_back(0.0);

    bool isMovable = true;

    int depart = this->hotel_depart;
    
    for (int j = 0; j < this->hotel_Intermedaire.size() + 1; ++j){
        
        float max_distance_jour = this->instance->get_POI_Duree_Max_Voyage(j);

        if (j < this->hotel_Intermedaire.size()){
            //cout << "Day :: " << j + 1 << endl;
            // Add this POI at the end of the day sequence POI
            int last_poi_before_hotel = best_poi_between_two_hotels(current_hotel,this->hotel_Intermedaire[j],this->instance->get_POI_Duree_Max_Voyage(j));
            //cout << "Last POI before hotel " << this->hotel_Intermedaire[j] << " is POI " << last_poi_before_hotel << endl;

            float distance_poi_to_last_hotel = this->instance->get_distance_Hotel_POI(this->hotel_Intermedaire[j], last_poi_before_hotel);
            //cout << "Distance from last POI to next hotel : " << distance_poi_to_last_hotel << endl;

            vector<int> poi_sequence_for_day = vector<int>();

            

            if (v_Date_Depart.size() < this->instance->get_Nombre_Jour()){
                v_Date_Depart.push_back(0.0f);
            }

            
            
            if (!poi_already_visited(last_poi_before_hotel) && distance_poi_to_last_hotel != -1){
                poi_sequence_for_day.push_back(last_poi_before_hotel);
                add_to_poi_visited(last_poi_before_hotel);
                sequence_Id_Poi_Par_Jour.push_back(poi_sequence_for_day);
            }else{
                //cout << "Which ?" << endl;
                
                for (int i = 0; i < this->instance->get_Nombre_POI(); ++i){
                    if (!poi_already_visited(i)){
                        if(this->instance->get_distance_Hotel_POI(current_hotel,i) + this->instance->get_distance_Hotel_POI(this->hotel_Intermedaire[j],i) <=  max_distance_jour &&
                            this->instance->get_distance_Hotel_POI(current_hotel,i) < this->instance->get_POI_Heure_fermeture(i)){
                            poi_sequence_for_day.push_back(i);
                            add_to_poi_visited(i);
                            sequence_Id_Poi_Par_Jour.push_back(poi_sequence_for_day);
                            break;
                        }
                    }
                }
                
            }
            
            
            total_distance_for_trip  = 0.0f;
            depart = this->hotel_Intermedaire[j];
            current_hotel = this->hotel_Intermedaire[j];
            //cout << "Current Hotel :: " << current_hotel << " - Hotel initial : " << this->instance->get_Id_Hotel_depart() << endl;

        }else{

            //cout << "Last day " << j << " - " << this->instance->get_Nombre_Jour() << endl;
            int last_poi_before_last_hotel = poi_between_hotel_and_last_hotel(current_hotel, this->instance->get_Id_Hotel_Arrivee(),this->instance->get_POI_Duree_Max_Voyage(j));
            cout << "Last POI before last # hotel " << current_hotel << " is POI " << last_poi_before_last_hotel << endl;

            vector<int> poi_sequence_for_last_day = vector<int>();
            

            if (last_poi_before_last_hotel != -1){
                cout << "Last Day :: " << j + 1 << endl;
            
                if (!poi_already_visited(last_poi_before_last_hotel)){
                    poi_sequence_for_last_day.push_back(last_poi_before_last_hotel);
                    sequence_Id_Poi_Par_Jour.push_back(poi_sequence_for_last_day);
                }else{
                    cout << "$$ Visited." << endl;

                    int find_index = -1;

                    for (int index = 0; index < this->instance->get_Nombre_POI(); ++index){
                        if(!poi_already_visited(index) && !hotel_Intermedaire.empty()){
                            float first_distance = this->instance->get_distance_Hotel_POI(hotel_Intermedaire[hotel_Intermedaire.size()-1],index);
                            float second_distance = this->instance->get_distance_Hotel_POI(this->instance->get_Id_Hotel_Arrivee(),index);

                            if (first_distance < this->instance->get_POI_Heure_fermeture(index) &&
                                first_distance + second_distance <= this->instance->get_POI_Duree_Max_Voyage(j)){

                                    

                                poi_sequence_for_last_day.push_back(index);
                                sequence_Id_Poi_Par_Jour.push_back(poi_sequence_for_last_day);

                                break;
                            }
                        }
                    }

                    
                    
                }
            }else{
                cout << "Find another POI for the last day !!!" << endl;
                cout << "Last Day :: " << j + 1 << endl;

                for (int i = 0; i < this->instance->get_Nombre_POI(); ++i){
                    if (!poi_already_visited(i)){
                        if(this->instance->get_distance_Hotel_POI(hotel_Intermedaire[hotel_Intermedaire.size()-1],i) + this->instance->get_distance_Hotel_POI(this->instance->get_Id_Hotel_Arrivee(),i) <=  max_distance_jour &&
                            this->instance->get_distance_Hotel_POI(hotel_Intermedaire[hotel_Intermedaire.size()-1],i) < this->instance->get_POI_Heure_fermeture(i)){
                            poi_sequence_for_last_day.push_back(i);
                            add_to_poi_visited(i);
                            sequence_Id_Poi_Par_Jour.push_back(poi_sequence_for_last_day);
                            break;
                        }
                    }
                }
                
                
            }
            
            
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

int SolutionInitiale::find_nearest_hotel_index(int index_current_hotel, int index_jour)
{
    float nearest_distance = numeric_limits<float>::max();
    int nearest_index = -1;

    if (index_current_hotel != this->instance->get_Id_Hotel_Arrivee()){
        for (int p = 0; p < this->instance->get_Nombre_POI(); ++p){
            float distance_hotel_poi = this->instance->get_distance_Hotel_POI(index_current_hotel,p);
            for (int h = 0; h < this->instance->get_Nombre_Hotel(); ++h){
                float distance_poi_hotel = this->instance->get_distance_Hotel_POI(h,p);
                if (distance_hotel_poi < this->instance->get_POI_Heure_fermeture(p)){
                    if (distance_hotel_poi < this->instance->get_POI_Heure_ouverture(p)){
                        distance_hotel_poi = this->instance->get_POI_Heure_ouverture(p);
                    }
                    float total_distance = distance_hotel_poi + distance_poi_hotel;
                    if (total_distance < this->instance->get_POI_Duree_Max_Voyage(index_jour)){
                        if (distance_poi_hotel < nearest_distance){
                            nearest_distance = distance_poi_hotel;
                            nearest_index = h;
                        }
                    }
                }
            }
        }
    }

    

    /*if (index_current_hotel != this->instance->get_Id_Hotel_Arrivee()){
        for (int h = 0; h < this->instance->get_Nombre_Hotel(); ++h){
            if (h != this->instance->get_Id_Hotel_Arrivee() && h != this->instance->get_Id_Hotel_depart()){
                if (this->instance->get_distance_Hotel_Hotel(index_current_hotel, h) < this->instance->get_POI_Duree_Max_Voyage(index_jour)){
                    if (this->instance->get_distance_Hotel_Hotel(index_current_hotel, h) > nearest_distance){
                        nearest_distance = this->instance->get_distance_Hotel_Hotel(index_current_hotel, h);
                        nearest_index = h;
                    }
                }
            }
            
        }
    }*/

    return nearest_index;
}

bool SolutionInitiale::hotel_is_in_itermadiate_hotel_list(int hotel_index)
{
    return find(hotel_Intermedaire.begin(),hotel_Intermedaire.end(),hotel_index) != hotel_Intermedaire.end();
}

vector<int> SolutionInitiale::get_not_visited_poi()
{
    vector<int> vect_poi = vector<int>();

    for (int p = 0; p < this->instance->get_Nombre_POI(); ++p ){
        if (!poi_already_visited(p)){
            vect_poi.push_back(p);
        }
    }

    return vect_poi;
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


