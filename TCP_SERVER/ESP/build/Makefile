# This is the whole project's makefile
LibJson=/opt/jsoncpp/build-static/libjsoncpp.a
ExLinks=-pthread
SourceDir=../
CC=g++ -std=c++11
ALL=esp_json.o esp_tcp_server.o

edit : ${ALL} ${SourceDir}main.cpp
	${CC} ${ALL} ${SourceDir}main.cpp ${LibJson} ${ExLinks} -o run
esp_json.o : ${SourceDir}esp_json.cpp
	${CC} -c ${SourceDir}esp_json.cpp
esp_tcp_server.o : ${SourceDir}esp_tcp_server.cpp
	${CC} -c ${SourceDir}esp_tcp_server.cpp ${ExLinks}
.PHONY : clean
clean :
	rm *.o
	rm run
