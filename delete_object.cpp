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


void usage()
{
  printf("usage:\n\tcreate_bucket {bucketName} \n");
  return;
}


int main(int argc, char** argv)
{
  if (2 > argc){
    usage();
    return 1;
  }

  char *slash = argv[1];

  while (*slash && (*slash != '/')) {
    slash++;
  }
  *slash++ = 0;

  const char *bucketName = argv[1];
  const char *key = slash;


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

  S3BucketContext bucketContext =
      {
        0,
        bucketName,
        protocolG,
        S3UriStylePath,
        accessKeyIdG,
        secretAccessKeyG,
        0
      };

  S3ResponseHandler responseHandler =
      {
        0,
        &responseCompleteCallback
      };


  //----------------------create bucket-----------------//
  S3_delete_object(&bucketContext, key, NULL, 0, &responseHandler, NULL);
  S3_deinitialize();

  return 0;
}
