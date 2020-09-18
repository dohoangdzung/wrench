/**
 * Copyright (c) 2017-2018. The WRENCH Team.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

/**
 ** This simulator simulates the execution of a chain workflow, that is, of a workflow
 ** in which each task has at most a single parent and at most a single child:
 **
 **   File #0 -> Task #0 -> File #1 -> Task #1 -> File #2 -> .... -> Task #n-1 -> File #n
 **
 ** The compute platform comprises two hosts, WMSHost and ComputeHost. On WMSHost runs a simple storage
 ** service and a WMS (defined in class OneTaskAtATimeWMS). On ComputeHost runs a bare metal
 ** compute service, that has access to the 10 cores of that host. Once the simulation is done,
 ** the completion time of each workflow task is printed.
 **
 ** Example invocation of the simulator for a 10-task workflow, with no logging:
 **    ./wrench-example-bare-metal-chain 10 ./two_hosts.xml
 **
 ** Example invocation of the simulator for a 10-task workflow, with only WMS logging:
 **    ./wrench-example-bare-metal-chain 10 ./two_hosts.xml --log=custom_wms.threshold=info
 **
 ** Example invocation of the simulator for a 5-task workflow with full logging:
 **    ./wrench-example-bare-metal-chain 5 ./two_hosts.xml --wrench-full-log
 **/


#include <iostream>
#include <wrench.h>
#include <wrench/services/memory/MemoryManager.h>
#include "OneTaskAtATimeWMS.h" // WMS implementation

/**
 * @brief The Simulator's main function
 *
 * @param argc: argument count
 * @param argv: argument array
 * @return 0 on success, non-zero otherwise
 */

wrench::Workflow *workflow_single(int num_tasks, long file_size_gb, long mem_req_gb, double cpu_time_sec) {

    wrench::Workflow *workflow = new wrench::Workflow();

    /* Add workflow tasks */
    for (int i = 0; i < num_tasks; i++) {
        auto task = workflow->addTask("task_" + std::to_string(i), cpu_time_sec * 1000000000, 1, 1, 0.90,
                                      mem_req_gb * 1000000000);
    }

    /* Add workflow files */
    for (int i = 0; i < num_tasks + 1; i++) {
        workflow->addFile("file_" + std::to_string(i), file_size_gb * 1000000000);
    }

    /* Set input/output files for each task */
    for (int i = 0; i < num_tasks; i++) {
        auto task = workflow->getTaskByID("task_" + std::to_string(i));
        task->addInputFile(workflow->getFileByID("file_" + std::to_string(i)));
        task->addOutputFile(workflow->getFileByID("file_" + std::to_string(i + 1)));
    }

    return workflow;
}

wrench::Workflow *workflow_multithread(int num_pipes, int num_tasks, int core_per_task,
                                       long flops, long file_size, long mem_required) {

    wrench::Workflow *workflow = new wrench::Workflow();

    for (int i = 0; i < num_pipes; i++) {

        /* Add workflow tasks */
        for (int j = 0; j < num_tasks; j++) {
            /* Create a task: 1GFlop, single core */
            auto task = workflow->addTask("task_" + std::to_string(i) + "_" + std::to_string(j),
                                          flops, 1, core_per_task, 1, mem_required);
        }

        /* Add workflow files */
        for (int j = 0; j < num_tasks + 1; j++) {
            workflow->addFile("file_" + std::to_string(i) + "_" + std::to_string(j), file_size);
        }

        /* Create tasks and set input/output files for each task */
        for (int j = 0; j < num_tasks; j++) {

            auto task = workflow->getTaskByID("task_" + std::to_string(i) + "_" + std::to_string(j));

            task->addInputFile(workflow->getFileByID("file_" + std::to_string(i) + "_" + std::to_string(j)));
            task->addOutputFile(workflow->getFileByID("file_" + std::to_string(i) + "_" + std::to_string(j + 1)));
        }
    }

    return workflow;
}

void export_output_single(wrench::SimulationOutput output, int num_tasks, std::string filename) {
    auto read_start = output.getTrace<wrench::SimulationTimestampFileReadStart>();
    auto read_end = output.getTrace<wrench::SimulationTimestampFileReadCompletion>();
    auto write_start = output.getTrace<wrench::SimulationTimestampFileWriteStart>();
    auto write_end = output.getTrace<wrench::SimulationTimestampFileWriteCompletion>();
    auto task_start = output.getTrace<wrench::SimulationTimestampTaskStart>();
    auto task_end = output.getTrace<wrench::SimulationTimestampTaskCompletion>();

    FILE *log_file = fopen(filename.c_str(), "w");
    fprintf(log_file, "type, start, end\n");

    for (int i = 0; i < num_tasks; i++) {
//        std::cerr << "Task " << read_end[i]->getContent()->getTask()->getID()
//                  << " read completed in " << read_end[i]->getDate() - read_start[i]->getDate()
//                  << std::endl;
//        std::cerr << "Task " << read_end[i]->getContent()->getTask()->getID()
//                  << " write completed in " << write_end[i]->getDate() - write_start[i]->getDate()
//                  << std::endl;
        std::cerr << "Task " << read_end[i]->getContent()->getTask()->getID()
                  << " completed at " << task_end[i]->getDate()
                  << " in " << task_end[i]->getDate() - task_start[i]->getDate()
                  << std::endl;


        fprintf(log_file, "read, %lf, %lf\n", read_start[i]->getDate(), read_end[i]->getDate());
        fprintf(log_file, "write, %lf, %lf\n", write_start[i]->getDate(), write_end[i]->getDate());
    }

    fclose(log_file);
}

void export_output_multi(wrench::SimulationOutput output, int num_tasks, std::string filename) {
    auto read_start = output.getTrace<wrench::SimulationTimestampFileReadStart>();
    auto read_end = output.getTrace<wrench::SimulationTimestampFileReadCompletion>();
    auto write_start = output.getTrace<wrench::SimulationTimestampFileWriteStart>();
    auto write_end = output.getTrace<wrench::SimulationTimestampFileWriteCompletion>();
    auto task_start = output.getTrace<wrench::SimulationTimestampTaskStart>();
    auto task_end = output.getTrace<wrench::SimulationTimestampTaskCompletion>();

    FILE *log_file = fopen(filename.c_str(), "w");
    fprintf(log_file, "read_start, read_end, cpu_start, cpu_end, write_start, write_end\n");

    for (int i = 0; i < num_tasks; i++) {
        std::cerr << read_end[i]->getContent()->getTask()->getID()
                  << " started at " << task_start[i]->getDate()
                  << ", ended at " << task_end[i]->getDate()
                  << ", completed in " << task_end[i]->getDate() - task_start[i]->getDate()
                  << std::endl;

        fprintf(log_file, "%lf, %lf, %lf, %lf, %lf, %lf\n", read_start[i]->getDate(), read_end[i]->getDate(),
                read_end[i]->getDate(), write_start[i]->getDate(),
                write_start[i]->getDate(), write_end[i]->getDate());
    }

    fclose(log_file);
}

int main(int argc, char **argv) {

    long file_size_gb = 20;
    long mem_req_gb = 20;
    double cpu_time_sec = 28;

    wrench::Simulation simulation;
    simulation.init(&argc, argv);

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <number of tasks> <xml platform file> [--log=custom_wms.threshold=info]"
                  << std::endl;
        exit(1);
    }

    std::cerr << "Instantiating simulated platform..." << std::endl;
    simulation.instantiatePlatform(argv[2]);


    int no_pipelines = 0;
    try {
        no_pipelines = std::atoi(argv[1]);
    } catch (std::invalid_argument &e) {
        std::cerr << "Invalid number of pipelines\n";
        exit(1);
    }

    /* Declare a workflow */
    wrench::Workflow *workflow = workflow_single(3, file_size_gb, mem_req_gb, cpu_time_sec);

//    wrench::Workflow *workflow = workflow_multithread(no_pipelines, 3, 1, cpu_time_sec * 10000000000.0,
//                                                      file_size_gb * 1000000000, mem_req_gb * 1000000000);

    std::cerr << "Instantiating a SimpleStorageService on host01..." << std::endl;
    auto storage_service = simulation.add(new wrench::SimpleStorageService(
            "host01", {"/"}, {{wrench::SimpleStorageServiceProperty::BUFFER_SIZE, "50000000"}}, {}));

    std::cerr << "Instantiating a BareMetalComputeService on ComputeHost..." << std::endl;
    auto baremetal_service = simulation.add(new wrench::BareMetalComputeService(
            "host01", {"host01"}, "", {}, {}));

    auto wms = simulation.add(
            new wrench::OneTaskAtATimeWMS({baremetal_service}, {storage_service}, "host01"));

    wms->addWorkflow(workflow);

    std::cerr << "Instantiating a FileRegistryService on host01 ..." << std::endl;
    auto file_registry_service = new wrench::FileRegistryService("host01");
    simulation.add(file_registry_service);

    std::cerr << "Staging task input files..." << std::endl;
    for (auto const &f : workflow->getInputFiles()) {
        simulation.stageFile(f, storage_service);
    }

    /* Launch the simulation. This call only returns when the simulation is complete. */
    std::cerr << "Launching the Simulation..." << std::endl;
    try {
        simulation.launch();
    } catch (std::runtime_error &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    std::cerr << "Simulation done!" << std::endl;

    /* Simulation results can be examined via simulation.output, which provides access to traces
     * of events. In the code below, we print the  retrieve the trace of all task completion events, print how
     * many such events there are, and print some information for the first such event. */
    auto trace = simulation.getOutput().getTrace<wrench::SimulationTimestampTaskCompletion>();
    for (auto const &item : trace) {
        std::cerr << "Task " << item->getContent()->getTask()->getID() << " completed at time " << item->getDate()
                  << std::endl;
    }

    export_output_single(simulation.getOutput(), 3, "2nd/" + to_string(file_size_gb) + "gb_sim_time.csv");
    simulation.getMemoryManagerByHost("host01")->export_log("2nd/" + to_string(file_size_gb) + "gb_sim_mem.csv");

//    simulation.getOutput().dumpUnifiedJSON(workflow, "multi/original/dump_" + to_string(no_pipelines) + ".json");
//    export_output_multi(simulation.getOutput(), workflow->getNumberOfTasks(),
//            "timestamp_multi_sim_.csv");

    return 0;
}
