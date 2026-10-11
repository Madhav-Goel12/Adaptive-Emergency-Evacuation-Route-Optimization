# Adaptive Emergency Evacuation Route Optimization

An intelligent emergency response navigation system built in C++ that dynamically calculates safe evacuation routes during disasters (floods, fires, and road blockages) using graph theory and shortest-path algorithms.

---

##  Project Overview

During severe emergencies, standard GPS routes can become hazardous or impassable due to sudden road damage, active hazard zones, or traffic jams. Traditional static routing systems fail to adapt to changing conditions.

This project models a city road network as a **dynamic weighted graph**. It computes evacuation routes using **Dijkstra's Algorithm combined with a Priority Queue**. When road conditions change, edge weights are updated to help recalculate safer alternate paths.

---

##  Project Progress & Compilation Status

**Overall Backend Progress: Approximately 70–75%**

The core backend modules and route optimization logic have been developed. The remaining work mainly involves database integration, testing, error handling, and frontend development.

### Implementation Checklist

- [x] **Location Module** — Represents locations in the road network.
- [x] **Road Module** — Represents roads and their associated properties.
- [x] **Graph Module** — Represents the road network using an adjacency list.
- [x] **Dijkstra's Algorithm** — Calculates shortest paths using weighted edges.
- [x] **Priority Queue** — Supports efficient selection of the next node during route calculation.
- [x] **Route Optimization Logic** — Determines routes based on road weights.
- [x] **Disaster Manager** — Handles changes in road conditions, including safe, risky, congested, and blocked roads.
- [x] **Weather API Integration** — Weather-related integration has been developed; live API reliability and end-to-end behaviour require testing.
- [ ] **MongoDB Integration** — Complete database connectivity and data persistence.
- [ ] **End-to-End Testing** — Test route calculations, changing road conditions, and alternate routes.
- [ ] **Error Handling** — Improve handling of invalid inputs, API failures, and unavailable routes.
- [ ] **Frontend Development** — Build the user interface for entering locations and viewing routes.
- [ ] **Map Visualization** — Display the road network and calculated evacuation routes visually.
- [ ] **Final Integration and Demonstration** — Validate the complete system.

**Compilation Note:** The core backend modules have been developed. A fresh build is required to confirm that the complete project compiles successfully together. Individual implementation completion does not necessarily mean every module has passed compilation and integration testing.

---

##  System Architecture & Execution Pipeline

### [1] USER INPUT
- Receives the origin node (start location).
- Receives the destination node (evacuation centre or safe zone).

↓

### [2] ROAD NETWORK DATA
- Represents locations as graph nodes using `Location` objects.
- Represents connections as edges using `Road` objects.
- Assigns weights based on road properties and conditions.

↓

### [3] DIJKSTRA'S ALGORITHM + PRIORITY QUEUE
- Evaluates paths using a priority queue.
- Calculates the minimum-cost route based on the current edge weights.

↓

### [4] INITIAL BEST ROUTE
- Establishes the initial evacuation route.
- Route output is handled by the backend; a graphical route display is planned for the frontend.

↓

### [5] ROAD CONDITION UPDATE
- Handles changes such as road blockages, flooding, or increased risk.
- Updates the affected road's status or associated cost.

↓

### [6] ALTERNATE ROUTE RECALCULATION
- Recalculates the route using the updated graph weights.
- Finds an alternate path when a suitable route is available.

---

##  Key Features

- **Dynamic Edge Weighting:** Adjusts route costs according to road conditions, risk levels, and blockage status.
- **Safety Centre Routing:** Supports routing towards designated evacuation zones and medical centres.
- **Alternate Route Calculation:** Recalculates routes when road conditions change.
- **Weather Integration:** Incorporates weather-related information through API integration.
- **Object-Oriented Design:** Uses modular C++ classes for maintainable code.
- **Modular Backend:** Separates graph representation, road data, location data, disaster management, and route optimization.

---

## 🛠️ Data Structures & Algorithms Used

- **Graph Representation:** Adjacency list for storing locations and road connections.
- **Priority Queue / Min-Heap:** Helps select the next node with the lowest tentative cost.
- **Dijkstra's Algorithm:** Computes minimum-cost paths for non-negative edge weights.
- **Object-Oriented Programming:** Uses classes such as `Location`, `Road`, `Graph`, and `RouteOptimizer`.
- **Dynamic Edge Weights:** Represents changing road conditions through adjusted route costs.

---

##  Current Limitations

- The frontend and interactive map view have not yet been implemented.
- Live traffic and real-time road blockage data are not yet fully integrated.
- MongoDB connectivity and persistence need to be completed and validated.
- Full end-to-end testing and final integration are pending.

---

##  Future Scope

- Complete MongoDB integration for storing locations, roads, weather information, emergencies, and routes.
- Develop an interactive frontend with map-based route visualization.
- Integrate more reliable live weather, traffic, and road-condition data.
- Improve error handling, route validation, and system testing.
- Extend the system to support larger road networks and additional emergency scenarios.

---

##  Project Status

The project is currently focused on completing and validating the backend. The main graph structures and route optimization components have been developed, with approximately **70–75% of the backend work completed**. Database integration, comprehensive testing, frontend development, and map visualization remain part of the upcoming work.
