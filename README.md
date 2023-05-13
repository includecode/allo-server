# Snowpack
This program is a simulation on connection between users throught a proxy server.

How to use this program

1. Unzip file

2. Go to directory "Walkers" where "src" and "inc" are located

 
**********
 
 Usage:
./app   path/to/file   index  metric
path/to/file  : Path to input data file
index         : Index of the path to find 3 closest
metric        : Metric to use (0 = Distance, 1 = Speed)
Example       : ./app   src/cpp_test_trajectories/small_trajectory.dat  2  0
This example will show the 3 closest trajectories to the 2nd trajectory according to distance
**********
