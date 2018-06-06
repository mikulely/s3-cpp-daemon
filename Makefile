cc=g++
LDFLAGS += -lpthread -ls3
CXXFLAGS += -I /usr/include

all: create_bucket.cpp put_object.cpp get_object.cpp delete_object.cpp list_service.cpp delete_bucket.cpp check_bucket_exist.cpp check_object_exist.cpp
	$(cc) $(CXXFLAGS) create_bucket.cpp $(LDFLAGS) -o create_bucket
	$(cc) $(CXXFLAGS) put_object.cpp $(LDFLAGS) -o put_object
	$(cc) $(CXXFLAGS) get_object.cpp $(LDFLAGS) -o get_object
	$(cc) $(CXXFLAGS) delete_object.cpp $(LDFLAGS) -o delete_object
	$(cc) $(CXXFLAGS) list_service.cpp $(LDFLAGS) -o list_service
	$(cc) $(CXXFLAGS) delete_bucket.cpp $(LDFLAGS) -o delete_bucket
	$(cc) $(CXXFLAGS) check_bucket_exist.cpp $(LDFLAGS) -o check_bucket_exist
	$(cc) $(CXXFLAGS) check_object_exist.cpp $(LDFLAGS) -o check_object_exist

create_bucket: create_bucket.cpp
	$(cc) $(CXXFLAGS) create_bucket.cpp $(LDFLAGS) -o create_bucket
put_object: put_object.cpp
	$(cc) $(CXXFLAGS) put_object.cpp $(LDFLAGS) -o put_object
get_object: get_object.cpp
	$(cc) $(CXXFLAGS) get_object.cpp $(LDFLAGS) -o get_object
delete_object: delete_object.cpp
	$(cc) $(CXXFLAGS) delete_object.cpp $(LDFLAGS) -o delete_object
list_service: list_service.cpp
	$(cc) $(CXXFLAGS) list_service.cpp $(LDFLAGS) -o list_service
delete_bucket: delete_bucket.cpp
	$(cc) $(CXXFLAGS) delete_bucket.cpp $(LDFLAGS) -o delete_bucket
check_bucket_exist: check_bucket_exist.cpp
	$(cc) $(CXXFLAGS) check_bucket_exist.cpp $(LDFLAGS) -o check_bucket_exist
check_object_exist: check_object_exist.cpp
	$(cc) $(CXXFLAGS) check_object_exist.cpp $(LDFLAGS) -o check_object_exist


clean:
	rm create_bucket put_object get_object delete_object list_service delete_bucket check_bucket_exist check_object_exist
