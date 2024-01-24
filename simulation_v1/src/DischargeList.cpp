#include "DischargeList.h"

#include <vector>
#include <iostream>
#include "Patient.h"

#include "arrow/io/file.h"
#include "parquet/stream_writer.h" 
#include "Reader_Writer.h"

DischargeList::DischargeList(){
    discharge_list = std::vector<Patient>();
}

DischargeList::DischargeList(std::string p){
    discharge_list = std::vector<Patient>();
    path = p;

    std::shared_ptr<arrow::io::FileOutputStream> outfile;

    // std::cout << "Making outfile" << std::endl;

    PARQUET_ASSIGN_OR_THROW(
        outfile,
        arrow::io::FileOutputStream::Open(path));

    // std::cout << "Setting up schema" << std::endl;
    std::shared_ptr<parquet::schema::GroupNode> schema = SetupSchema();
    // std::cout << "Schema setup" << std::endl;
    
    parquet::WriterProperties::Builder builder;
    builder.compression(parquet::Compression::GZIP);
    // std::cout << "Initialized builder" << std::endl;

    os = parquet::StreamWriter(parquet::ParquetFileWriter::Open(outfile, schema, builder.build()));
    // std::cout << "Started output stream" << std::endl;
}

void DischargeList::add_patient(Patient patient){
    n_patients += 1;
    // discharge_list.push_back(patient);
    os << (patient.get_pathway()) << (patient.get_base_duration()) << (patient.get_arrival_t()) << (patient.get_first_appt())
        << (patient.get_n_appts()) << (patient.get_discharge_time()) << (patient.get_n_ext())
        << (patient.get_sojourn_time()) << (patient.get_total_wait_time()) << parquet::EndRow;
}

int DischargeList::get_n_patients(){return n_patients;}

int DischargeList::size(){
    return discharge_list.size();
}