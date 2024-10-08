#include "Patient.h"
#include <iostream>

Patient::Patient(int arrival_time, int pathway, int base_duration, double service_red_beta, int service_red_cap,
                    double ext_prob_cap, double base_ext_p, double wait_ext_beta, double queue_ext_beta, std::mt19937 &gen){
    Patient::set_arrival_time(arrival_time);
    Patient::set_pathway(pathway);
    Patient::set_base_duration(base_duration);
    Patient::set_service_duration(base_duration);
    Patient::set_serv_red_beta(service_red_beta);
    Patient::set_serv_red_cap(service_red_cap);
    Patient::set_ext_prob_cap(ext_prob_cap);
    Patient::set_base_ext_p(base_ext_p);
    Patient::set_wait_ext_beta(wait_ext_beta);
    Patient::set_queue_ext_beta(queue_ext_beta);
    Patient::set_extended(0);
    rng = gen;
}

void Patient::add_appt(int epoch){
    appt_epochs.push_back(epoch);
}

void Patient::add_wait(int wlist_arr_t, int add_t){
    total_wait_time += (add_t - wlist_arr_t);
}

std::array<bool, 2> Patient::process_patient(int epoch, int wl_len){
    Patient::add_appt(epoch);
    return Patient::check_complete(wl_len);
}

double Patient::calculate_ext_prob(){
    int delta_appts = service_duration - appt_epochs.size();
    double p = std::min(ext_prob_cap, base_ext_p + (wait_ext_beta*total_wait_time) + (queue_ext_beta*delta_appts));
    // std::cout << "Extension prob: " << p << std::endl;
    return p;
}

std::array<bool, 2> Patient::check_complete(int wl_len){
    int min_reduction = std::min(serv_red_cap, (int)std::round(serv_red_beta*wl_len));
    double temp_duration = service_duration - min_reduction;   // calculate service reductions
    if (appt_epochs.size() >= temp_duration){
        double ext_p = Patient::calculate_ext_prob();
        if ((dis(rng) < ext_p) && (extended < ext_cap)){
            service_duration += base_duration;
            extended += 1;
            return {true, true};
        } else {
            set_discharge_duration(temp_duration);
            return {true, false};
        }
    } else {
        return {false, false};
    }
}

// member variable setters
void Patient::set_arrival_time(int t){arrival_time=t;}
void Patient::set_pathway(int p){pathway=p;}
void Patient::set_base_duration(int d){base_duration=d;}
void Patient::set_service_duration(int d){service_duration=d;}
void Patient::set_serv_red_beta(double b){serv_red_beta=b;}
void Patient::set_serv_red_cap(int c){serv_red_cap=c;}
void Patient::set_ext_prob_cap(double p){ext_prob_cap=p;}
void Patient::set_base_ext_p(double p){base_ext_p=p;}
void Patient::set_wait_ext_beta(double b){wait_ext_beta=b;}
void Patient::set_queue_ext_beta(double b){queue_ext_beta=b;}
void Patient::set_discharge_time(int epoch){discharge_time = epoch;}
void Patient::set_extended(int c){extended=c;}
void Patient::set_discharge_duration(int d){discharge_duration=d;}

// extraneous get-set methods
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

int Patient::get_discharge_duration(){return discharge_duration;}