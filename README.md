# C++ SDK for Ceph RGW

how to use test case

1.install libs3
  
    git clone https://github.com/bji/libs3/
    cd libs3
    make
    make install

2.compile test code

    git clone https://github.com/mikulely/s3-cpp-daemon
    cd s3-cpp-daemon
  
    #compile create_bucket.cpp
    make create_bucket
    #compile put_object.cpp
    make put_object
    ...

    or

    #compile all
    make all
    
3.run test code

    export S3_ACCESS_KEY_ID=***(ceph user accesskey)
    export S3_SECRET_ACCESS_KEY=***(ceph user secretkey)
    export S3_HOSTNAME=ip_addr:port(ceph rgw)
    
    ./list_service
    ./creat_bucket bucket_name
    ./check_bucket_exist bucket_name
    ./put_object bucket_name/new_file_name file_name
    ./check_object_exist bucket_name new_file_name
    ./get_object bucket_name/new_file_name file_name
    ./delete_object bucket_name new_file_name
    ./delete_bucket bucket_name
