/**
 * Copyright (c) 2017. The WRENCH Team.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef WRENCH_WORKFLOWFILE_H
#define WRENCH_WORKFLOWFILE_H

#include <string>
#include <map>


namespace wrench {

    class Workflow;

    class WorkflowTask;

    /**
     * @brief A data file used i workflow
     */
    class WorkflowFile {

    public:

        double getSize();

        std::string getId();


        /***********************/
        /** \cond DEVELOPER    */
        /***********************/

        bool isOutput();

        Workflow *getWorkflow();

        /***********************/
        /** \endcond           */
        /***********************/

    private:

        friend class Workflow;

        friend class WorkflowTask;

        std::string id;
        double size; // in bytes

        void setOutputOf(WorkflowTask *task);

        WorkflowTask *getOutputOf();

        void setInputOf(WorkflowTask *task);

        std::map<std::string, WorkflowTask *> getInputOf();

        Workflow *workflow; // Containing workflow
        WorkflowFile(const std::string, double);

        WorkflowTask *output_of;
        std::map<std::string, WorkflowTask *> input_of;

    };

};

#endif //WRENCH_WORKFLOWFILE_H