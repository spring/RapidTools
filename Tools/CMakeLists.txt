add_executable (AddZip AddZip.cpp)
target_link_libraries (AddZip Rapid)

add_executable (BuildGit BuildGit.cpp)
target_link_libraries (BuildGit Rapid)

add_executable (MakeZip MakeZip.cpp)
target_link_libraries (MakeZip Rapid)

add_executable (Streamer Streamer.cpp)
target_link_libraries (Streamer Rapid)
