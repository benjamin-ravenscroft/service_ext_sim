#ifndef WAITLIST_H
#define WAITLIST_H

#include <iostream>
#include <vector>
#include <deque>
#include "Patient.h"

class Waitlist{
    public:
        std::vector<int> classes;
        std::vector<std::deque<std::pair<Patient, int>>> waitlist;
        std::deque<Patient> reassignment_list;
        std::mt19937 rng;

        Waitlist(); // def constr
        Waitlist(int n_classes, std::mt19937 &gen);

        int len_waitlist();
        bool check_waitlist();
        void add_patient(Patient &patient, int epoch);
        int len_reassignments();
        void add_reassignment(Patient patient);
        std::pair<Patient, int> get_patient();

};
#endif