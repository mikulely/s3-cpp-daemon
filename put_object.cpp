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


#define FOPEN_EXTRA_FLAGS ""


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


typedef struct growbuffer
{
  // The total number of bytes, and the start byte
  int size;
  // The start byte
  int start;
  // The blocks
  char data[64 * 1024];
  struct growbuffer *prev, *next;
} growbuffer;


typedef struct put_object_callback_data
{
  FILE *infile;
  growbuffer *gb;
  uint64_t contentLength, originalContentLength;
  int noStatus;
} put_object_callback_data;


static void growbuffer_read(growbuffer **gb, int amt, int *amtReturn,
                            char *buffer)
{
  *amtReturn = 0;

  growbuffer *buf = *gb;

  if (!buf) {
    return;
  }

  *amtReturn = (buf->size > amt) ? amt : buf->size;

  memcpy(buffer, &(buf->data[buf->start]), *amtReturn);

  buf->start += *amtReturn, buf->size -= *amtReturn;

  if (buf->size == 0) {
    if (buf->next == buf) {
      *gb = 0;
    }
    else {
      *gb = buf->next;
      buf->prev->next = buf->next;
      buf->next->prev = buf->prev;
    }
    free(buf);
  }
}


static int putObjectDataCallback(int bufferSize, char *buffer,
                                 void *callbackData)
{
  put_object_callback_data *data =
      (put_object_callback_data *) callbackData;

  int ret = 0;

  if (data->contentLength) {
    int toRead = ((data->contentLength > (unsigned) bufferSize) ?
                  (unsigned) bufferSize : data->contentLength);
    if (data->gb) {
      growbuffer_read(&(data->gb), toRead, &ret, buffer);
    }
    else if (data->infile) {
      ret = fread(buffer, 1, toRead, data->infile);
    }
  }

  data->contentLength -= ret;

  if (data->contentLength && !data->noStatus) {
    // Avoid a weird bug in MingW, which won't print the second integer
    // value properly when it's in the same call, so print separately
    printf("%llu bytes remaining ",
           (unsigned long long) data->contentLength);
    printf("(%d%% complete) ...\n",
           (int) (((data->originalContentLength -
                    data->contentLength) * 100) /
                  data->originalContentLength));
  }

  return ret;
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
    exit(-1);
  }
  *slash++ = 0;

  const char* bucketName = argv[1];
  const char *key = slash;
  const char *uploadId = 0;
  const char *filename = argv[2];
  uint64_t contentLength = 0;
  const char *cacheControl = 0, *contentType = 0, *md5 = 0;
  const char *contentDispositionFilename = 0, *contentEncoding = 0;
  int64_t expires = -1;
  S3CannedAcl cannedAcl = S3CannedAclPrivate;
  int metaPropertiesCount = 0;
  S3NameValue metaProperties[S3_MAX_METADATA_COUNT];
  char useServerSideEncryption = 0;
  int noStatus = 0;

  put_object_callback_data data;

  data.infile = 0;
  data.gb = 0;
  data.noStatus = noStatus;

  if (filename) {
    if (!contentLength) {
      struct stat statbuf;
      // Stat the file to get its length
      if (stat(filename, &statbuf) == -1) {
        fprintf(stderr, "\nERROR: Failed to stat file %s: ",
                filename);
        perror(0);
        exit(-1);
      }
      contentLength = statbuf.st_size;
    }
    // Open the file
    if (!(data.infile = fopen(filename, "r" FOPEN_EXTRA_FLAGS))) {
      fprintf(stderr, "\nERROR: Failed to open input file %s: ",
              filename);
      perror(0);
      exit(-1);
    }
  }
  else{
    usage();
  }
  data.contentLength = data.originalContentLength = contentLength;

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

  S3PutProperties putProperties =
      {
        contentType,
        md5,
        cacheControl,
        contentDispositionFilename,
        contentEncoding,
        expires,
        cannedAcl,
        metaPropertiesCount,
        metaProperties,
        useServerSideEncryption
      };

  S3PutObjectHandler putObjectHandler =
      {
        { &responsePropertiesCallback, &responseCompleteCallback },
        &putObjectDataCallback
      };
  //----------------------create bucket-----------------//
  S3_put_object(&bucketContext, key, contentLength, &putProperties, 0,
                &putObjectHandler, &data);
  S3_deinitialize();

  return 0;
}
