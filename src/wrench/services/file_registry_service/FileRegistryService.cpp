/**
 * Copyright (c) 2017. The WRENCH Team.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include <wrench/services/compute/MulticoreComputeService.h>
#include <wrench/logging/TerminalOutput.h>
#include <wrench/simgrid_S4U_util/S4U_Simulation.h>
#include <simgrid_S4U_util/S4U_Mailbox.h>
#include <simulation/SimulationMessage.h>
#include <services/ServiceMessage.h>

#include "wrench/services/file_registry/FileRegistryService.h"
#include "FileRegistryMessage.h"
#include <wrench/services/storage/StorageService.h>
#include <wrench/workflow/WorkflowFile.h>
#include <wrench/exceptions/WorkflowExecutionException.h>

XBT_LOG_NEW_DEFAULT_CATEGORY(file_registry_service, "Log category for File Registry Service");

namespace wrench {


    /**
     * @brief Constructor
     * @param hostname: the hostname on which to start the service
     * @param plist: a property list ({} means "use all defaults")
     */
    FileRegistryService::FileRegistryService(std::string hostname,
                                             std::map<std::string, std::string> plist) :
            FileRegistryService(hostname, plist, "") {

    }

    /**
     * @brief Constructor
     * @param hostname: the hostname on which to start the service
     * @param plist: a property list ({} means "use all defaults")
     * @param suffix: suffix to append to the service name and mailbox
     */
    FileRegistryService::FileRegistryService(
            std::string hostname,
            std::map<std::string, std::string> plist,
            std::string suffix) :
            Service("file_registry_service" + suffix, "file_registry_service" + suffix) {

      // Set default properties
      for (auto p : this->default_property_values) {
        this->setProperty(p.first, p.second);
      }

      // Set specified properties
      for (auto p : plist) {
        this->setProperty(p.first, p.second);
      }

      // Start the daemon on the same host
      try {
        this->start(hostname);
      } catch (std::invalid_argument e) {
        throw e;
      }
    }


    /**
     * @brief Lookup an entry for a file
     * @param file: the file to lookup
     * @return The storage services that hold a copy of the file
     *
     * @throw WorkflowExecutionException
     * @throw std::invalid_argument
     * @throw std::runtime_error
     */
    std::set<StorageService *> FileRegistryService::lookupEntry(WorkflowFile *file) {

      WRENCH_INFO("IN lookupEntry()");

      if (file == nullptr) {
        throw std::invalid_argument("FileRegistryService::lookupEntry(): Invalid argument");
      }

      std::string answer_mailbox = S4U_Mailbox::generateUniqueMailboxName("lookup_entry");

      try {
        S4U_Mailbox::putMessage(this->mailbox_name, new FileRegistryFileLookupRequestMessage(answer_mailbox, file,
                                                                                             this->getPropertyValueAsDouble(
                                                                                                     FileRegistryServiceProperty::FILE_LOOKUP_REQUEST_MESSAGE_PAYLOAD)));
      } catch (std::shared_ptr<NetworkError> cause) {
          throw WorkflowExecutionException(cause);
      }

      std::unique_ptr<SimulationMessage> message = nullptr;

      try {
        message = S4U_Mailbox::getMessage(answer_mailbox);
      } catch (std::shared_ptr<NetworkError> cause) {
        throw WorkflowExecutionException(cause);
      }

      if (FileRegistryFileLookupAnswerMessage *msg = dynamic_cast<FileRegistryFileLookupAnswerMessage *>(message.get())) {
        return msg->locations;
      } else {
        throw std::runtime_error("FileRegistryService::lookupEntry(): Unexpected [" + message->getName() + "] message");
      }
    }


    /**
     * @brief Add an entry
     * @param file: a file
     * @param storage_service: a storage_service
     *
     * @throw WorkflowExecutionException
     * @throw std::invalid_argument
     * @throw std::runtime_error
     */
    void FileRegistryService::addEntry(WorkflowFile *file, StorageService *storage_service) {

      if ((file == nullptr) || (storage_service == nullptr)) {
        throw std::invalid_argument("FileRegistryService::addEntry(): Invalid  argument");
      }

      std::string answer_mailbox = S4U_Mailbox::generateUniqueMailboxName("add_entry");

      try {
        S4U_Mailbox::putMessage(this->mailbox_name,
                                new FileRegistryAddEntryRequestMessage(answer_mailbox, file, storage_service,
                                                                       this->getPropertyValueAsDouble(
                                                                               FileRegistryServiceProperty::ADD_ENTRY_REQUEST_MESSAGE_PAYLOAD)));
      } catch (std::shared_ptr<NetworkError> cause) {
          throw WorkflowExecutionException(cause);
      }

      std::unique_ptr<SimulationMessage> message = nullptr;

      try {
        message = S4U_Mailbox::getMessage(answer_mailbox);
      } catch (std::shared_ptr<NetworkError> cause) {
        throw WorkflowExecutionException(cause);
      }

      if (FileRegistryAddEntryAnswerMessage *msg = dynamic_cast<FileRegistryAddEntryAnswerMessage *>(message.get())) {
        return;
      } else {
        std::runtime_error("Unexpected [" + message->getName() + "] message");
      }
    }

    /**
     * @brief Remove an entry
     * @param file: a file
     * @param storage_service: a storage service
     *
     * @throw WorkflowExecutionException
     * @throw std::invalid_argument
     * @throw std::runtime_error
     */
    void FileRegistryService::removeEntry(WorkflowFile *file, StorageService *storage_service) {

      if ((file == nullptr) || (storage_service == nullptr)) {
        throw std::invalid_argument(" FileRegistryService::removeEntry(): Invalid input argument");
      }
      std::string answer_mailbox = S4U_Mailbox::generateUniqueMailboxName("remove_entry");

      try {
        S4U_Mailbox::putMessage(this->mailbox_name,
                                new FileRegistryRemoveEntryRequestMessage(answer_mailbox, file, storage_service,
                                                                          this->getPropertyValueAsDouble(
                                                                                  FileRegistryServiceProperty::REMOVE_ENTRY_REQUEST_MESSAGE_PAYLOAD)));
      } catch (std::shared_ptr<NetworkError> cause) {
        throw WorkflowExecutionException(cause);
      }

      std::unique_ptr<SimulationMessage> message = nullptr;

      try {
        message = S4U_Mailbox::getMessage(answer_mailbox);
      } catch (std::shared_ptr<NetworkError> cause) {
        throw WorkflowExecutionException(cause);
      }

      if (FileRegistryRemoveEntryAnswerMessage *msg = dynamic_cast<FileRegistryRemoveEntryAnswerMessage *>(message.get())) {
        if (!msg->success) {
          WRENCH_WARN("Attempted to remove non-existent (%s,%s) entry from file registry service",
                      file->getId().c_str(), storage_service->getName().c_str());
        }
        return;
      } else {
        std::runtime_error("Unexpected [" + message->getName() + "] message");
      }
    }


    /**
     * @brief Main method of the daemon
     *
     * @return 0 on termination
     */
    int FileRegistryService::main() {

      TerminalOutput::setThisProcessLoggingColor(WRENCH_LOGGING_COLOR_MAGENTA);

      WRENCH_INFO("File Registry Service starting on host %s!", S4U_Simulation::getHostName().c_str());

      /** Main loop **/
      while (this->processNextMessage()) {

      }

      WRENCH_INFO("File Registry Service on host %s terminated!", S4U_Simulation::getHostName().c_str());
      return 0;
    }


    /**
     * @brief Helper function to process incoming messages
     * @return false if the daemon should terminate after processing this message
     */
    bool FileRegistryService::processNextMessage() {

      // Wait for a message
      std::unique_ptr<SimulationMessage> message = nullptr;

      try {
        message = S4U_Mailbox::getMessage(this->mailbox_name);
      } catch (std::shared_ptr<NetworkError> cause) {
        return true;
      }

      if (message == nullptr) {
        WRENCH_INFO("Got a NULL message... Likely this means we're all done. Aborting!");
        return false;
      }

      WRENCH_INFO("Got a [%s] message", message->getName().c_str());

      if (ServiceStopDaemonMessage *msg = dynamic_cast<ServiceStopDaemonMessage *>(message.get())) {
        // This is Synchronous
        try {
          S4U_Mailbox::putMessage(msg->ack_mailbox,
                                  new ServiceDaemonStoppedMessage(this->getPropertyValueAsDouble(
                                          FileRegistryServiceProperty::DAEMON_STOPPED_MESSAGE_PAYLOAD)));
        } catch (std::shared_ptr<NetworkError> cause) {
          return false;
        }
        return false;

      } else if (FileRegistryFileLookupRequestMessage *msg = dynamic_cast<FileRegistryFileLookupRequestMessage *>(message.get())) {
        std::set<StorageService *> locations;
        if (this->entries.find(msg->file) != this->entries.end()) {
          locations = this->entries[msg->file];
        }
        try {
          S4U_Mailbox::dputMessage(msg->answer_mailbox,
                                   new FileRegistryFileLookupAnswerMessage(msg->file, locations,
                                                                           this->getPropertyValueAsDouble(
                                                                                   FileRegistryServiceProperty::FILE_LOOKUP_ANSWER_MESSAGE_PAYLOAD)));
        } catch (std::shared_ptr<NetworkError> cause) {
          return true;
        }
        return true;

      } else if (FileRegistryAddEntryRequestMessage *msg = dynamic_cast<FileRegistryAddEntryRequestMessage *>(message.get())) {
        addEntryToDatabase(msg->file, msg->storage_service);
        try {
          S4U_Mailbox::dputMessage(msg->answer_mailbox,
                                   new FileRegistryAddEntryAnswerMessage(this->getPropertyValueAsDouble(
                                           FileRegistryServiceProperty::ADD_ENTRY_ANSWER_MESSAGE_PAYLOAD)));
        } catch (std::shared_ptr<NetworkError> cause) {
          return true;
        }
        return true;

      } else if (FileRegistryRemoveEntryRequestMessage *msg = dynamic_cast<FileRegistryRemoveEntryRequestMessage *>(message.get())) {

        bool success = removeEntryFromDatabase(msg->file, msg->storage_service);
        try {
          S4U_Mailbox::dputMessage(msg->answer_mailbox,
                                   new FileRegistryRemoveEntryAnswerMessage(success,
                                                                            this->getPropertyValueAsDouble(
                                                                                    FileRegistryServiceProperty::REMOVE_ENTRY_ANSWER_MESSAGE_PAYLOAD)));
        } catch (std::shared_ptr<NetworkError> cause) {
          return true;
        }
        return true;

      } else {
        throw std::runtime_error(
                "FileRegistryService::waitForNextMessage(): Unknown message type: " + std::to_string(message->payload));
      }
    }

    /**
     * Internal method to add an entry to the database
     * @param file: a file
     * @param storage_service: a storage_service
     */
    void FileRegistryService::addEntryToDatabase(WorkflowFile *file, StorageService *storage_service) {
      if (this->entries.find(file) != this->entries.end()) {
        this->entries[file].insert(storage_service);
      } else {
        this->entries[file] = {storage_service};
      }
    }

    /**
     * Internal method to remove an entry from the database
     * @param file: a file
     * @param storage_service: a storage_service
     *
     * @return true if an entry was removed
     */
    bool FileRegistryService::removeEntryFromDatabase(WorkflowFile *file, StorageService *storage_service) {
      if (this->entries.find(file) != this->entries.end()) {
        if (this->entries[file].find(storage_service) != this->entries[file].end()) {
          this->entries[file].erase(storage_service);
        } else {
          return false;
        }
      } else {
        return false;
      }
      return true;
    }


};
