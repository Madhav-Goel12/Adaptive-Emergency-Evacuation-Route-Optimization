Adaptive Emergency Evacuation Route Optimization

A C++ and React-based emergency evacuation-routing prototype for Dehradun, India. The application loads road-network data from OpenStreetMap (OSM), models the network as a weighted graph, and uses Dijkstra's algorithm to calculate a minimum-cost route between two selected locations.

The React dashboard displays the selected route on an interactive map alongside weather information, road conditions, and route-specific explanations of risky or blocked alternatives.

Features

C++ Backend

- Road network represented using an adjacency-list graph.
- Dehradun road data loaded from "backend/data/dehradun-roads.osm".
- OSM XML parsing with supported highway types and one-way road handling.
- Coordinate-based routing by snapping selected coordinates to nearby graph nodes.
- Dijkstra's algorithm with a priority queue to find a minimum-cost available route.
- Blocked road edges excluded from route calculation.
- Road-condition classification based on blocked status and hazard-adjusted cost.
- Route-specific alternative-road analysis, returning relevant risky, congested, or blocked road segments with explanations.
- HTTP API served on port "8080".
- Open-Meteo weather integration.
- Optional MongoDB persistence when configured.

React Frontend

- React, TypeScript, and Vite dashboard.
- Interactive Leaflet map using OpenStreetMap tiles.
- Dehradun location search using OpenStreetMap geocoding.
- Route planner for selecting starting and destination locations.
- Calculated route displayed on the interactive map.
- Dashboard panels for weather, road conditions, and emergency alerts.
- Emergency alerts explain why relevant alternative road segments were not selected for the currently calculated origin and destination.

Project Development History: MongoDB and OpenStreetMap

MongoDB was used in the project through Phase 2. Following the Phase 2 review and the feedback received from evaluators, the project was extended to use OpenStreetMap (OSM) road-network data and a graph-based implementation for route calculation.

The OSM graph is now used by the routing system, while the MongoDB integration has been retained in the project and has not been removed. MongoDB remains available for persistence functionality where configured.

Technology Stack

Technology| Purpose
C++| Backend and routing logic
Graph / Adjacency List| Road-network representation
Dijkstra's Algorithm| Minimum-cost route search
Priority Queue| Efficient shortest-path search
OpenStreetMap (OSM)| Road-network and map data
TinyXML2| OSM XML parsing
cpp-httplib| C++ HTTP server
libcurl| Open-Meteo HTTP requests
Open-Meteo| Weather data
MongoDB C++ Driver| Optional data persistence
React| Frontend interface
TypeScript| Frontend application code
Vite| Frontend development server
Leaflet| Interactive map
CMake| Backend build configuration

System Architecture

                   Browser
                      |
                      v
               React + Vite
                  Frontend
                      |
                      v
          frontend/src/services/api.ts
                      |
                HTTP Requests
                      |
                      v
              C++ HTTP Server
                 Port 8080
                      |
          +-----------+-----------+
          |           |           |
          v           v           v
       /api/       /api/       /api/
       roads       route       weather
          |
          v
     Route and Road-Condition Analysis
                      |
                      v
          Dijkstra's Algorithm
              + Priority Queue
                      |
                      v
             Weighted Road Graph
                      |
                      v
          Dehradun OpenStreetMap Data

External Services:

- Weather Service: Open-Meteo
- Optional Persistence: MongoDB

Road Network and Routing

The expected OSM data file is:

backend/data/dehradun-roads.osm

The OSM loader reads supported road types, node coordinates, and road-direction information, then creates graph edges. One-way roads and roundabouts are handled according to the loader's supported OSM tags.

Graph vertices represent road-network nodes. Edges represent traversable road connections and carry a travel-time/cost weight.

Dijkstra's algorithm calculates a minimum-cost path when edge weights are non-negative.

Route Calculation Workflow

For a route request, the backend:

1. Receives the selected start and destination coordinates.
2. Finds nearby graph nodes to represent those coordinates.
3. Runs Dijkstra's algorithm over the available graph.
4. Returns route coordinates, distance, and estimated duration.
5. Analyses other road segments in the origin-destination corridor and returns relevant risky, congested, or blocked alternatives with explanations.

Note: The snapped graph nodes may differ slightly from the exact coordinates selected in the frontend.

Road Risk and Emergency Alert Logic

Road status is derived from the blocked flag and the relationship between adjusted cost and base travel time.

The current backend classification is:

Condition| Status
Road's blocked flag is set| Blocked
Adjusted cost is at least 2.5 times base time| Congested
Adjusted cost is at least 1.5 times base time| Risky
Otherwise| Safe

The route planner excludes blocked edges from the available route graph.

Risky and congested edges can remain traversable, but their higher cost can make another route preferable.

Emergency Alerts

The Emergency Alerts panel is route-specific. It uses the currently selected origin and destination to identify alternative road segments rather than simply listing every risky road in Dehradun.

Each returned alternative includes:

- Road name
- Road status
- Reason for classification
- Map coordinates

These alerts are algorithmic assessments based on the data loaded into the prototype. They are not live, authoritative reports of accidents, closures, or danger.

Their quality depends on the OSM data, road weights, and alternative-road relevance filter.

Backend API

The C++ HTTP server listens on port "8080".

Method| Endpoint| Purpose
GET| "/api/health"| Backend health check
GET| "/api/roads"| Returns road-network edge data and road conditions
GET| "/api/weather"| Returns current weather data from the configured weather integration
GET| "/api/alerts"| Returns road-hazard alert data
POST| "/api/route"| Calculates a route and returns route-specific avoided alternatives
OPTIONS| API paths| Handles browser CORS preflight requests

Health Check

Request:

GET /api/health

Example Response:

{
  "status": "ok"
}

Route Request

Request:

POST /api/route

Example Request Body:

Coordinates are provided in "[latitude, longitude]" format.

{
  "startId": "start",
  "destinationId": "destination",
  "startCoordinates": [30.325, 78.041],
  "destinationCoordinates": [30.316, 78.032]
}

The route response includes:

- Calculated distance
- Estimated duration
- Route coordinates
- Route details
- An "avoidedRoads" collection containing relevant alternative road segments and explanations

Exact response fields are defined in "backend/src/HttpServer.cpp".

Testing the Backend

Once the backend is running, the following commands can be used to test the API endpoints:

curl http://127.0.0.1:8080/api/health

curl http://127.0.0.1:8080/api/weather

curl http://127.0.0.1:8080/api/alerts

curl http://127.0.0.1:8080/api/roads

Running the Project in GitHub Codespaces

Use separate terminals for the backend and frontend.

1. Prerequisites

The Codespace requires:

- C++17-compatible compiler
- CMake
- Git
- TinyXML2
- cpp-httplib
- libcurl
- Node.js
- npm
- Dehradun OSM dataset
- MongoDB C++ Driver (if MongoDB integration is enabled)

On Ubuntu-based Codespaces, common system packages can be installed using:

sudo apt update

sudo apt install -y \
  build-essential \
  cmake \
  git \
  pkg-config \
  curl \
  libssl-dev \
  libcurl4-openssl-dev \
  libtinyxml2-dev \
  libcpp-httplib-dev

Install the MongoDB C++ Driver separately if the backend is configured to use MongoDB and the driver is not already available.

2. Build the Backend

From the repository root:

cmake -S backend -B backend/build

cmake --build backend/build -j2

Ensure that the following file exists:

backend/data/dehradun-roads.osm

3. Start the Backend

Run the executable from the backend directory so that the relative path "data/dehradun-roads.osm" resolves correctly.

cd backend

./build/evacuation_app

Keep this terminal running.

The backend listens on port "8080".

4. Configure the Frontend API URL

Create or edit the frontend environment file:

frontend/.env

Add the following configuration:

VITE_API_BASE_URL=https://YOUR-CODESPACE-8080.app.github.dev

Replace the placeholder with the actual forwarded HTTPS URL for port "8080" displayed in the GitHub Codespaces Ports panel.

Important: Do not add a trailing slash.

If the frontend uses the Vite "/api" development proxy instead, configure "frontend/vite.config.ts" to proxy "/api" to "http://localhost:8080" and use the API base URL expected by "frontend/src/services/api.ts".

Restart Vite after changing ".env".

5. Start the Frontend

Open a second terminal from the repository root:

cd frontend

npm install

npm run dev -- --host 0.0.0.0

Open the forwarded frontend port, usually "5173", in the browser.

6. Verify the Backend

Run:

curl -i http://127.0.0.1:8080/api/health

Expected response:

{
  "status": "ok"
}

Application Ports

Port| Service
5173| Vite Frontend
8080| C++ Backend

The frontend must reach the backend through the Codespaces forwarded backend URL or the Vite development proxy.

Configuration and Security

- Keep local ".env" files out of version control.
- Never commit database credentials, access tokens, or private connection strings.
- Do not expose MongoDB credentials through frontend environment variables.
- Use a sanitized ".env.example" with placeholders when documenting configuration.
- Check "git status" before committing to ensure private environment files, "node_modules", and build outputs are excluded.
- Restrict production CORS origins and use suitable authentication and transport security before deploying beyond a development environment.

Current Limitations

1. Coordinate Snapping: Coordinate snapping approximates selected locations using nearby graph nodes. The route may not start or end at the exact selected coordinates.

2. OSM Data Dependency: Route quality depends on the completeness, correctness, and freshness of the OSM dataset.

3. Road-Risk Classification: Road-risk classifications depend on configured cost and blocked-status data. They are not independently verified hazard reports.

4. Live Traffic and Emergency Feeds: Live traffic, verified road closures, and authoritative emergency feeds are not integrated.

5. Weather-Based Recalculation: Weather data is retrieved from Open-Meteo, but the application should not be assumed to update every graph edge or reroute automatically in response to weather unless that behaviour is explicitly implemented and tested.

6. Alternative-Road Filtering: Alternative-road results depend on the backend's corridor and relevance filtering and should be validated against known origin-destination test cases.

7. Large Road-Network Responses: The "/api/roads" response can be large for a detailed road graph. Filtering or pagination may be needed.

8. Optional MongoDB Persistence: MongoDB persistence depends on valid environment configuration and an available MongoDB deployment.

9. Prototype Validation: The prototype has not been validated for operational emergency response.

Testing and Validation

The project was tested through backend build verification, API testing, and manual frontend testing in GitHub Codespaces.

1. Backend Build Testing

- Successfully configured and compiled the C++ backend using CMake.
- Verified that the backend executable was generated successfully.
- Checked that the application could load the Dehradun OpenStreetMap dataset and initialize the road graph.

2. API Testing

The backend endpoints were tested using "curl" to verify their responses:

Endpoint| Testing Performed
"/api/health"| Checked backend availability
"/api/roads"| Verified that road-network data was returned
"/api/weather"| Verified weather data retrieval from Open-Meteo
"/api/alerts"| Checked the emergency-alert response
"/api/route"| Tested route calculation using selected starting and destination coordinates

3. Frontend and Integration Testing

- Verified that the React frontend successfully connected to the C++ backend.
- Tested location search and selection of starting and destination points.
- Checked that calculated routes were displayed on the interactive Leaflet map.
- Verified the display of route distance and estimated travel time.
- Checked weather information and connection status on the dashboard.
- Tested route-specific emergency alerts to display relevant risky or blocked alternative road segments and their explanations.
- Verified that road-condition information was displayed in the dashboard.

Future Improvements

- Integrate verified road closures, hazard reports, and live traffic data.
- Calibrate road-risk weights using reliable data and documented thresholds.
- Improve alternative-route comparison so each alert corresponds to a demonstrably relevant alternative for the selected origin and destination.
- Recalculate routes when verified road conditions change.
- Improve coordinate snapping and exact coordinate-to-road connections.
- Add automated tests for OSM loading, route correctness, risky and blocked roads, invalid coordinates, unreachable destinations, and route-specific alerts.
- Add filtering or pagination for large road-network responses.
- Improve deployment configuration, monitoring, and security.

---

Project: Adaptive Emergency Evacuation Route Optimization
Location: Dehradun, India
Backend: C++17
Frontend: React + TypeScript + Vite
Routing Algorithm: Dijkstra's Algorithm
Road Network Data: OpenStreetMap (OSM)
Weather Integration: Open-Meteo
Database: MongoDB (Optional)e traffic, verified road closures, and authoritative emergency feeds are not integrated.
Weather data is retrieved from Open-Meteo, but the application should not be assumed to update every graph edge or reroute automatically in response to weather unless that behaviour is explicitly implemented and tested.
Alternative-road results depend on the backend's corridor and relevance filtering and should be validated against known origin-destination test cases.
The /api/roads response can be large for a detailed road graph; filtering or pagination may be needed.
MongoDB persistence is optional and depends on valid environment configuration and an available MongoDB deployment.
The prototype has not been validated for operational emergency response.
Future improvements
Integrate verified road closures, hazard reports, and live traffic data.
Calibrate road-risk weights using reliable data and documented thresholds.
Improve alternative-route comparison so each alert corresponds to a demonstrably relevant alternative for the selected origin and destination.
Recalculate routes when verified road conditions change.
Improve coordinate snapping and exact coordinate-to-road connections.
Add automated tests for OSM loading, route correctness, risky and blocked roads, invalid coordinates, unreachable destinations, and route-specific alerts.
Add filtering or pagination for large road-network responses.
Improve deployment configuration, monitoring, and security.
Testing and Validation
The project was tested through backend build verification, API testing, and manual frontend testing in GitHub Codespaces.
1. Backend Build Testing
Successfully configured and compiled the C++ backend using CMake.
Verified that the backend executable was generated successfully.
Checked that the application could load the Dehradun OpenStreetMap dataset and initialize the road graph.
2. API Testing
The backend endpoints were tested using curl to verify their responses:
/api/health – Checked backend availability.
/api/roads – Verified that road-network data was returned.
/api/weather – Verified weather data retrieval from Open-Meteo.
/api/alerts – Checked the emergency-alert response.
/api/route – Tested route calculation using selected starting and destination coordinates.
3. Frontend and Integration Testing
Verified that the React frontend successfully connected to the C++ backend.
Tested location search and selection of starting and destination points.
Checked that calculated routes were displayed on the interactive Leaflet map.
Verified the display of route distance and estimated travel time.
Checked weather information and connection status on the dashboard.
Tested route-specific emergency alerts to display relevant risky or blocked alternative road segments and their explanations.
Verified that road-condition information was displayed in the dashboard.
