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
  printf("usage:\n\tcreate_bucket {bucketName/key} {filename}\n");
  return;
}


static S3Status getObjectDataCallback(int bufferSize, const char *buffer,
                                      void *callbackData)
{
  FILE *outfile = (FILE *) callbackData;

  size_t wrote = fwrite(buffer, 1, bufferSize, outfile);

  return ((wrote < (size_t) bufferSize) ?
          S3StatusAbortedByCallback : S3StatusOK);
}


int main(int argc, char** argv)
{
  if (3 > argc){
    usage();
    return 1;
  }

  char *slash = argv[1];
  while (*slash && (*slash != '/')) {
    slash++;
  }
  if (!*slash || !*(slash + 1)) {
    fprintf(stderr, "\nERROR: Invalid bucket/key name: %s\n",
            argv[1]);
    usage();
  }
  *slash++ = 0;

  const char* bucketName = argv[1];
  const char *key = slash;
  const char *filename = argv[2];

  S3Protocol protocolG = S3ProtocolHTTP;
  const char* accessKeyIdG = getenv("S3_ACCESS_KEY_ID");
  const char* secretAccessKeyG = getenv("S3_SECRET_ACCESS_KEY");

  //const char *filename = 0;
  int64_t ifModifiedSince = -1, ifNotModifiedSince = -1;
  const char *ifMatch = 0, *ifNotMatch = 0;
  uint64_t startByte = 0, byteCount = 0;
  FILE *outfile = 0;

  outfile = fopen(filename, "w" "");



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

  S3GetConditions getConditions =
      {
        ifModifiedSince,
        ifNotModifiedSince,
        ifMatch,
        ifNotMatch
      };

  S3GetObjectHandler getObjectHandler =
      {
        { &responsePropertiesCallback, &responseCompleteCallback },
        &getObjectDataCallback
      };


  //----------------------create bucket-----------------//
  S3_get_object(&bucketContext, key, &getConditions, startByte,
                byteCount, NULL, 0, &getObjectHandler, outfile);
  S3_deinitialize();

  return 0;
}
