cmake_minimum_required(VERSION 2.8)
find_package( OpenCV REQUIRED )
set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME ON) 

find_package(Boost COMPONENTS system filesystem) 

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    add_executable (EAPimg Main.cpp Image.cpp)
    target_link_libraries(EAPimg ${OpenCV_Libs} ${OpenCV_LIBRARIES} ${Boost_LIBRARIES})
endif()

#
#add_executable (EAPimg
#    Main.cpp
#    Image.cpp
#)
#    target_link_libraries(EAPimg ${OpenCV_Libs} ${OpenCV_LIBRARIES})
