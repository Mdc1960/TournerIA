#ifndef CHECKER_H
#define CHECKER_H

#include "Instance.hpp"
#include <stdio.h>
#include <vector>
#include <iostream>
#include <set>

using namespace std;

class Checker
{
    public:
        vector<int> v_Id_Hotel_Intermedaire;            
        vector<vector<int>> v_v_Sequence_Id_Par_Jour;   
        vector<float> v_Date_Depart;                    
        int i_valeur_fonction_objectif;                 
    
    public:
    /* Constructeurs et destructeur  */
    Checker();
    virtual ~Checker();
    bool Verification_Solution(Instance *instance);
};

#endif