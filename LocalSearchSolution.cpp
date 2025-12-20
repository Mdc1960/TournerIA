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

    for (int id_jour = 0; id_jour < this->instance->get_Nombre_Jour(); ++id_jour){
        if (id_jour < this->hotel_Intermedaire.size()){

            float distance_max_jour = this->instance->get_POI_Duree_Max_Voyage(id_jour);
            bool movable = true;
            
            int current_position = this->instance->get_Id_Hotel_depart();
            int last_position = hotel_Intermedaire[id_jour];
            
            bool hotel = is_hotel;

            vector<int> saved_sequence_day = sequence_Id_Poi_Par_Jour[id_jour];

            if (saved_sequence_day.empty()) continue;

            float last_distance_poi_hotel = this->instance->get_distance_Hotel_POI(last_position,saved_sequence_day[saved_sequence_day.size()-1]);

            
            this->total_distance_for_trip = 0.0f;
            vector<int> sequence_jour = vector<int>();

            while(movable){
                cout << "Current total distance : " << total_distance_for_trip << " / " << distance_max_jour << endl;

                if (hotel){
                    int index_best_poi = best_poi_from_hotel(current_position,total_distance_for_trip, id_jour);
                    if (!this->poi_already_visited(index_best_poi) && index_best_poi != -1 &&
                        find(sequence_jour.begin(),sequence_jour.end(),index_best_poi) == sequence_jour.end()){

                        
                        float distance_hote_poi = this->instance->get_distance_Hotel_POI(current_position, index_best_poi);
                        if (distance_hote_poi < this->instance->get_POI_Heure_fermeture(index_best_poi)){
                            if (distance_hote_poi < this->instance->get_POI_Heure_ouverture(index_best_poi)){
                                distance_hote_poi = this->instance->get_POI_Heure_ouverture(index_best_poi);
                            }
                            this->total_distance_for_trip += distance_hote_poi;

                            if (this->total_distance_for_trip <= distance_max_jour){

                                sequence_jour.push_back(index_best_poi);
                                add_to_poi_visited(index_best_poi);

                                hotel = false;
                                current_position = index_best_poi;

                                

                            }
                        }

                    }
                }else{

                }
                

            }
        }

        
    }
    


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

                if (corrected_distance < distance_max_by_day){
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

    float best_ratio = -1.0f;
    int best_poi_index = -1;

    float distance_max_by_day = this->instance->get_POI_Duree_Max_Voyage(id_jour);

    for (int p = 0; p < this->instance->get_Nombre_POI(); ++p){

        if (!poi_already_visited(p) && p != poi_index){
            float distance_poi_poi = this->instance->get_distance_POI_POI(p,poi_index);
            if (distance_poi_poi < this->instance->get_POI_Heure_fermeture(p)){

                if (distance_poi_poi < this->instance->get_POI_Heure_ouverture(p)){
                    distance_poi_poi = this->instance->get_POI_Heure_ouverture(p);
                }

                float corrected_distance = distance_poi_poi + total_distance;

                if (corrected_distance < distance_max_by_day){
                    if (best_ratio < this->instance->get_POI_Score(p)/(corrected_distance)){
                        best_ratio = this->instance->get_POI_Score(p)/(corrected_distance);
                        best_poi_index = p;
                    }
                }

            }
        }

    }

    return 0;
}
