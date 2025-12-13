#include "SolutionInitiale.hpp"


SolutionInitiale::SolutionInitiale(Instance *instance)
{
    this->instance = instance;

    
    vector<float> stock = stock_and_get(0,0);
    
}

vector<float> SolutionInitiale::stock_and_get(int poi_index, int id_jour)
{
    int hotel_depart = this->instance->get_Id_Hotel_depart();
    int length_poi = this->instance->get_Nombre_POI();
    vector<int> sequence_jour = vector<int>();
    float distance_jour = this->instance->get_POI_Duree_Max_Voyage(id_jour);

    //hotel_Intermedaire.push_back(hotel_depart);

    vector<float> stock = vector<float>();
    
    float total_distance = 0;


    vector<float> list_distance = distance_Hotel_and_all_Poi(hotel_depart, length_poi);

    for (auto d : list_distance){
        cout << "H-P " << d << endl;
    }

    vector<int> list_index_condition = find_Poi_Index_By_Condition(list_distance, this->instance->get_POI_Duree_Max_Voyage(id_jour));

    for (auto d : list_index_condition){
        cout << "C-P " << d << endl;
    }

    int index_max = max_Poi_index(list_distance, length_poi, list_index_condition);

    cout << "IndexMax : " << index_max << endl;

    cout << "Max " << this->instance->get_POI_Score(index_max) << endl;

    hotel_Intermedaire.push_back(hotel_depart);
    v_Date_Depart.push_back(0);
    sequence_jour.push_back(index_max);
    total_distance = this->instance->get_distance_Hotel_POI(hotel_depart,index_max);

    cout << "Total distance = " << total_distance << endl;
    cout << "Total Jour = " << distance_jour << endl;

    int curent_Poi_Index = index_max;
    bool isDone = false;

    for (int index = 0; index < length_poi; ++index){
        if (find(sequence_jour.begin(),sequence_jour.end(),index) == sequence_jour.end()){
            while(!isDone){
                int pos = get_Best_Index_From_Poi_Poi_Distance(curent_Poi_Index);
                if ((pos != -1) && (total_distance + this->instance->get_distance_POI_POI(curent_Poi_Index,index) < distance_jour)){
                    curent_Poi_Index = pos;
                    sequence_jour.push_back(pos);
                    v_Date_Depart.push_back(this->instance->get_POI_Heure_fermeture(pos));
                    total_distance += this->instance->get_distance_POI_POI(curent_Poi_Index,index);
                }else{
                    isDone = true;
                }
            }
        }
    }
    //Todo : Find hotel intermediaire
    //Return sequence_jour

    cout << "Sequence Jour : ";
    for (auto c : sequence_jour){
        cout << c << "-->";
    }
    cout << endl;
    cout << "Final distance = " << total_distance << endl;

    return vector<float>();
}

vector<float> SolutionInitiale::distance_Hotel_and_all_Poi(int index_hotel, int length_poi)
{
    vector<float> distance = vector<float>();

    
    if (index_hotel <= this->instance->get_Nombre_Hotel() && length_poi <= this->instance->get_Nombre_POI()){
        for(int index = 0; index < length_poi; ++index){
            float hp = this->instance->get_distance_Hotel_POI(index_hotel, index);
            distance.push_back(hp);
        }
    }
    return distance;
}

vector<int> SolutionInitiale::find_Poi_Index_By_Condition(vector<float> distance, float condition)
{
    vector<int> indexes = vector<int>();

    for (int i = 0; i < distance.size(); ++i){
        if (distance[i] < condition){
            indexes.push_back(distance[i]);
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
    int max_index = -1;

    for (int i = 0; i < distance.size(); ++i){
        if (max_value <= distance[i]){
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
    int min_index = -1;

    for (int i = 0; i < distance.size(); ++i){
        if (min_value >= distance[i]){
            cout << "Min "<< min_value << " - " << distance[i] << endl;
            min_value = distance[i];
            min_index = i;
        }
    }
    

    return min_index;
}
