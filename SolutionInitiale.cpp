#include "SolutionInitiale.hpp"


SolutionInitiale::SolutionInitiale(Instance *instance)
{
    this->instance = instance;

    hotel_depart = this->instance->get_Id_Hotel_depart();

    

    /*for (int i = 0; i < this->instance->get_Nombre_Jour(); ++i){
        stock.push_back(stock_and_get(0,i));
        sequence_Id_Poi_Par_Jour.push_back(stock_and_get(0,i));
    }

    
    
    cout << "Size = " << stock.size() << endl;

    cout << "Final sequence : ";
    for (int i = 0; i < stock.size(); ++i){
        cout << i << " % ";
        for (int j = 0; j < stock[i].size(); ++j){
            cout << stock[i][j] << "-->";
        }
        cout << " % ";
    }
    cout << endl;
    cout << "NB Jours ::: " << this->instance->get_Nombre_Jour() << endl;*/
}

vector<int> SolutionInitiale::stock_and_get(int poi_index, int id_jour)
{

    int length_poi = this->instance->get_Nombre_POI();
    vector<int> sequence_jour = vector<int>();
    float distance_jour = this->instance->get_POI_Duree_Max_Voyage(id_jour);
    float total_distance = 0;
    bool isMovable = true;

    //hotel_Intermedaire.push_back(hotel_depart);

    cout << "Depart : " << hotel_depart << " Durée Jour : " << distance_jour << endl;

    vector<float> stock = vector<float>();
    
    


    vector<float> list_distance = distance_Hotel_and_all_Poi(hotel_depart, length_poi);

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


    int poi_best_index = get_Best_Index_From_Poi_Poi_Distance(max_index);

    cout << "Best poi index = " << poi_best_index << endl;

    int current_poi_index = max_index;

    int nb_iteration = 0;

    while (isMovable){
        int poi_best_index = get_Best_Index_From_Poi_Poi_Distance(current_poi_index);
        nb_iteration++;
        if ((nb_iteration)>1000000 ? true : false){
            isMovable = false;
        }
        cout << "Visit : " << " Dist = " << total_distance + this->instance->get_distance_POI_POI(current_poi_index,poi_best_index) << endl;
        if (total_distance + this->instance->get_distance_POI_POI(current_poi_index,poi_best_index) < distance_jour){
            
            
            if (!poi_already_visited(poi_best_index)){
                sequence_jour.push_back(poi_best_index);
                add_to_poi_visited(poi_best_index);
                total_distance += this->instance->get_distance_POI_POI(current_poi_index,poi_best_index);
                current_poi_index = poi_best_index;
            }
            cout << "Totale distance = " << total_distance << " - " << " Jour distance = " << distance_jour << endl;
            
        }
        
    }

    cout << "MiniFinal Sq : ";
    for (int i = 0; i < sequence_jour.size();++i){
        cout << sequence_jour[i] << " --> ";
    }
    cout << endl;

    cout << "Current Poi = " << current_poi_index << endl;

    hotel_depart = 1;
    
    
   
    

    return sequence_jour;
}

vector<float> SolutionInitiale::distance_Hotel_and_all_Poi(int index_hotel, int length_poi)
{
    vector<float> distance = vector<float>();

    
    if (index_hotel <= this->instance->get_Nombre_Hotel() && length_poi <= this->instance->get_Nombre_POI()){
        for(int index = 0; index < length_poi; ++index){
            float hp = this->instance->get_distance_Hotel_POI(index_hotel, index);
            distance.push_back(this->instance->get_POI_Score(index)/hp);
        }
    }
    return distance;
}

vector<int> SolutionInitiale::find_Poi_Index_By_Condition(vector<float> distance, float condition)
{
    vector<int> indexes = vector<int>();

    for (int i = 0; i < distance.size(); ++i){
        if (distance[i] < condition){
            indexes.push_back(i);
        }
    }

    return indexes;
}

int SolutionInitiale::max_Poi_index(vector<float> distance, int length_poi, vector<int> listIndex)
{
    
    vector<float> poids_score = vector<float>();
    
    if (length_poi <= this->instance->get_Nombre_POI()){
        for (int i = 0; i < listIndex.size(); ++i){
            if (i <= this->instance->get_Nombre_POI() && i <= distance.size()){
                poids_score.push_back(this->instance->get_POI_Score(i)/distance[i]);
            }
        }
    }
    return max_Index(poids_score);
}

int SolutionInitiale::max_Index(vector<float> distance)
{
     
    if (distance.empty()) return -1;

    float max_value = distance[0];
    int max_index = 0;

    for (int i = 1; i < distance.size(); ++i){
        if (max_value < distance[i]){
            cout << "Max "<< max_value << " - " << distance[i] << endl;
            max_value = distance[i];
            max_index = i;
        }
    }
    

    return max_index;
}

int SolutionInitiale::get_Best_Index_From_Poi_Poi_Distance(int index_curent_Poi)
{
    
    vector<float> list_distance = vector<float>();

    for (int index = 0; index < this->instance->get_Nombre_POI(); ++index){
        if(index != index_curent_Poi){
            list_distance.push_back(this->instance->get_POI_Score(index)/this->instance->get_distance_POI_POI(index_curent_Poi,index));
        }
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
            cout << "Min "<< min_value << " - " << distance[i] << endl;
            min_value = distance[i];
            min_index = i;
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

int SolutionInitiale::find_best_intermediate_hotel_from_poi(int index_poi)
{
    
    vector<float> list_distance = vector<float>();

    for (int index = 0; index < this->instance->get_Nombre_Hotel(); ++index){
        if (index != this->hotel_depart){
            list_distance.push_back(this->instance->get_distance_Hotel_POI(index_poi,index));
        }
        
    }

    return find_Min_Index(list_distance);
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
        this->sequence_Id_Poi_Par_Jour.push_back(stock_and_get(0,i));
    }
}
