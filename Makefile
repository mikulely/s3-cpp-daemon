cc=g++
LDFLAGS +=  -lpthread -ls3
target=delete_object
exe=$(target)
obj=$(target).o 
resource=$(target).cpp 
$(exe):$(obj)
	$(cc) -o $(exe) $(obj)  $(LDFLAGS)
$(obj):$(resource)
	$(cc) -c $(resource) 
clean:
	rm -rf *.o main
