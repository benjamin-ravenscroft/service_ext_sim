#include <iostream>
#include <cmath>
#include <random>
#include <map>
#include <iomanip>
#include <string>
#include <chrono>

#include "arrow/io/file.h"
#include "parquet/stream_writer.h"

#include "Simulation.h"
#include "Patient.h"
#include "Waitlist.h"
#include "DischargeList.h"
#include "Server.h"
#include "Reader_Writer.h"

std::random_device rd;
std::mt19937 rng(rd());

Simulation::Simulation(int n_ep, std::vector<int> clin, double lam, double ext_p, int pol,
    std::vector<int> serv_path, std::vector<double> serv_prob, std::string path) {
        n_epochs = n_ep;
        clinicians = clin;
        arr_lam = lam;
        ext_prob = ext_p;
        ext_pol = pol;
        pathways = serv_path;
        probs = serv_prob;
        n_classes = serv_path.size();
        class_dstb = std::discrete_distribution<> (serv_prob.begin(), serv_prob.end());

        dl = DischargeList(path);
        // dl = DischargeList();
        wl = Waitlist(n_classes, rng);
}

void Simulation::generate_servers() {
    for (int i = 0; i < clinicians.size(); i++) {
        for (int j = 0; j < clinicians[i]; j++) {
            servers.push_back(Server(wl, dl, ext_pol));
        }
    }
}

void Simulation::generate_arrivals(int epoch) {
    std::poisson_distribution<> arr_dstb(arr_lam);
    int n_patients = arr_dstb(rng);
    n_admitted += n_patients;
    // std::cout << "Adding " << n_patients << " patients to waitlist." << std::endl;
    for (int i = 0; i < n_patients; i++) {
        int pat_class = class_dstb(rng); // get int pat class
        Patient pat = Patient(epoch, pat_class, pathways[pat_class], ext_prob, rng);
        wl.add_patient(pat, epoch);
        // std::cout << "Waitlist length after adding patient: " << wl.len_waitlist() << std::endl;
    }
}

void Simulation::run() {
    auto start = std::chrono::high_resolution_clock::now();
    for (int epoch = 0; epoch < n_epochs; epoch++) {
        generate_arrivals(epoch);
        for (int i = 0; i < servers.size(); i++) {
            servers[i].process_epoch(epoch);
        }
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
    std::cout << "Simulation runtime (seconds): " << duration.count() << std::endl;
}

int Simulation::get_n_admitted(){return n_admitted;}

void Simulation::write_statistics(std::string path){}

void Simulation::write_parquet(std::string path) {
    std::shared_ptr<arrow::io::FileOutputStream> outfile;

    std::cout << "Making outfile" << std::endl;

    PARQUET_ASSIGN_OR_THROW(
        outfile,
        arrow::io::FileOutputStream::Open(path));

    std::cout << "Setting up schema" << std::endl;

    std::shared_ptr<parquet::schema::GroupNode> schema = SetupSchema();

    std::cout << "Schema setup" << std::endl;
    
    parquet::WriterProperties::Builder builder;
    builder.compression(parquet::Compression::GZIP);

    std::cout << "Initialized builder" << std::endl;

    parquet::StreamWriter os{
        parquet::ParquetFileWriter::Open(outfile, schema, builder.build())};

    std::cout << "Setup output stream" << std::endl;

    for (auto & patient : dl.discharge_list) {
        os << (patient.get_pathway()) << (patient.get_base_duration()) << (patient.get_arrival_t()) << (patient.get_first_appt())
            << (patient.get_n_appts()) << (patient.get_discharge_time()) << (patient.get_n_ext())
            << (patient.get_sojourn_time()) << (patient.get_total_wait_time()) << parquet::EndRow;
    }
}

int main(){
    std::string extension_protocols[3] = {"Standard", "Reduced Frequency", "Waitlist"};
    int n_epochs = 25000;
    std::vector<int> clinicians = {(800)};
    double arr_lam = 36;
    double ext_prob = 0.05;
    std::vector<int> serv_path = {14, 21, 28};
    std::vector<double> serv_prob = {40, 40, 20};

    int runs = 1;

    for (int ext_pol = 0; ext_pol < sizeof(extension_protocols)/sizeof(std::string); ext_pol++) {
        std::cout << "Running " << extension_protocols[ext_pol] << " policy simulation:" << std::endl;
        std::string path = "/mnt/c/Users/benja/OneDrive - University of Waterloo/KidsAbility Research/Extending Patient Treatment/C++ Simulation Results/l_36_800_s_1_r/" + extension_protocols[ext_pol] + "/";
        for (int run = 0; run < runs; run++){
            std::cout << "Run " << run << std::endl;
            std::string run_path =  path + ("simulation_data_" + std::to_string(run) + ".parquet");
            Simulation sim = Simulation(n_epochs, clinicians, arr_lam, ext_prob, ext_pol, serv_path, serv_prob, run_path);
            sim.generate_servers();
            sim.run();
            std::cout << "N admitted: " << sim.get_n_admitted() << " N discharged: " << sim.dl.get_n_patients() << " N on waitlist: " << sim.wl.len_waitlist() << std::endl;
            // output results to a parquet file
            // sim.write_parquet(run_path);
        }
    }

    // Patient pat = Patient(0, 0, 14, 0.05, rng);
    // Waitlist wl = Waitlist(1, rng);
    // DischargeList dl = DischargeList();
    // Server serv = Server(wl, dl, "std");
    // wl.add_patient(pat, 1);
    // serv.add_from_waitlist(5);
    // serv.print_patients();
};