# определяем минимальную версию для компиляции
cmake_minimum_required(VERSION 3.5)

# НАЗНАЧАЕМ ИМЯ ПРОЕКТУ И УСТАНАВЛИВАЕМ НОМЕР ВЕРСИИ------------------------
# если определена среда сборки Travis то выставляем номер сборки из Travis
if($ENV{TRAVIS_BUILD_NUMBER})
project(bulk_server VERSION 1.1.$ENV{TRAVIS_BUILD_NUMBER})

# если среда не найдена то ставим номер сборки указанный ниже
else()
project(bulk_server VERSION 1.1.5)
endif()
# --------------------------------------------------------------------------

# ДОБАВЛЯЕМ В ПРОЕКТ ДОПОЛНИТЕЛЬНЫЕ БИБЛИОТЕКИ ДЛЯ ТЕСТИРОВАНИЯ-------------
# Опция "REQUIRED" говорит о том, что библиотека необходима проекту. Без нее cmake решит, что отсутствие данной библиотеки - не так уж и страшно, и будет собирать дальше.
find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(GTest REQUIRED)
find_package(Threads)
# Добавим директории с хедерами Boost, gtest и директорию с исходниками для поиска в них хедеров
include_directories(${Boost_INCLUDE_DIRS})
# include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
include_directories(${GTEST_INCLUDE_DIRS})
#include_directories(${PROJECT_SOURCE_DIR})
# --------------------------------------------------------------------------

# Генерируем файл version.h при сборке из файла с уловиями version.h.in
configure_file(${CMAKE_SOURCE_DIR}/include/version.h.in ${CMAKE_SOURCE_DIR}/include/version.h)

# СОЗДАЕМ ИСПОЛНЯЕМЫЕ ФАЙЛЫ И ДОБОВЛЯЕМ БИБЛИОТЕКИ В ПРОЕКТ-----------------
# Создаем исполняемый фаил print_ip из main.cpp
add_executable(bulk_server ${CMAKE_SOURCE_DIR}/source/server.cpp ${CMAKE_SOURCE_DIR}/source/arg.cpp 
${CMAKE_SOURCE_DIR}/source/asio_async_server.cpp)
add_executable(bulk_client ${CMAKE_SOURCE_DIR}/source/client.cpp ${CMAKE_SOURCE_DIR}/source/arg_client.cpp
${CMAKE_SOURCE_DIR}/source/asio_client.cpp)
# Создаем библиотеку с именем versionlib из источника - файла с именем version_lib.cpp
add_library(versionlib ${CMAKE_SOURCE_DIR}/source/version_lib.cpp)
add_library(async ${CMAKE_SOURCE_DIR}/source/myasync.cpp ${CMAKE_SOURCE_DIR}/source/data.cpp)

# Создаем исполняемый фаил google_test из test.cpp (для Boost test)
# Создаем исполняемый фаил gtest из gtest.cpp
add_executable(gtest  ${CMAKE_SOURCE_DIR}/source/gtest.cpp  ${CMAKE_SOURCE_DIR}/source/arg.cpp)
# --------------------------------------------------------------------------

#Функция установки пакетов CONAN в проект для линковки(я так понял)
#conan_basic_setup()

# УСТАНАВЛИВАЕМ СВОЙСТВА ДЛЯ КОМПИЛЯЦИИ ИСПОЛНЯЕМЫМ ФАЙЛАМ И БИБЛИОТЕКАМ----

set_target_properties(bulk_server PROPERTIES
# Стандарт С++ версии 17
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON

    COMPILE_OPTIONS "-Wall;-Wpedantic;-Wextra;-fsanitize=thread"
    LINK_FLAGS "-fsanitize=thread"
    )
set_target_properties(bulk_client versionlib async gtest PROPERTIES
# Стандарт С++ версии 17
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    )
    
target_include_directories(bulk_server
    PRIVATE "${CMAKE_SOURCE_DIR}/include"
)
target_include_directories(bulk_client
    PRIVATE "${CMAKE_SOURCE_DIR}/include"
)
target_include_directories(versionlib
    PRIVATE "${CMAKE_SOURCE_DIR}/include"
)
target_include_directories(async
    PRIVATE "${CMAKE_SOURCE_DIR}/include"
)
target_include_directories(gtest
    PRIVATE "${CMAKE_SOURCE_DIR}/include"
)


target_link_libraries(bulk_server PRIVATE
    versionlib
    async
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    Threads::Threads
)
target_link_libraries(bulk_client PRIVATE
    versionlib
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    Threads::Threads
)
target_link_libraries(async PRIVATE
 Threads::Threads
)


target_link_libraries(gtest
    ${GTEST_LIBRARIES}
    versionlib
    )
target_link_libraries(gtest
${CMAKE_THREAD_LIBS_INIT}
versionlib
)

# СВОЙСТВА ДЛЯ КОМПИЛЯТОРОВ (ЛЮБЫЕ ПРЕДУПРЕЖДЕНИЯ ТРАКТОВАТЬ КАК ОШИБКИ)
if (MSVC)
    target_compile_options(bulk_server PRIVATE
        /W4
    )
    target_compile_options(bulk_client PRIVATE
    /W4
    )
    target_compile_options(versionlib PRIVATE
        /W4
    )
    target_compile_options(async PRIVATE
    /W4
    )
    target_compile_options(gtest PRIVATE
        /W4
    )
else ()
    target_compile_options(bulk_server PRIVATE
        -Wall -Wextra -pedantic -Werror -fsanitize=thread
    )
    target_compile_options(bulk_client PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(versionlib PRIVATE
    -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(async PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(gtest PRIVATE
            -Wall -Wextra -pedantic -Werror
    )
endif()

# ----------------------------------------------------------------------

install(TARGETS bulk_server bulk_client RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT sise07@gmail.com)

include(CPack)

enable_testing()

add_test(version_test_case gtest)



