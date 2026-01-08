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
#include "Util.hpp"


using namespace std;

int Resolution(Instance * instance);


int main(int argc, const char * argv[])
{ 

    Util::configureResolution();

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
                    

                    chrono_start = chrono::system_clock::now();
                    i_best_solution_score=Resolution(instance);
                    cout<< " Fin de résolution de "<<s_tmp<<endl;
                    chrono_end = chrono::system_clock::now();
                    


                    elapsed=chrono_end-chrono_start;
                    fichier_Sortie<<s_chemin <<"\t"<<elapsed.count()<<"\t"<< i_best_solution_score <<endl;
                    s_tmp="";
                    getline(fichier,s_tmp);

                    

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
    return 0;
}

int Resolution(Instance * instance)
{
    int i_val_Retour_Fct_obj=0;
    Solution * uneSolution = new Solution();
    vector<int> v_i_tmp ;


    // INITIALISATION OF THE SOLUTION
    
    NearestNeighbor nearestNeighbor = NearestNeighbor(instance);

    nearestNeighbor.GRASP();
    
    
    

    v_i_tmp.clear();

    for (int i = 0; i < nearestNeighbor.get_intermediate_hotel().size(); ++i){
        uneSolution->v_Id_Hotel_Intermedaire.push_back(nearestNeighbor.get_intermediate_hotel()[i]);
    }

    for (int i = 0; i < nearestNeighbor.get_date_depart().size(); ++i){
        uneSolution->v_Date_Depart.push_back(nearestNeighbor.get_date_depart()[i]);
    }

    cout << "----------------START-------------------" << endl;

    cout << "Nearest Neighbor Sequence POI : " << endl;
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

    cout << "----------------END-------------------" << endl;
    
    i_val_Retour_Fct_obj=uneSolution->i_valeur_fonction_objectif;

    
    
    

    delete uneSolution;

    return i_val_Retour_Fct_obj;



}