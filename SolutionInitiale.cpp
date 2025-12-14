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
    float distance_to_poi = distance_to_next_poi_or_hotel(current_position, poi_index, is_hotel ? HOTEL : POI);
    float arrival_time = current_distance + distance_to_poi;

    return arrival_time >= this->instance->get_POI_Heure_ouverture(poi_index) && 
           arrival_time <= this->instance->get_POI_Heure_fermeture(poi_index);
}

// Curent distance is the total distance traveled per day moins the distance spent
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
                sequence_jour.push_back(poi_best_index);

                add_to_poi_visited(poi_best_index);

                total_distance += distance_to_next_poi_or_hotel(depart, poi_best_index, is_hotel ? HOTEL : POI);

                if (total_distance < max_distance_jour){
                    i_valeur_fonction_objectif += this->instance->get_POI_Score(poi_best_index);
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
                    sequence_Id_Poi_Par_Jour.push_back(sequence_jour);
                    isMovable = false;
                    
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
            cout << "Next Hotel Depart = " << depart << endl;
            add_to_intermadiate_hotel(depart);
            if (depart != this->instance->get_Id_Hotel_depart()){
                v_Date_Depart.push_back(0.0f);
            }
            
            is_hotel = true;
            isMovable = true;
        }

        cout << "Jour " << jour_index << " completed. POIs visited so far : " << poi_visited.size() << endl;


    }

}

vector<int> SolutionInitiale::stock_and_get(int id_jour)
{

    
    int length_poi = this->instance->get_Nombre_POI();

    vector<int> sequence_jour = vector<int>();

    float distance_jour = this->instance->get_POI_Duree_Max_Voyage(id_jour);

    float total_distance = 0.0f;

    bool isMovable = true;

    v_Date_Depart.push_back((float)total_distance);

    

    cout << "Depart : " << hotel_depart << " Durée Jour : " << distance_jour << endl;


    vector<float> list_distance = distance_Hotel_and_all_Poi(hotel_depart, length_poi, id_jour);

    for (int i = 0; i < list_distance.size(); ++i){
        cout << "Hotel - POI " << list_distance[i] << " - " << i << endl;
    }

    int max_index = max_Index(list_distance);

    cout << "Max POI Index : " << max_index << endl;

    sequence_jour.push_back(max_index);

    add_to_poi_visited(max_index);

    total_distance += this->instance->get_distance_Hotel_POI(hotel_depart,max_index);

    cout << "Current distance = " <<total_distance << endl;

    cout << "POI visited size = " << poi_visited.size() << endl;

    for (int i = 0; i < sequence_jour.size();++i){
        cout << "Sq : " << sequence_jour[i] << " - " << i << endl;
    }
    cout << endl;


    int current_poi_index = max_index;

    int nb_iteration = 0;

    while (isMovable){
        int poi_best_index = get_Best_Index_From_Poi_Poi_Distance(current_poi_index,total_distance,id_jour);
        nb_iteration++;
        if ((nb_iteration)>6 ? true : false){
            isMovable = false;
        }
        //cout << "Visit : " << " Dist = " << total_distance + this->instance->get_distance_POI_POI(current_poi_index,poi_best_index) << endl;
        
        if (!poi_already_visited(poi_best_index)){
            sequence_jour.push_back(poi_best_index);
            add_to_poi_visited(poi_best_index);
            total_distance += this->instance->get_distance_POI_POI(current_poi_index,poi_best_index);
            current_poi_index = poi_best_index;
        }
        
    }

    cout << "MiniFinal Sq : ";
    for (int i = 0; i < sequence_jour.size();++i){
        cout << sequence_jour[i] << " --> ";
    }
    cout << endl;

    cout << "Current Poi = " << current_poi_index << endl;
    cout << "Final $ Total distance = " << total_distance << " - " << distance_jour << endl;

    hotel_depart = find_best_intermediate_hotel_from_poi(current_poi_index,total_distance, distance_jour);

    cout << "Next Hotel Depart = " << hotel_depart << endl;

    add_to_intermadiate_hotel(hotel_depart);

    
    
   
    

    return sequence_jour;
}

vector<float> SolutionInitiale::distance_Hotel_and_all_Poi(int index_hotel, int length_poi, int id_jour)
{

    vector<float> vect_distance = vector<float>();

    
    if (index_hotel <= this->instance->get_Nombre_Hotel() && length_poi <= this->instance->get_Nombre_POI()){
        for(int index = 0; index < length_poi; ++index){
            float hp = this->instance->get_distance_Hotel_POI(index_hotel, index);
            
            if (hp < this->instance->get_POI_Duree_Max_Voyage(id_jour)){
                if ((hp >= this->instance->get_POI_Heure_ouverture(index) && hp < this->instance->get_POI_Heure_fermeture(index)) || 
                (hp < this->instance->get_POI_Heure_ouverture(index))){
                    vect_distance.push_back(this->instance->get_POI_Score(index)/hp);  
                }
            }
            //vect_distance.push_back(this->instance->get_POI_Score(index)/hp);    
        }
    }
    return vect_distance;
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

int SolutionInitiale::get_Best_Index_From_Poi_Poi_Distance(int index_curent_Poi, float distance, int id_jour)
{
    vector<float> list_distance = vector<float>();

    int index = 0;
    while (index < this->instance->get_Nombre_POI()){
        if(index != index_curent_Poi){
            float pp = this->instance->get_distance_POI_POI(index_curent_Poi,index);
            if ( (distance + pp) < this->instance->get_POI_Duree_Max_Voyage(id_jour) ){
                if ( (pp >= this->instance->get_POI_Heure_ouverture(index) && pp < this->instance->get_POI_Heure_fermeture(index)) || 
                (pp < this->instance->get_POI_Heure_ouverture(index)) ){
                    list_distance.push_back(this->instance->get_POI_Score(index)/pp);  
                }
            }
            
        }
        index++;
    }
    

    return max_Index(list_distance);
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

bool SolutionInitiale::belongs_to_prohibited_poi(int index_poi)
{ 
    return find(prohibited_poi.begin(),prohibited_poi.end(),index_poi) != prohibited_poi.end();
}

void SolutionInitiale::show_poi_visited()
{
    cout << "POI Visited : ";
        for (int i = 0; i < poi_visited.size(); ++i){
            cout << poi_visited[i] << " - ";
        }
    cout << endl;   
}

void SolutionInitiale::fill_objective_function_value()
{
    int total_score = 0;

    for (int i = 0; i < sequence_Id_Poi_Par_Jour.size(); ++i){
        for (int j = 0; j < sequence_Id_Poi_Par_Jour[i].size(); ++j){
            total_score += this->instance->get_POI_Score(sequence_Id_Poi_Par_Jour[i][j]);
        }
    }

    this->i_valeur_fonction_objectif = total_score;
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

void SolutionInitiale::load_sequence()
{
    for (int i = 0; i < this->instance->get_Nombre_Jour(); i++){
        this->sequence_Id_Poi_Par_Jour.push_back(stock_and_get(i));
    }
}
