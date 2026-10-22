# Adaptive Emergency Evacuation Route Optimization

An emergency-response navigation system built in C++ that calculates minimum-cost evacuation routes using graph theory, Dijkstra's algorithm, a priority queue, MongoDB persistence, and live weather data from the Open-Meteo API.

---

## Project Overview

During emergencies such as floods, fires, and road blockages, ordinary routes may become hazardous or impassable. This project models a road network as a weighted graph and calculates routes based on road connections and associated costs.

The application supports road-condition updates, MongoDB storage, and weather-data retrieval through an external API. These components provide the foundation for an adaptive evacuation-routing system.

## Project Progress & Implementation Status

The core C++ backend has been implemented, MongoDB connectivity and road-data loading have been integrated, and the weather API has been tested successfully.

### Implementation Checklist

- **Location Module** — Represents locations in the road network.
- **Road Module** — Represents roads, their distances, and associated properties.
- **Graph Module** — Represents the road network using an adjacency list.
- **Dijkstra's Algorithm** — Calculates minimum-cost routes for non-negative edge weights.
- **Priority Queue** — Efficiently selects the next vertex with the smallest tentative cost.
- **Route Optimization** — Finds a route between source and destination locations.
- **Disaster Manager** — Supports road-condition updates for safe, risky, blocked, and congested roads.
- **MongoDB Integration** — Connects to MongoDB Atlas, loads road records, and supports saving road-condition updates.
- **Weather API Integration** — Uses the Open-Meteo API through libcurl to retrieve temperature, precipitation, wind speed, and weather code.
- **Weather Status Calculation** — Classifies conditions using configured rainfall and wind-speed thresholds.
- **Build Configuration** — CMake configures the C++ application and its MongoDB and CURL dependencies.
- **Menu-Driven Interface** — Provides options to display the road network, find routes, update road conditions, test MongoDB connectivity, and check live weather.
- **End-to-End Testing** — Initial functional tests have been performed; broader testing remains necessary.
- **Frontend and Map Visualization** — Planned future development.

---

## Technology Stack

| Technology | Purpose |
|---|---|
| C++ | Core application and routing logic |
| Graphs and adjacency lists | Represent the road network |
| Dijkstra's algorithm | Calculate minimum-cost routes |
| Priority queue | Select the next lowest-cost vertex |
| MongoDB Atlas | Store and retrieve road records |
| MongoDB C++ Driver | Connect the C++ application to MongoDB |
| Open-Meteo API | Retrieve weather information |
| libcurl | Make HTTP requests to the weather API |
| CMake | Configure and build the application |

---

## System Architecture & Execution Pipeline

### 1. Application Input

The user interacts with the menu-driven C++ application to display the network, request an evacuation route, update a road condition, test MongoDB connectivity, or retrieve weather information.

### 2. Road Network Data

Locations are represented as graph vertices, while roads are represented as weighted edges. The graph uses an adjacency list to store connections.

Road records are loaded from the MongoDB `roads` collection when database configuration and connectivity are available. A demonstration network is used when road records are not successfully loaded.

### 3. Route Optimization

Dijkstra's algorithm works with a priority queue to find a minimum-cost path between the requested source and destination.

The result includes the selected route and its calculated total cost. The route is based on the edge weights and road conditions represented in the graph.

### 4. Road-Condition Management

The Disaster Manager supports road-condition updates using the following statuses:

- **Safe**
- **Risky**
- **Blocked**
- **Congested**

Updates can be saved to MongoDB, allowing road information to persist beyond the current application session.

### 5. Weather API Integration

The API client sends an HTTP request to Open-Meteo using latitude and longitude coordinates. The response supplies current weather information, including:

- Temperature in degrees Celsius
- Precipitation in millimetres
- Wind speed in kilometres per hour
- Weather code

The application parses these values and calculates a weather status using configured thresholds.

### 6. Route Recalculation

The route optimizer can calculate routes using the graph's current edge costs. After road conditions are updated, the application can be tested to determine whether the in-memory graph reflects those changes correctly and whether the new route is appropriate.

**Current integration boundary:** Weather data retrieval and weather-status calculation are working, but automatic propagation of weather results into MongoDB road records and automatic weather-driven rerouting are not yet established as completed features.

---

## MongoDB Integration

MongoDB Atlas is used as the persistent data store for road information.

### Database Structure

- **Database:** `evacuation_db`
- **Collection:** `roads`

Road documents use fields such as:

- `fromLocation` — Starting location ID
- `toLocation` — Destination location ID
- `distance` — Road distance or base cost
- `status` — Road-condition status

### Implemented Functionality

- MongoDB connectivity testing.
- Retrieval of road documents from the collection.
- Conversion of database road records into graph edges.
- Representation of two-way roads using forward and reverse edges.
- Support for road-condition updates through the Disaster Manager and database manager.
- Persistence of road updates for subsequent retrieval.

Database credentials and connection strings are supplied through environment variables, rather than hard-coded into the application source code.

The application uses `MONGODB_URI` and `MONGODB_DATABASE` for database configuration.

---

## Weather API Integration

The application integrates the Open-Meteo forecast API using libcurl.

### Weather Data Flow

1. The user selects the weather API option.
2. The application requests latitude and longitude coordinates.
3. The API client sends an HTTP request to Open-Meteo.
4. The response is parsed to extract current weather fields.
5. The application displays the weather information and calculated status.

### Weather Status Rules

The current implementation calculates status from rainfall and wind speed:

| Condition | Calculated status |
|---|---|
| Rainfall ≥ 10 mm or wind ≥ 60 km/h | Blocked |
| Rainfall ≥ 5 mm or wind ≥ 40 km/h | Congested |
| Rainfall ≥ 2 mm or wind ≥ 25 km/h | Risky |
| Otherwise | Safe |

The application checks the most severe threshold first. These are configurable demonstration rules and should not be interpreted as official emergency classifications.

The API has been tested successfully with coordinates for Meerut, and it returned non-zero weather values. Additional testing across different locations and weather conditions is recommended.

---

## Data Structures and Algorithms

### Graph Representation

The road network is represented using an adjacency list. Each location stores its neighboring locations, making traversal efficient for a relatively sparse road network.

### Dijkstra's Algorithm

Dijkstra's algorithm finds minimum-cost paths when edge weights are non-negative. It is appropriate for this application because the route optimizer works with weighted road connections.

### Priority Queue

A min-priority queue allows the algorithm to process the vertex with the smallest tentative route cost first.

### Dynamic Road Costs

Road-condition information can affect the cost or availability of a road. Risky or congested roads can be assigned higher costs, while blocked roads should be excluded from usable routes.

The correctness of the final route depends on ensuring that the in-memory graph and route optimizer consistently enforce these rules.

---

## Running the Application

From the project root in GitHub Codespaces, build the project:

```bash
cmake --build build -j2
```

Load the environment variables and run the application:

```bash
set -a && source .env && set +a && ./build/evacuation_app
```

### Menu Options

1. Display road network
2. Find evacuation route
3. Update road condition and save to MongoDB
4. Test MongoDB connection
5. Check live weather API
6. Exit

For the weather option, enter the latitude and longitude of the location to check.

**Security note:** Keep `.env` out of version control and never publish database credentials.

---

## Testing Status

The following functionality has been exercised during development:

- CMake build and application startup.
- MongoDB Atlas connection.
- Road-data loading and graph display.
- Evacuation-route calculation.
- Road-condition update workflow.
- Open-Meteo API requests and weather-value parsing.
- Weather status calculation.

Further validation should cover database write failures, invalid inputs, unreachable destinations, blocked-road behavior, API timeouts, and route correctness after road updates.

---

## Current Limitations

- No graphical frontend or interactive map visualization.
- Live traffic and authoritative road-closure feeds are not integrated.
- Weather status is not yet automatically connected to road updates and route recalculation.
- The configured weather thresholds require domain-specific validation.
- More extensive integration and regression testing is needed.
- The demonstration road network and available road records limit the realism of the current routing model.

---

## Future Scope

- Connect weather results to road-risk updates and route recalculation.
- Integrate live traffic, road closures, and disaster-related data.
- Add designated evacuation centres, hospitals, and safe zones.
- Develop a map-based frontend to display road networks and routes.
- Improve route validation, error handling, and automated tests.
- Scale the routing model to larger real-world road networks.
- Evaluate route quality using realistic emergency scenarios.

---

## Project Status

The project currently has a working C++ routing application with graph-based route optimization, a priority queue, MongoDB connectivity and road-data integration, road-condition update functionality, and a tested Open-Meteo weather API client.

The next major step is to connect live weather information to road-risk updates and automatic route recalculation, followed by comprehensive testing and frontend/map development.

The system provides a functional foundation for an adaptive emergency evacuation-routing application, with further work required before it can be considered a complete real-world emergency navigation solution.