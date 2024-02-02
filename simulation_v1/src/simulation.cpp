#include <iostream>
#include <cmath>
#include <random>
#include <map>
#include <iomanip>
#include <string>
#include <chrono>
#include <charconv>
#include <stdexcept>

#include "arrow/io/file.h"
#include "parquet/stream_writer.h"

#include "Simulation.h"
#include "Patient.h"
#include "Waitlist.h"
#include "DischargeList.h"
#include "Server.h"
#include "Reader_Writer.h"
#include "WriteCSV.h"

std::random_device rd;
std::mt19937 rng(rd());

Simulation::Simulation(int n_epochs, std::vector<int> clinicians, double arr_lam, double service_red_beta, int service_red_cap,
                        double ext_prob_cap, double ext_prob, double wait_ext_beta, double queue_ext_beta, int ext_pol, 
                        std::vector<int> pathways, std::vector<double> probs, std::string path, std::string wl_path) {

        Simulation::set_n_epochs(n_epochs);
        Simulation::set_clinicians(clinicians);
        Simulation::set_arr_lam(arr_lam);
        Simulation::set_service_red_beta(service_red_beta);
        Simulation::set_service_red_cap(service_red_cap);
        Simulation::set_ext_prob_cap(ext_prob_cap);
        Simulation::set_ext_prob(ext_prob);
        Simulation::set_wait_ext_beta(wait_ext_beta);
        Simulation::set_queue_ext_beta(queue_ext_beta);
        Simulation::set_ext_pol(ext_pol);
        Simulation::set_pathways(pathways);
        Simulation::set_probs(probs);
        Simulation::set_n_classes(pathways.size());
        Simulation::set_class_dstb(std::discrete_distribution<> (probs.begin(), probs.end()));
        Simulation::set_discharge_list(path);
        Simulation::set_waitlist(n_classes, rng);

        // setup output stream for waitlist statistics
        std::shared_ptr<arrow::io::FileOutputStream> outfile;
        PARQUET_ASSIGN_OR_THROW(
            outfile,
            arrow::io::FileOutputStream::Open(wl_path)
        );
        std::shared_ptr<parquet::schema::GroupNode> schema = SetupSchema_Waitlist();
        parquet::WriterProperties::Builder builder;
        builder.compression(parquet::Compression::GZIP);
        wl_os = parquet::StreamWriter(parquet::ParquetFileWriter::Open(outfile, schema, builder.build()));
}

void Simulation::set_n_epochs(int n){n_epochs = n;}
void Simulation::set_clinicians(std::vector<int> c){clinicians = c;}
void Simulation::set_arr_lam(double l){arr_lam = l;}
void Simulation::set_service_red_beta(double b){service_red_beta = b;}
void Simulation::set_service_red_cap(int c){service_red_cap = c;}
void Simulation::set_ext_prob_cap(double p){ext_prob_cap = p;}
void Simulation::set_ext_prob(double p){ext_prob = p;}
void Simulation::set_wait_ext_beta(double b){wait_ext_beta = b;}
void Simulation::set_queue_ext_beta(double b){queue_ext_beta = b;}
void Simulation::set_ext_pol(int p){ext_pol = p;}
void Simulation::set_pathways(std::vector<int> ps){pathways = ps;}
void Simulation::set_probs(std::vector<double> ps){probs = ps;}
void Simulation::set_n_classes(int n){n_classes = n;}
void Simulation::set_class_dstb(std::discrete_distribution<> dstb){class_dstb = dstb;}
void Simulation::set_discharge_list(std::string p){dl = DischargeList(p);}
void Simulation::set_waitlist(int n, std::mt19937 &gen){wl = Waitlist(n, gen);}

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
        Patient pat = Patient(epoch, pat_class, pathways[pat_class], service_red_beta, service_red_cap,
                                ext_prob_cap, ext_prob, wait_ext_beta, queue_ext_beta, rng);
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
        stream_waitlist(epoch);
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
    std::cout << "Simulation runtime (seconds): " << duration.count() << std::endl;
}

int Simulation::get_n_admitted(){return n_admitted;}

int Simulation::get_n_discharged(){return dl.get_n_patients();}

int Simulation::get_n_waitlist(){return wl.len_waitlist();}

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

    for (auto & patient : dl.get_discharge_list()) {
        os << (patient.get_pathway()) << (patient.get_base_duration()) << (patient.get_arrival_t()) << (patient.get_first_appt())
            << (patient.get_n_appts()) << (patient.get_discharge_time()) << (patient.get_n_ext())
            << (patient.get_sojourn_time()) << (patient.get_total_wait_time()) << parquet::EndRow;
    }
}

void Simulation::stream_waitlist(int epoch){
    wl_os << epoch << wl.len_waitlist() << parquet::EndRow;
}

int main(int argc, char *argv[]){
    // std::string extension_protocols[3] = {"Standard", "Reduced Frequency", "Waitlist"};
    std::string extension_protocols[1] = {"Standard"};
    int n_epochs = 10000;
    std::vector<int> clinicians = {(40)};
    double arr_lam = 1.65;
    double service_red_beta = 1;
    int service_red_cap = 6;
    double ext_prob_cap = 0.2;
    double ext_prob = 0.05;
    double wait_ext_beta = 0.001;
    double queue_ext_beta = 0.000025;
    std::vector<int> serv_path = {21};
    std::vector<double> serv_prob = {100};
    int runs = 1;

    std::string folder = "/mnt/c/Users/benja/OneDrive - University of Waterloo/KidsAbility Research/Extending Patient Treatment/C++ Simulation_V1 Results/";

    // parse command-line args if passed
    // std::cout << "argc" << argc << std::endl;
    // std::cout << "argv" << argv[0] << std::endl;
    if (argc > 1) {
        if (argc == 11) {
            std::cout << "Processing with CLI arguments." << std::endl;
            n_epochs=std::stoi(argv[1]);
            arr_lam=std::stod(argv[2]);
            service_red_beta=std::atof(argv[3]);
            std::cout << "Service reduction beta: " << service_red_beta;
            service_red_cap=std::atof(argv[4]);
            std::cout << "Service reduction cap: " << service_red_cap;
            ext_prob_cap=std::atof(argv[5]);
            ext_prob=std::atof(argv[6]);
            wait_ext_beta=std::atof(argv[7]);
            queue_ext_beta=std::atof(argv[8]);
            runs=std::atof(argv[9]);
            // folder += ("l_" + std::string(argv[2]) + "_srb_" + std::string(argv[3] + 2) + "_epc_" + std::string(argv[4] + 2) + 
            //             "_ep_" + std::string(argv[5] + 2) + "_web_" + std::string(argv[6] + 2) + "_qeb_" + std::string(argv[7] + 2) + "/");
            folder = argv[10];
        } else {
            throw std::invalid_argument("Parameter Arguments Invalid Size Error: Attempted to pass parameter arguments, but too few included. n_args: " + std::to_string(argc));
        }
    } else {
        folder += "test/";
    }

    std::vector<std::pair<std::string, double>> parameters{std::pair("n_epochs", n_epochs), std::pair("servers", clinicians[0]),
                                                            std::pair("ext_prob_cap", ext_prob_cap), std::pair("ext_prob", ext_prob),
                                                            std::pair("service_red_beta", service_red_beta), std::pair("service_red_cap", service_red_cap),
                                                            std::pair("wait_ext_beta", wait_ext_beta), std::pair("queue_ext_beta", queue_ext_beta),
                                                            std::pair("runs", runs), std::pair("arr_lam", arr_lam)};
    write_csv(folder+"parameters.csv", parameters);

    for (int ext_pol = 0; ext_pol < sizeof(extension_protocols)/sizeof(std::string); ext_pol++) {
        std::cout << "Running " << extension_protocols[ext_pol] << " policy simulation:" << std::endl;
        std::string path = folder + extension_protocols[ext_pol] + "/";
        std::string wl_path = folder + extension_protocols[ext_pol] + "/waitlist_data/";
        for (int run = 0; run < runs; run++){
            std::cout << "Run " << run << std::endl;
            std::string run_path =  path + ("simulation_data_" + std::to_string(run) + ".parquet");
            std::string waitlist_path = wl_path + ("waitlist_data_" + std::to_string(run) + ".parquet");
            Simulation sim = Simulation(n_epochs, clinicians, arr_lam, service_red_beta, service_red_cap, ext_prob_cap, ext_prob, 
                                        wait_ext_beta, queue_ext_beta, ext_pol, serv_path, serv_prob, run_path, waitlist_path);
            sim.generate_servers();
            sim.run();
            std::cout << "N admitted: " << sim.get_n_admitted() << " N discharged: " << sim.get_n_discharged() << " N on waitlist: " << sim.get_n_waitlist() << std::endl;
            // output results to a parquet file
            // sim.write_parquet(run_path);
        }
    }
};