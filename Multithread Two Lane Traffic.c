// Author: Tobechi Onwenu

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_VEHICLES 1000
#define MAX_GROUPS 10

typedef struct
{
    int id;
    char direction;
    char type;
    int weight;
} Vehicle;

Vehicle bridgeLane1[MAX_VEHICLES];
Vehicle bridgeLane2[MAX_VEHICLES];

int lane1Count = 0;
int lane2Count = 0;
int waitingNorthCount = 0;
int waitingSouthCount = 0;
int totalWeight = 0;
int vehicleCount = 0;

pthread_mutex_t bridgeLock;
pthread_cond_t northboundCV;
pthread_cond_t southboundCV;

// Initialize mutexes and condition variables
void initializeLanes()
{
    pthread_mutex_init(&bridgeLock, NULL);
    pthread_cond_init(&northboundCV, NULL);
    pthread_cond_init(&southboundCV, NULL);
}

// Print the current status of the bridge, including vehicles on each lane, total weight, and waiting counts
void printBridgeStatus()
{
    printf("Bridge status:\n");
    printf("Lane 1 (Northbound): ");
    for (int i = 0; i < lane1Count; i++)
        printf("Car #%d ", bridgeLane1[i].id);
    printf("\n");

    printf("Lane 2 (Southbound): ");
    for (int i = 0; i < lane2Count; i++)
        printf("Car #%d ", bridgeLane2[i].id);
    printf("\n");

    printf("Total weight on bridge: %d\n", totalWeight);
    printf("Waiting northbound: %d\n", waitingNorthCount);
    printf("Waiting southbound: %d\n", waitingSouthCount);
    printf("\n");
}

// Vehicle's arrival at the bridge
void Arrive(Vehicle *vehicle)
{
    pthread_mutex_lock(&bridgeLock);

    if (vehicle->direction == 'n')
    {
        // Check if the vehicle can enter the bridge lane based on weight and lane capacity
        if (totalWeight + vehicle->weight > 20 || lane1Count > 2 || lane2Count > 0)
        {
            // Vehicle must wait if the bridge is overloaded or lane 1 is full or lane 2 has vehicles
            waitingNorthCount++;
            pthread_cond_wait(&northboundCV, &bridgeLock);
        }

        // Vehicle can enter lane 1
        bridgeLane1[lane1Count++] = *vehicle;
        totalWeight += vehicle->weight;
        printf("Car #%d from the northbound lane has arrived at the bridge.\n", vehicle->id);
        printBridgeStatus();
    }
    else if (vehicle->direction == 's')
    {
        // Check if the vehicle can enter the bridge lane based on weight and lane capacity
        if (totalWeight + vehicle->weight > 20 || lane2Count > 2 || lane1Count > 0)
        {
            // Vehicle must wait if the bridge is overloaded or lane 2 is full or lane 1 has vehicles
            waitingSouthCount++;
            pthread_cond_wait(&southboundCV, &bridgeLock);
        }

        // Vehicle can enter lane 2
        bridgeLane2[lane2Count++] = *vehicle;
        totalWeight += vehicle->weight;
        printf("Car #%d from the southbound lane has arrived at the bridge.\n", vehicle->id);
        printBridgeStatus();
    }

    pthread_mutex_unlock(&bridgeLock);
}

// Vehicle's crossing the bridge
void Cross(Vehicle *vehicle)
{
    printf("Car #%d is crossing the bridge.\n", vehicle->id);
    sleep(1);
}

// Vehicle's departure from the bridge
void Leave(Vehicle *vehicle)
{
    pthread_mutex_lock(&bridgeLock);

    if (vehicle->direction == 'n')
    {
        // Find the vehicle in lane 1 and remove it
        for (int i = 0; i < lane1Count; i++)
        {
            if (bridgeLane1[i].id == vehicle->id)
            {
                // Shift remaining vehicles in lane 1 to the left
                for (int j = i; j < lane1Count - 1; j++)
                    bridgeLane1[j] = bridgeLane1[j + 1];

                lane1Count--;
                totalWeight -= vehicle->weight;
                printf("Car #%d from the northbound lane has left the bridge.\n", vehicle->id);
                printBridgeStatus();
                break;
            }
        }

        // Signal a waiting southbound vehicle or a waiting northbound vehicle if no more vehicles in lane 1
        if (waitingSouthCount > 0)
        {
            waitingSouthCount--;
            pthread_cond_signal(&southboundCV);
        }
        else if (lane1Count == 0 && waitingNorthCount > 0)
        {
            waitingNorthCount--;
            pthread_cond_signal(&northboundCV);
        }
    }
    else if (vehicle->direction == 's')
    {
        // Find the vehicle in lane 2 and remove it
        for (int i = 0; i < lane2Count; i++)
        {
            if (bridgeLane2[i].id == vehicle->id)
            {
                // Shift remaining vehicles in lane 2 to the left
                for (int j = i; j < lane2Count - 1; j++)
                    bridgeLane2[j] = bridgeLane2[j + 1];

                lane2Count--;
                totalWeight -= vehicle->weight;
                printf("Car #%d from the southbound lane has left the bridge.\n", vehicle->id);
                printBridgeStatus();
                break;
            }
        }

        // Signal a waiting northbound vehicle or a waiting southbound vehicle if no more vehicles in lane 2
        if (waitingNorthCount > 0)
        {
            waitingNorthCount--;
            pthread_cond_signal(&northboundCV);
        }
        else if (lane2Count == 0 && waitingSouthCount > 0)
        {
            waitingSouthCount--;
            pthread_cond_signal(&southboundCV);
        }
    }

    free(vehicle);
    pthread_mutex_unlock(&bridgeLock);
}

// Routine for each vehicle thread
void *VehicleRoutine(void *arg)
{
    Vehicle *vehicle = (Vehicle *)arg;

    Arrive(vehicle);
    Cross(vehicle);
    Leave(vehicle);

    pthread_exit(NULL);
}

// Enter schedule for vehicle groups and create corresponding vehicles in threads
void enterSchedule(int groupCount)
{
    int delay[MAX_GROUPS];
    int northboundCount[MAX_GROUPS];
    int southboundCount[MAX_GROUPS];

    // Get vehicle group information from the user
    for (int i = 0; i < groupCount; i++)
    {
        printf("Group %d:\n", i + 1);
        printf("Enter the number of vehicles in the northbound lane: ");
        scanf("%d", &northboundCount[i]);
        printf("Enter the number of vehicles in the southbound lane: ");
        scanf("%d", &southboundCount[i]);
        printf("Enter the delay before the next group (0 if none): ");
        scanf("%d", &delay[i]);
    }

    // Create vehicles for each group and start their threads
    for (int i = 0; i < groupCount; i++)
    {
        int n = northboundCount[i];
        int s = southboundCount[i];

        // Create vehicles for the northbound lane
        for (int j = 0; j < n; j++)
        {
            Vehicle *vehicle = (Vehicle *)malloc(sizeof(Vehicle));
            vehicle->id = ++vehicleCount;
            vehicle->direction = 'n';
            vehicle->type = (rand() % 2) == 0 ? 'c' : 'v'; // Randomly choose 'c' for car or 'v' for van
            vehicle->weight = (vehicle->type == 'c') ? 3 : 5; // Set weight based on vehicle type

            pthread_t vehicleThread;
            pthread_create(&vehicleThread, NULL, VehicleRoutine, (void *)vehicle);
        }

        // Create vehicles for the southbound lane
        for (int j = 0; j < s; j++)
        {
            Vehicle *vehicle = (Vehicle *)malloc(sizeof(Vehicle));
            vehicle->id = ++vehicleCount;
            vehicle->direction = 's';
            vehicle->type = (rand() % 2) == 0 ? 'c' : 'v'; // Randomly choose 'c' for car or 'v' for van
            vehicle->weight = (vehicle->type == 'c') ? 3 : 5; // Set weight based on vehicle type

            pthread_t vehicleThread;
            pthread_create(&vehicleThread, NULL, VehicleRoutine, (void *)vehicle);
        }

        // Sleep to introduce a delay before the next group
        if (i < groupCount - 1 && delay[i] > 0)
            sleep(delay[i]);
    }
}

// Main function
int main()
{
    srand(time(NULL));
    int groupCount;

    printf("Enter the number of vehicle groups: ");
    scanf("%d", &groupCount);

    initializeLanes();
    enterSchedule(groupCount);

    pthread_exit(NULL);
}
