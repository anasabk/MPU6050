if(ESP_PLATFORM)
    set(SOURCES MPU6050.cpp)

    idf_component_register(
        SRCS ${SOURCES}
        INCLUDE_DIRS . 
        REQUIRES I2Cdev
    )
        
else(ESP_PLATFORM)
    add_library(MPU6050 MPU6050.cpp MPU6050.h)
    target_link_libraries(MPU6050 I2Cdev)
    target_include_directories(MPU6050 PUBLIC .)
endif(ESP_PLATFORM)