GCC=g++ -std=c++11
JsonLibFile=libjsoncpp.a
JsonLibFileDir=/opt/jsoncpp/build-static/
SourceDir=../
ExeFile=run
OutFiles=esp_json.o

edit : ${OutFiles}
	${GCC} ${OutFiles} ${JsonLibFileDir}${JsonLibFile} -o ${ExeFile}
esp_json.o : ${SourceDir}esp_json.cpp
	${GCC} -c ${SourceDir}esp_json.cpp
.PHONY : clean
clean:
	rm *.o
	rm ${ExeFile}

