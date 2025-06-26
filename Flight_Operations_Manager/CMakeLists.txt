# This is the top-level CMakeLists.txt file for the Flight Operations Manager project.
# It defines the project, specifies C++ standards, finds necessary Qt packages,
# collects source files, headers, UI files, and Qt resource files, and configures
# the build process for the main executable.

cmake_minimum_required(VERSION 3.16)

project(FlightOperationsManager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# enable all of Qt's auto-tools
set(CMAKE_AUTOMOC  ON)
set(CMAKE_AUTORCC  ON)
set(CMAKE_AUTOUIC  ON)

# Find Qt packages
find_package(Qt6 COMPONENTS Widgets Sql REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Set UI files directory
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/src/gui)

# Collect source files
set(SOURCES
    src/main.cpp
    src/models/Flight.cpp
    src/models/Seat.cpp
    src/models/Passenger.cpp
    src/models/Airline.cpp
    src/models/Ticket.cpp
    src/models/FlightStatus.cpp
    src/models/Baggage.cpp
    src/gui/MainWindow.cpp
    src/gui/FlightDialog.cpp
    src/gui/TicketDialog.cpp
    src/gui/PassengerDialog.cpp
    src/gui/SeatMapWidget.cpp
    src/database/Database.cpp
    src/database/SQLiteDatabase.cpp
    src/database/FileDatabase.cpp
)

set(HEADERS
    include/models/Flight.h
    include/models/Seat.h
    include/models/Passenger.h
    include/models/Airline.h
    include/models/Ticket.h
    include/models/FlightStatus.h
    include/models/Baggage.h
    include/gui/MainWindow.h
    include/gui/FlightDialog.h
    include/gui/TicketDialog.h
    include/gui/PassengerDialog.h
    include/gui/SeatMapWidget.h
    include/database/Database.h
    include/database/SQLiteDatabase.h
    include/database/FileDatabase.h
)

set(UI_FILES
    src/gui/mainwindow.ui
    src/gui/flightdialog.ui
    src/gui/ticketdialog.ui
    src/gui/passengerdialog.ui
)

# Create executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
)

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(Qt6_VERSION VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Flight_Operations_Manager)
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)