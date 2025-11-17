cmake_minimum_required(VERSION 3.25)
project(PEA_3)

set(CMAKE_CXX_STANDARD 17)
# Flaga -static może powodować problemy w zależności od środowiska.
# Jeśli kompilacja się nie powiedzie, spróbuj ją zakomentować (dodać # na początku linii).
set(CMAKE_EXE_LINKER_FLAGS "-static")

# 1. Stwórz listę plików źródłowych, które są wspólne dla obu programów.
# Plików .h nie dodajemy tutaj!
set(COMMON_SOURCES
        Genetyczny.cpp
        Graf.cpp
        )

# 2. Zdefiniuj pierwszy program (ten z interaktywnym menu).
# Nazwa pliku wykonywalnego: "MenuApp" (możesz wybrać inną).
# Składa się z: main.cpp, Menu.cpp oraz plików wspólnych.
add_executable(MenuApp
        main.cpp
        Menu.cpp
        ${COMMON_SOURCES}
        )

# 3. Zdefiniuj drugi program (do automatycznych testów).
# Nazwa pliku wykonywalnego: "TestRunnerApp" (możesz wybrać inną).
# Składa się z: TestRunner.cpp oraz plików wspólnych.
add_executable(TestRunnerApp
        TestRunner.cpp
        ${COMMON_SOURCES}
        )