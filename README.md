# Two-Way-Traffic-Bridge-Thread
Thread Synchronization
The objective is to provide some hands-on experience on
multithreaded programming and thread synchronization.

You will design and implement a multithreaded program to control traffic on an
imaginary bridge with two lanes.

Traffic Patterns: Traffic will consist of cars and vans. A vehicle (car or van)
maybe traveling southbound or northbound: The direction and type of each
vehicle will be determined according to given probability distributions (see
below). Cars and vans will weigh 200 and 300 units each, respectively. Each
vehicle will take 3 seconds to cross the bridge. Your program will enforce
traffic control policies and bridge restrictions while providing fair and fast
traffic flow.

Bridge Restrictions:
R1. Vehicles cannot change lanes while on the bridge, and a given lane should
never be opened to traffic in opposite directions at the same time (otherwise, a
collision will occur).
R2. Due to construction constraints, the total weight of all the vehicles on
the bridge cannot exceed 1200 units at any time.

Traffic Control Policies:
The traffic flow will be controlled according to the following rules.
P1. As long as oncoming traffic is in both directions, one lane will be allocated
for southbound and the other for northbound traffic.
P2. If there are no waiting/crossing vehicles in one direction, then both lanes
will be used by the vehicles coming from the other direction (if any) to improve
the traffic flow. And, while both lanes are being used in one direction, policy P1
will be effective as soon as vehicles arrive at the other end of the bridge.
However, before changing the traffic direction in a given lane, your program
must wait until all the vehicles already in that lane leave (otherwise, a crash will
occur).
P3. Subject to restrictions R1-R2 and policies P1-P2, no vehicle should incur a
delay. If there is sufficient traffic, the bridge capacity must be fully utilized (for
example, having vehicles cross the bridge one by one is not acceptable). The
deadlocks should also be prevented.
