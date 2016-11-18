#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <getopt.h>
#include <ctype.h>

#include "libs3.h"


static S3Status responsePropertiesCallback(const S3ResponseProperties *properties,
                                           void *callbackData)
{

    return S3StatusOK;
}


static void responseCompleteCallback(S3Status status,
                                     const S3ErrorDetails *error, 
                                     void *callbackData)
{

}



static S3Status listServiceCallback(const char *ownerId, 
                                    const char *ownerDisplayName,
                                    const char *bucketName,
                                    int64_t creationDate, void *callbackData)
{
    bool *header_printed = (bool*) callbackData;
    if (!*header_printed) {
        *header_printed = true;
        printf("%-22s", "       Bucket");
        printf("  %-20s  %-12s", "     Owner ID", "Display Name");
        printf("\n");
        printf("----------------------");
        printf("  --------------------" "  ------------");
        printf("\n");
    }

    printf("%-22s", bucketName);
    printf("  %-20s  %-12s", ownerId ? ownerId : "", ownerDisplayName ? ownerDisplayName : "");
    printf("\n");

    return S3StatusOK;
}


int main(int argc, char** args)
{
    S3Protocol protocolG = S3ProtocolHTTP;
    const char* accessKeyIdG = getenv("S3_ACCESS_KEY_ID");
    const char* secretAccessKeyG = getenv("S3_SECRET_ACCESS_KEY");

    S3Status status;
    const char* hostname = getenv("S3_HOSTNAME");    
    if ((status = S3_initialize("s3", S3_INIT_ALL, hostname)) != S3StatusOK)
    {
        fprintf(stdout, "Failed to initialize libs3: %s\n", S3_get_status_name(status));
        exit(-1);
    }

    S3ResponseHandler responseHandler = { &responsePropertiesCallback,
                                          &responseCompleteCallback
                                        };
    S3ListServiceHandler listServiceHandler = { responseHandler,
                                                &listServiceCallback
                                               };

    //----------------------list all owned bucket-----------------//
    bool data = false;
    S3_list_service(protocolG, accessKeyIdG, secretAccessKeyG, 0, 0, 0, 
                        &listServiceHandler, &data);

    //-----------------------create bucket------------------------//
    const char* bucketName = "unitedstack-bucket";
    S3_create_bucket(protocolG, accessKeyIdG, secretAccessKeyG, 0,
                         0, bucketName, S3CannedAclPrivate, 0, 0,
                         &responseHandler, 0);

    S3_deinitialize();

    return 0;
}
