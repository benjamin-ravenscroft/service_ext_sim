#include "Waitlist.h"

#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include "Patient.h"

Waitlist::Waitlist(){}

Waitlist::Waitlist(int n_classes, std::mt19937 &gen){
    rng = gen;
    for (int i = 0; i < n_classes; i++){
        classes.push_back(i);
        waitlist.push_back(std::deque<std::pair<Patient, int>>());
    }
}

int Waitlist::len_waitlist(){
    int len = 0;
    for (int i = 0; i < waitlist.size(); i++){
        len += waitlist[i].size();
    }
    return len;
}

bool Waitlist::check_waitlist(){
    for (int i = 0; i < waitlist.size(); i++){
        if (waitlist[i].size() > 0){
            return true;
        };
    }
    return false;
}

void Waitlist::add_patient(Patient &patient, int epoch){
    waitlist[patient.get_pathway()].push_back((std::pair<Patient, int>) {patient, epoch});
}

int Waitlist::len_reassignments(){
    return reassignment_list.size();
}

void Waitlist::add_reassignment(Patient patient){
    reassignment_list.push_back(patient);
}

std::pair<Patient, int> Waitlist::get_patient(){
    std::shuffle(classes.begin(), classes.end(), rng);
    for (auto & i : classes){
        if (waitlist[i].size() > 0){
            std::pair<Patient, int> pair = waitlist[i].front();
            waitlist[i].pop_front();
            return pair;
        }
    }
}