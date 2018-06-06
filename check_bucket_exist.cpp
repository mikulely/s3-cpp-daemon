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
  const char* statusName = S3_get_status_name(status);

  if (!strcmp(statusName,"OK")){
    printf("Response status code is : %s\n", statusName);
    printf("This object is exist.\n");
  }
  else{
    printf("Response status code is : %s\n", statusName);
    printf("Don't found this object.\n");
  }

}


void usage()
{
  printf("usage:\n\tcheck_object_exist {bucketName} {object}\n");
  return;
}


int main(int argc, char** args)
{
  if (2 > argc){
    usage();
    return 1;
  }

  const char* bucketName = args[1];

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

  char locationConstraint[64];
  S3ResponseHandler responseHandler = { &responsePropertiesCallback,
                                        &responseCompleteCallback
  };

  //----------------------check whether bucket is exist-----------------//
  S3_test_bucket(protocolG, S3UriStylePath, accessKeyIdG, secretAccessKeyG,
                 NULL, NULL, bucketName, "US", sizeof(locationConstraint),
                 locationConstraint, NULL, 0, &responseHandler, NULL);

  S3_deinitialize();

  return 0;
}
