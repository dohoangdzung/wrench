/**
 * Copyright (c) 2017. The WRENCH Team.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */


#ifndef WRENCH_STORAGESERVICEPROPERTY_H
#define WRENCH_STORAGESERVICEPROPERTY_H

#include <services/ServiceProperty.h>


namespace wrench {

   /**
    * @brief Properties for a StorageService
    */
    class StorageServiceProperty : public ServiceProperty {

    public:

        /** @brief The number of bytes in the control message sent to the daemon to request its free space information **/
        DECLARE_PROPERTY_NAME(FREE_SPACE_REQUEST_MESSAGE_PAYLOAD);
        /** @brief The number of bytes in the control message sent by the daemon to answer free space information request **/
        DECLARE_PROPERTY_NAME(FREE_SPACE_ANSWER_MESSAGE_PAYLOAD);
       
        /** @brief The number of bytes in the control message sent to the daemon to request a file lookup **/
        DECLARE_PROPERTY_NAME(FILE_LOOKUP_REQUEST_MESSAGE_PAYLOAD);
        /** @brief The number of bytes in the control message sent by the daemon to answer a file lookup request **/
        DECLARE_PROPERTY_NAME(FILE_LOOKUP_ANSWER_MESSAGE_PAYLOAD);

        /** @brief The number of bytes in the control message sent to the daemon to request a file deletion **/
        DECLARE_PROPERTY_NAME(FILE_DELETE_REQUEST_MESSAGE_PAYLOAD);
        /** @brief The number of bytes in the control message sent by the daemon to answer a file deletion request **/
        DECLARE_PROPERTY_NAME(FILE_DELETE_ANSWER_MESSAGE_PAYLOAD);

        /** @brief The number of bytes in the control message sent to the daemon to request a file copy **/
        DECLARE_PROPERTY_NAME(FILE_COPY_REQUEST_MESSAGE_PAYLOAD);
        /** @brief The number of bytes in the control message sent by the daemon to answer a file copy request **/
        DECLARE_PROPERTY_NAME(FILE_COPY_ANSWER_MESSAGE_PAYLOAD);

        /** @brief The number of bytes in the control message sent to the daemon to request a file copy **/
        DECLARE_PROPERTY_NAME(FILE_WRITE_REQUEST_MESSAGE_PAYLOAD);
        /** @brief The number of bytes in the control message sent by the daemon to answer a file copy request **/
        DECLARE_PROPERTY_NAME(FILE_WRITE_ANSWER_MESSAGE_PAYLOAD);

        /** @brief The number of bytes in the control message sent to the daemon to request a file copy **/
        DECLARE_PROPERTY_NAME(FILE_READ_REQUEST_MESSAGE_PAYLOAD);
        /** @brief The number of bytes in the control message sent by the daemon to answer a file copy request **/
        DECLARE_PROPERTY_NAME(FILE_READ_ANSWER_MESSAGE_PAYLOAD);

        /** @brief The number of bytes in the control message sent by the daemon to say "file not found" **/
        DECLARE_PROPERTY_NAME(FILE_NOT_FOUND_MESSAGE_PAYLOAD);

        /** @brief The number of bytes in the control message sent by the daemon to say "not enough space" **/
        DECLARE_PROPERTY_NAME(NOT_ENOUGH_STORAGE_SPACE_MESSAGE_PAYLOAD);

    };

};


#endif //WRENCH_STORAGESERVICEPROPERTY_H
