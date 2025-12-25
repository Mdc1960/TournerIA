#include "SolutionInitiale.hpp"


SolutionInitiale::SolutionInitiale(Instance *instance)
{
    this->instance = instance;

    hotel_depart = this->instance->get_Id_Hotel_depart();

}

// Works
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

// Works
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
            if (total_distance < max_distance_jour){
                if (max_ratio < this->instance->get_POI_Score(k)/distance_to_arrivee_from_poi){
                    max_ratio = this->instance->get_POI_Score(k)/distance_to_arrivee_from_poi;
                    best_poi_index = k;
                    
                }
            }
        }
        
        
    }

    return best_poi_index;
}

//
void SolutionInitiale::solution_by_building_hotel_first()
{
    build_Intermediate_Hotel_List();

    int current_hotel = this->instance->get_Id_Hotel_depart();

    v_Date_Depart.push_back(0.0);

    bool isMovable = true;

    //int depart = this->hotel_depart;
    
    for (int j = 0; j < this->hotel_Intermedaire.size() + 1; ++j){
        
        float max_distance_jour = this->instance->get_POI_Duree_Max_Voyage(j);

        if (j < this->hotel_Intermedaire.size()){
            
            int last_poi_before_hotel = best_poi_between_two_hotels(current_hotel,this->hotel_Intermedaire[j],this->instance->get_POI_Duree_Max_Voyage(j));
            //cout << "Last POI before hotel " << this->hotel_Intermedaire[j] << " is POI " << last_poi_before_hotel << endl;

            float distance_poi_to_last_hotel = this->instance->get_distance_Hotel_POI(this->hotel_Intermedaire[j], last_poi_before_hotel);
            //cout << "Distance from last POI to next hotel : " << distance_poi_to_last_hotel << endl;

            vector<int> poi_sequence_for_day = vector<int>();

            

            if (v_Date_Depart.size() < this->instance->get_Nombre_Jour()){
                v_Date_Depart.push_back(0.0f);
            }

            
            
            if (!poi_already_visited(last_poi_before_hotel) && last_poi_before_hotel != -1){
                poi_sequence_for_day.push_back(last_poi_before_hotel);
                add_to_poi_visited(last_poi_before_hotel);
                sequence_Id_Poi_Par_Jour.push_back(poi_sequence_for_day);
            }else{
                
                for (int i = 0; i < this->instance->get_Nombre_POI(); ++i){
                    if (!poi_already_visited(i)){
                        if(this->instance->get_distance_Hotel_POI(current_hotel,i) + this->instance->get_distance_Hotel_POI(this->hotel_Intermedaire[j],i) <=  max_distance_jour &&
                            this->instance->get_distance_Hotel_POI(current_hotel,i) <= this->instance->get_POI_Heure_fermeture(i)){
                            poi_sequence_for_day.push_back(i);
                            add_to_poi_visited(i);
                            sequence_Id_Poi_Par_Jour.push_back(poi_sequence_for_day);
                            break;
                        }
                    }
                }
                
            }
            
            
            total_distance_for_trip  = 0.0f;
            current_hotel = this->hotel_Intermedaire[j];

        }else{

            int last_poi_before_last_hotel = poi_between_hotel_and_last_hotel(current_hotel, this->instance->get_Id_Hotel_Arrivee(),this->instance->get_POI_Duree_Max_Voyage(j));
            cout << "Last POI before last # hotel " << current_hotel << " is POI " << last_poi_before_last_hotel << endl;

            vector<int> poi_sequence_for_last_day = vector<int>();

            bool best_poi_is_found = false;
            

            if (last_poi_before_last_hotel != -1 && !poi_already_visited(last_poi_before_last_hotel)){
                cout << "Last Day :: " << j + 1 << endl;

                float distance_hotel_last_poi = this->instance->get_distance_Hotel_POI(current_hotel, last_poi_before_last_hotel);
                float distance_last_poi_hotel = this->instance->get_distance_Hotel_POI(this->instance->get_Id_Hotel_Arrivee(), last_poi_before_last_hotel);
                
                
                if (distance_hotel_last_poi < this->instance->get_POI_Heure_ouverture(last_poi_before_last_hotel)){
                    distance_hotel_last_poi = this->instance->get_POI_Heure_ouverture(last_poi_before_last_hotel);
                }
                
                if (distance_hotel_last_poi <= this->instance->get_POI_Heure_fermeture(last_poi_before_last_hotel) && 
                    distance_hotel_last_poi + distance_last_poi_hotel <= this->instance->get_POI_Duree_Max_Voyage(j)){
                    cout << "@ - @ POI is HERE" << endl;
                    if (!poi_already_visited(last_poi_before_last_hotel)){
                        best_poi_is_found = true;
                    
                    }

                }
            }
            
            if (best_poi_is_found){
                cout << "@ - @ You are in best POI" << endl;
                poi_sequence_for_last_day.push_back(last_poi_before_last_hotel);
                add_to_poi_visited(last_poi_before_last_hotel);
                sequence_Id_Poi_Par_Jour.push_back(poi_sequence_for_last_day);
            }else{
                cout << "Find another POI for the last day !!!" << endl;
                cout << "Last Day :: " << j + 1 << endl;

                for (int i = 0; i < this->instance->get_Nombre_POI(); ++i){
                    if (!poi_already_visited(i)){
                        if(this->instance->get_distance_Hotel_POI(hotel_Intermedaire[hotel_Intermedaire.size()-1],i) + this->instance->get_distance_Hotel_POI(this->instance->get_Id_Hotel_Arrivee(),i) <=  max_distance_jour &&
                            this->instance->get_distance_Hotel_POI(hotel_Intermedaire[hotel_Intermedaire.size()-1],i) <= this->instance->get_POI_Heure_fermeture(i)){
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

// Works
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



// Works
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

    

    

    return nearest_index;
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

// Works
void SolutionInitiale::add_to_poi_visited(int index_poi)
{
    if (find(poi_visited.begin(),poi_visited.end(),index_poi) == poi_visited.end()){
        this->poi_visited.push_back(index_poi);
    }
}

// Works
bool SolutionInitiale::poi_already_visited(int index_poi)
{
    return find(poi_visited.begin(),poi_visited.end(),index_poi) != poi_visited.end();
}

// Works
void SolutionInitiale::add_to_intermadiate_hotel(int index)
{
    if (this->hotel_Intermedaire.size() >= this->instance->get_Nombre_Jour() -1){
        return;
    }
    if (find(hotel_Intermedaire.begin(),hotel_Intermedaire.end(),index) == hotel_Intermedaire.end()){
        this->hotel_Intermedaire.push_back(index);
    }
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


