#define CHEMIN_DOSSIER_DONNEES "./Data/"
#define NOM_FICHIER_LISTE_FICHIER_DONNEES "data.txt"
#define NOM_FICHIER_LISTE_SORTIE "sortie.txt"

#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>
#include "Instance.hpp"
#include "Solution.hpp"
#include "SolutionInitiale.hpp"
#include "LocalSearchSolution.hpp"


using namespace std;

int Resolution(Instance * instance);
int heuristic_test(Instance* instance);


 

int main(int argc, const char * argv[])
{ 
    try
    {
        string s_tmp;
        string s_chemin=CHEMIN_DOSSIER_DONNEES;
        s_chemin.append(NOM_FICHIER_LISTE_FICHIER_DONNEES);
        
        ifstream fichier(s_chemin.c_str(), std::ios::in);std::ofstream fichier_Sortie_Resume;
        s_chemin=CHEMIN_DOSSIER_DONNEES;
        s_chemin.append(NOM_FICHIER_LISTE_SORTIE);
        ofstream fichier_Sortie(s_chemin.c_str(), std::ios::out | std::ios::app);

        if(fichier)
        {
            if(fichier_Sortie)
            {
                fichier_Sortie<<" Fichier données\t Tps de résolution \t Best solution"<<endl;
                getline(fichier,s_tmp);
                while(s_tmp!="")
                {
                    Instance * instance = new Instance();
                    chrono::time_point<chrono::system_clock> chrono_start, chrono_end;
                    chrono::duration<double> elapsed;
                    int i_best_solution_score=0;
                    s_chemin=CHEMIN_DOSSIER_DONNEES;
                    cout<< " Résolution de "<<s_tmp<<endl;
                    s_chemin.append(s_tmp);
                    s_chemin.erase(remove(s_chemin.begin(), s_chemin.end(), '\r'), s_chemin.end());
                    s_chemin.erase(remove(s_chemin.begin(), s_chemin.end(), '\n'), s_chemin.end());
                    
                    instance->chargement_Instance(s_chemin);
                    

                    /*chrono_start = chrono::system_clock::now();
                    i_best_solution_score=Resolution(instance);
                    cout<< " Fin de résolution de "<<s_tmp<<endl;
                    chrono_end = chrono::system_clock::now();
                    


                    elapsed=chrono_end-chrono_start;
                    fichier_Sortie<<s_chemin <<"\t"<<elapsed.count()<<"\t"<< i_best_solution_score <<endl;
                    s_tmp="";
                    getline(fichier,s_tmp);*/

                    

                    



                    cout << "----------------START-------------------" << endl;
                    chrono_start = chrono::system_clock::now();
                    i_best_solution_score=heuristic_test(instance);
                    //cout << "Bis Resolution Score : " << i_best_solution_score << endl;
                    //cout<< " Fin de résolution de "<<s_tmp << " Bis" <<endl;
                    chrono_end = chrono::system_clock::now();

                    elapsed=chrono_end-chrono_start;
                    fichier_Sortie<<s_chemin <<"\t"<<elapsed.count()<<"\t"<< i_best_solution_score <<endl;
                    s_tmp="";
                    getline(fichier,s_tmp);

                    cout << "----------------END-------------------" << endl;

                    


                    


                    delete instance;
                }
                fichier_Sortie.close();
            }
            else
            {
                cout<<" Erreur lecture des données : chemin vers la sortie non valide. "<<endl;
            }
            fichier.close();
        }
        else
        {
            cout<<" Erreur lecture des données : chemin listant l'ensemble des données non valide. "<<endl;
        }
    }
    
    catch(string err)
    {
        cout << "Erreur fatale : " <<endl;
        cout << err <<endl;
    }
    cout << "# - # End of resolution !" << endl;
    return 0;
}

int Resolution(Instance * instance)
{
    int i_val_Retour_Fct_obj=0;
    Solution * uneSolution = new Solution();
    vector<int> v_i_tmp ;

//INITIALISATION D'UN SOLUTION EN DUR POUR L'INSTANCE 1
    v_i_tmp.clear();
    uneSolution->v_Id_Hotel_Intermedaire.push_back(2);
    uneSolution->v_Date_Depart.push_back(0.0);
    uneSolution->v_Date_Depart.push_back(0.0);
    v_i_tmp ={0, 2, 5, 9, 14, 21, 28, 20, 27, 35, 42, 36, 29, 22, 30, 31};
    uneSolution->v_v_Sequence_Id_Par_Jour.push_back(v_i_tmp);
    v_i_tmp ={24, 32, 40, 33, 25, 19, 26, 34, 41, 47, 52, 56, 59, 61};
    uneSolution->v_v_Sequence_Id_Par_Jour.push_back(v_i_tmp);
    uneSolution->i_valeur_fonction_objectif=816;

    
    uneSolution->Verification_Solution(instance);
    
    i_val_Retour_Fct_obj=uneSolution->i_valeur_fonction_objectif;
    delete uneSolution;
    return i_val_Retour_Fct_obj;
}

//------------------------------------- TESTING FUNCTION ---------------------------------------------


// Heuristique test

int heuristic_test(Instance* instance){
    NearestNeighbor nearestNeighbor = NearestNeighbor(instance);

    nearestNeighbor.heuristic_nearest_neighbor();

    nearestNeighbor.add_unvisited_poi_to_the_solution();
    

    //nearestNeighbor.two_opt();
    nearestNeighbor.swap();

    nearestNeighbor.add_unvisited_poi_to_the_solution();

    int i_val_Retour_Fct_obj=0;
    Solution * uneSolution = new Solution();
    vector<int> v_i_tmp ;

    v_i_tmp.clear();

    for (int i = 0; i < nearestNeighbor.get_intermediate_hotel().size(); ++i){
        uneSolution->v_Id_Hotel_Intermedaire.push_back(nearestNeighbor.get_intermediate_hotel()[i]);
    }

    for (int i = 0; i < nearestNeighbor.get_date_depart().size(); ++i){
        uneSolution->v_Date_Depart.push_back(nearestNeighbor.get_date_depart()[i]);
    }

    cout << "Nearest Neighbor Sequence POI Test : " << endl;
    for (int i = 0; i < nearestNeighbor.get_sequence_poi_par_jour().size(); ++i){
        cout << i << " % ";
        v_i_tmp = vector<int>();
        for (int j = 0; j < nearestNeighbor.get_sequence_poi_par_jour()[i].size(); ++j){
            v_i_tmp.push_back(nearestNeighbor.get_sequence_poi_par_jour()[i][j]);
            cout << nearestNeighbor.get_sequence_poi_par_jour()[i][j] << " --> ";
        }
        uneSolution->v_v_Sequence_Id_Par_Jour.push_back(v_i_tmp);
        cout << " % ";
    }
    cout << endl;

    uneSolution->i_valeur_fonction_objectif = (int)nearestNeighbor.get_objective_function_value();

    bool b = uneSolution->Verification_Solution(instance);

    cout << "Verification de la solution : " << (b ? "OK" : "NOK") << endl;
    
    i_val_Retour_Fct_obj=uneSolution->i_valeur_fonction_objectif;

    
    
    

    delete uneSolution;

    return i_val_Retour_Fct_obj;
}





