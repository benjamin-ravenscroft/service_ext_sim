#include "Patient.h"
#include <iostream>

Patient::Patient(int arr_t, int path, int duration, double ext_prob, std::mt19937 &gen){
    arrival_time = arr_t;
    pathway = path;
    base_duration = duration;
    service_duration = duration;
    extend_prob = ext_prob;
    rng = gen;
}

void Patient::add_appt(int epoch){
    appt_epochs.push_back(epoch);
}

void Patient::add_wait(int wlist_arr_t, int add_t){
    total_wait_time += (add_t - wlist_arr_t);
}

std::array<bool, 2> Patient::process_patient(int epoch){
    Patient::add_appt(epoch);
    return Patient::check_complete();
}

std::array<bool, 2> Patient::check_complete(){
    if (appt_epochs.size() >= service_duration){
        if ((dis(rng) < extend_prob) && (extended < ext_cap)){
            service_duration += base_duration;
            extended += 1;
            return {true, true};
        } else {
            return {true, false};
        }
    } else {
        return {false, false};
    }
}

int Patient::get_pathway(){return pathway;}

int Patient::get_base_duration(){return base_duration;}

int Patient::get_arrival_t(){return arrival_time;}

int Patient::get_first_appt(){return appt_epochs[0];}

int Patient::get_n_appts(){return appt_epochs.size();}

int Patient::get_n_ext(){return extended;}

int Patient::get_discharge_time(){return discharge_time;}

int Patient::get_sojourn_time(){return discharge_time - arrival_time;}

int Patient::get_total_wait_time(){
    return total_wait_time;
}

void Patient::set_discharge_time(int epoch){discharge_time = epoch;}