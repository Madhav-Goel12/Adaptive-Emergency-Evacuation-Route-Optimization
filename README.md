# Adaptive Emergency Evacuation Route Optimization

A C++ and React-based emergency evacuation-routing prototype for Dehradun, India. The application loads road-network data from OpenStreetMap (OSM), represents the network as a weighted graph, and uses Dijkstra's algorithm to calculate a minimum-cost route between two selected locations.

The React dashboard displays the selected route on an interactive map, together with weather information, road conditions, and route-specific explanations of risky or blocked alternative road segments.

## Features

### C++ Backend

- Represents the road network using an adjacency-list graph.
- Loads Dehradun road data from `backend/data/dehradun-roads.osm`.
- Parses OSM XML, including supported highway types and one-way road information.
- Snaps selected coordinates to nearby graph nodes for route calculation.
- Uses Dijkstra's algorithm and a priority queue to find a minimum-cost available route.
- Excludes blocked road edges from route calculation.
- Classifies road conditions using blocked status and hazard-adjusted cost.
- Analyses alternative road segments and returns relevant risky, congested, or blocked segments with explanations.
- Provides an HTTP API on port `8080`.
- Retrieves weather information through Open-Meteo.
- Retains optional MongoDB persistence when configured.

### React Frontend

- Built with React, TypeScript, and Vite.
- Displays an interactive Leaflet map using OpenStreetMap tiles.
- Supports Dehradun location search using OpenStreetMap geocoding.
- Allows users to select starting and destination locations.
- Displays the calculated route on the map.
- Shows weather, road conditions, and emergency-alert information.
- Presents explanations for relevant alternative road segments for the currently selected route.

## Project Development History: MongoDB and OpenStreetMap

MongoDB was used in the project through Phase 2. Following the Phase 2 review and feedback from evaluators, the project was extended to use OpenStreetMap (OSM) road-network data and a graph-based implementation for route calculation.

The OSM graph is now used by the routing system. The MongoDB integration has been retained and has not been removed; it remains available for persistence functionality when configured.

## Technology Stack

| Technology | Purpose |
|---|---|
| C++17 | Backend and routing logic |
| Graph / Adjacency List | Road-network representation |
| Dijkstra's Algorithm | Minimum-cost route search |
| Priority Queue | Efficient shortest-path search |
| OpenStreetMap (OSM) | Road-network and map data |
| TinyXML2 | OSM XML parsing |
| cpp-httplib | C++ HTTP server |
| libcurl | HTTP requests for weather data |
| Open-Meteo | Weather information |
| MongoDB C++ Driver | Optional data persistence |
| React | Frontend interface |
| TypeScript | Frontend application code |
| Vite | Frontend development server |
| Leaflet | Interactive map |
| CMake | Backend build configuration |

## System Architecture

```text
                  Browser
                     |
                     v
               React + Vite
                  Frontend
                     |
                     v
          frontend/src/services/api.ts
                     |
                     v
                HTTP Requests
                     |
                     v
              C++ HTTP Server
                 Port 8080
                     |
          +----------+----------+
          |          |          |
          v          v          v
       /api/       /api/       /api/
       roads       route      weather
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
```

**External services**

- **Weather:** Open-Meteo
- **Optional persistence:** MongoDB

## Road Network and Routing

The expected OSM data file is:

```text
backend/data/dehradun-roads.osm
```

The OSM loader reads supported road types, node coordinates, and road-direction information, then creates graph edges. One-way roads and roundabouts are handled according to the OSM tags supported by the loader.

Graph vertices represent road-network nodes. Edges represent traversable road connections and carry a travel-time/cost weight. Dijkstra's algorithm calculates a minimum-cost path using non-negative edge weights.

### Route Calculation Workflow

For each route request, the backend:

1. Receives the selected start and destination coordinates.
2. Finds nearby graph nodes to represent those coordinates.
3. Runs Dijkstra's algorithm over the available graph.
4. Returns route coordinates, distance, and estimated duration.
5. Analyses other road segments in the origin-to-destination corridor and returns relevant risky, congested, or blocked alternatives with explanations.

**Note:** The graph nodes selected by coordinate snapping may differ slightly from the exact coordinates selected in the frontend.

## Road Risk and Emergency Alert Logic

Road status is derived from the blocked flag and the relationship between adjusted cost and base travel time.

| Condition | Status |
|---|---|
| The road's blocked flag is set | Blocked |
| Adjusted cost is at least 2.5 times the base time | Congested |
| Adjusted cost is at least 1.5 times the base time | Risky |
| None of the above | Safe |

Blocked edges are excluded from route calculation. Risky and congested edges may remain traversable, but their higher costs can make another route preferable.

### Emergency Alerts

The Emergency Alerts panel uses the currently selected origin and destination to identify alternative road segments. It is intended to show route-relevant alternatives rather than simply listing every risky road in Dehradun.

Each returned alternative can include:

- Road name
- Road status
- Reason for its classification
- Map coordinates

These alerts are algorithmic assessments based on the data loaded into the prototype. They are not live or authoritative reports of accidents, closures, or danger. Their relevance depends on the OSM data, road weights, and the backend's alternative-road filtering.

## Backend API

The C++ HTTP server listens on port `8080`.

| Method | Endpoint | Purpose |
|---|---|---|
| `GET` | `/api/health` | Checks backend availability |
| `GET` | `/api/roads` | Returns road-network edge data and road conditions |
| `GET` | `/api/weather` | Returns weather data from the configured integration |
| `GET` | `/api/alerts` | Returns road-hazard alert data |
| `POST` | `/api/route` | Calculates a route and returns route-specific avoided alternatives |
| `OPTIONS` | API paths | Handles browser CORS preflight requests |

### Health Check

**Request**

```http
GET /api/health
```

**Example response**

```json
{
  "status": "ok"
}
```

### Route Request

**Request**

```http
POST /api/route
```

**Example request body**

Coordinates use the `[latitude, longitude]` format.

```json
{
  "startId": "start",
  "destinationId": "destination",
  "startCoordinates": [30.325, 78.041],
  "destinationCoordinates": [30.316, 78.032]
}
```

The route response includes the calculated distance, estimated duration, route coordinates, route details, and an `avoidedRoads` collection containing alternative road segments and explanations.

Exact response fields are defined in `backend/src/HttpServer.cpp`.

## Testing and Validation

The project was checked through backend build verification, API checks, and manual testing of the frontend in GitHub Codespaces.

### Backend Build

- Configured and built the C++ backend using CMake.
- Verified that the backend executable was generated.
- Checked that the application loaded the Dehradun OSM dataset and initialized the road graph.

### API Checks

The following endpoints were checked using `curl`:

| Endpoint | Check |
|---|---|
| `/api/health` | Confirmed that the backend health endpoint responded |
| `/api/roads` | Checked that road-network data was returned |
| `/api/weather` | Checked the weather response from the Open-Meteo integration |
| `/api/alerts` | Checked the emergency-alert response |
| `/api/route` | Checked route calculation using selected start and destination coordinates |

Example commands:

```bash
curl http://127.0.0.1:8080/api/health
curl http://127.0.0.1:8080/api/roads
curl http://127.0.0.1:8080/api/weather
curl http://127.0.0.1:8080/api/alerts
```

### Frontend and Integration Checks

- Checked that the React frontend connected to the C++ backend.
- Tested searching for and selecting starting and destination locations.
- Checked that the calculated route appeared on the Leaflet map.
- Checked the displayed route distance and estimated travel time.
- Checked the weather information and backend connection status on the dashboard.
- Checked the road-condition information and route-specific emergency-alert panel.

### Testing Scope

Testing was performed manually in the development environment. A comprehensive automated unit-test and integration-test suite has not yet been added. Additional testing is needed for cases such as unreachable destinations, different blocked-road configurations, and the accuracy of alternative-road suggestions across a wider range of origin and destination pairs.

## Running the Project in GitHub Codespaces

Use separate terminals for the backend and frontend.

### 1. Prerequisites

The Codespace requires:

- A C++17-compatible compiler
- CMake
- Git
- TinyXML2
- cpp-httplib
- libcurl
- Node.js and npm
- The Dehradun OSM dataset
- MongoDB C++ Driver, if MongoDB integration is enabled

On Ubuntu-based Codespaces, common system packages can be installed with:

```bash
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
```

Install the MongoDB C++ Driver separately if the backend is configured to use MongoDB and the driver is not already available.

### 2. Build the Backend

Run these commands from the repository root:

```bash
cmake -S backend -B backend/build
cmake --build backend/build -j2
```

Ensure that this dataset exists:

```text
backend/data/dehradun-roads.osm
```

### 3. Start the Backend

Run the executable from the `backend` directory so that the relative path `data/dehradun-roads.osm` resolves correctly:

```bash
cd backend
./build/evacuation_app
```

Keep this terminal running. The backend listens on port `8080`.

### 4. Configure the Frontend API URL

Create or edit:

```text
frontend/.env
```

Add the forwarded HTTPS URL for backend port `8080`:

```env
VITE_API_BASE_URL=https://YOUR-CODESPACE-8080.app.github.dev
```

Replace the placeholder with the actual forwarded URL shown in the GitHub Codespaces Ports panel. Do not add a trailing slash.

If the frontend uses the Vite `/api` development proxy instead, configure `frontend/vite.config.ts` to proxy `/api` to `http://localhost:8080` and use the API base URL expected by `frontend/src/services/api.ts`.

Restart the Vite development server after changing `.env`.

### 5. Start the Frontend

Open a second terminal from the repository root:

```bash
cd frontend
npm install
npm run dev -- --host 0.0.0.0
```

Open the forwarded frontend port, usually `5173`, in the browser.

### 6. Verify the Backend

With the backend running, use:

```bash
curl -i http://127.0.0.1:8080/api/health
```

Expected response:

```json
{
  "status": "ok"
}
```

## Application Ports

| Port | Service |
|---|---|
| `5173` | Vite frontend |
| `8080` | C++ backend |

The frontend must reach the backend through the Codespaces forwarded backend URL or the Vite development proxy.

## Configuration and Security

- Keep local `.env` files out of version control.
- Never commit database credentials, access tokens, or private connection strings.
- Do not expose MongoDB credentials through frontend environment variables.
- Use a sanitized `.env.example` with placeholder values when documenting configuration.
- Check `git status` before committing to ensure that private environment files, `node_modules`, and build outputs are excluded.
- Restrict production CORS origins and use suitable authentication and transport security before deploying beyond a development environment.

## Current Limitations

1. **Coordinate snapping:** Selected locations are approximated using nearby graph nodes. A route may not start or end at the exact coordinates selected in the frontend.
2. **OSM data dependency:** Route quality depends on the completeness, correctness, and freshness of the OSM dataset.
3. **Road-risk classification:** Risk classifications depend on configured cost and blocked-status data. They are not independently verified hazard reports.
4. **Live traffic and emergency feeds:** Live traffic, verified road closures, and authoritative emergency feeds are not integrated.
5. **Weather-based recalculation:** Weather data is retrieved from Open-Meteo. The application should not be assumed to update every graph edge or reroute automatically in response to weather unless that behaviour is explicitly implemented and tested.
6. **Alternative-road filtering:** Alternative-road results depend on the backend's corridor and relevance filtering. Their accuracy should be validated against known origin-to-destination test cases.
7. **Large road-network responses:** The `/api/roads` response can be large for a detailed road graph. Filtering or pagination may be needed.
8. **Optional MongoDB persistence:** MongoDB persistence depends on valid environment configuration and an available MongoDB deployment.
9. **Prototype status:** The application has not been validated for operational emergency response.

## Future Improvements

- Integrate verified road closures, hazard reports, and live traffic data.
- Calibrate road-risk weights using reliable data and documented thresholds.
- Improve alternative-route comparison so each alert corresponds to a demonstrably relevant alternative for the selected origin and destination.
- Recalculate routes when verified road conditions change.
- Improve coordinate snapping and exact coordinate-to-road connections.
- Add automated tests for OSM loading, route correctness, risky and blocked roads, invalid coordinates, unreachable destinations, and route-specific alerts.
- Add filtering or pagination for large road-network responses.
- Improve deployment configuration, monitoring, and security.

---

**Project:** Adaptive Emergency Evacuation Route Optimization  
**Location:** Dehradun, India  
**Backend:** C++17  
**Frontend:** React, TypeScript, and Vite  
**Routing algorithm:** Dijkstra's Algorithm  
**Road-network data:** OpenStreetMap (OSM)  
**Weather integration:** Open-Meteo  
**Database:** MongoDB (optional)
