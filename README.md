# Adaptive Emergency Evacuation Route Optimization

A C++ and React-based emergency evacuation-routing prototype for Dehradun. The C++ backend loads road data from OpenStreetMap, represents the network as a weighted graph, and uses Dijkstra's algorithm with a priority queue to calculate routes. A React/Vite frontend provides a dashboard, interactive map, location search, and route-planning interface.

## Project Features

### C++ Backend
- Road network represented using an adjacency-list graph.
- Dehradun road network loaded from `backend/data/dehradun-roads.osm`.
- OpenStreetMap XML parsing, including supported highway types and one-way road handling.
- Dijkstra-based route optimization using a priority queue.
- Coordinate-based routing by snapping start and destination coordinates to nearby graph nodes.
- HTTP server running on port `8080`.
- Health-check, road-network, and route API endpoints.

### React Frontend
- React, TypeScript, and Vite dashboard.
- Interactive map using Leaflet and OpenStreetMap tiles.
- Dehradun location search using OpenStreetMap geocoding.
- Route planner for selecting starting and destination locations.
- Dashboard panels for road conditions, weather, and emergency alerts.

### Technology Stack

| Technology | Purpose |
|---|---|
| C++ | Backend and routing logic |
| Graph / adjacency list | Road-network representation |
| Dijkstra's algorithm | Minimum-cost route search |
| Priority queue | Efficient route search |
| OpenStreetMap | Road-network and map data |
| TinyXML2 | OSM XML parsing |
| cpp-httplib | C++ HTTP server |
| React | Frontend interface |
| TypeScript | Frontend application code |
| Vite | Frontend development server |
| Leaflet | Interactive map |
| MongoDB | Road-data persistence where configured |
| CMake | Backend build configuration |

## System Architecture

```text
Browser
  |
React + Vite Frontend
  |
frontend/src/services/api.ts
  |
HTTP Requests
  |
C++ HTTP Server :8080
  |-- GET  /api/health
  |-- GET  /api/roads
  |-- POST /api/route
  |
Route Optimizer
  |
Dijkstra's Algorithm + Priority Queue
  |
Road Graph
  |
Dehradun OpenStreetMap Dataset
```

## Road Network and Routing

The expected road-data file is:

`backend/data/dehradun-roads.osm`

The OSM loader processes supported highway types, reads node coordinates, and creates graph edges. One-way roads and roundabouts are handled by the loader.

The graph uses vertices for road-network nodes and weighted edges for road connections. Dijkstra's algorithm finds a minimum-cost route when edge weights are non-negative.

For coordinate-based requests, the backend maps the supplied start and destination coordinates to nearby graph nodes before calculating a route. The resulting route may not begin or end at the exact selected coordinates.

## Backend API

The C++ HTTP server listens on port `8080`.

| Method | Endpoint | Purpose |
|---|---|---|
| GET | `/api/health` | Backend health check |
| GET | `/api/roads` | Returns road-edge data |
| POST | `/api/route` | Calculates a route from supplied coordinates |
| OPTIONS | API paths | Handles browser CORS preflight requests |

### Health Check

`GET /api/health`

Example response:

```json
{"status":"ok"}
```

### Route Request

`POST /api/route`

Example request body (coordinates are `[latitude, longitude]`):

```json
{
  "startId": "start",
  "destinationId": "destination",
  "startCoordinates": [30.325, 78.041],
  "destinationCoordinates": [30.316, 78.032]
}
```

The route response includes calculated distance, estimated duration, route coordinates, and route details.

**Integration status:** The frontend also references `/api/weather` and `/api/alerts`, but these endpoints are not currently registered in the C++ HTTP server. Their backend integration remains to be completed.

## Running the Project in GitHub Codespaces

Use separate terminals for the backend and frontend.

### 1. Build the Backend

From the repository root:

```bash
cmake -S backend -B backend/build
cmake --build backend/build -j2
```

Ensure the required C++ dependencies and the Dehradun OSM data file are available.

### 2. Start the Backend

```bash
./backend/build/evacuation_app
```

Keep this terminal running. The backend listens on port `8080`.

### 3. Configure the Frontend

Create or edit `frontend/.env`:

```env
VITE_API_BASE_URL=https://YOUR-CODESPACE-NAME-8080.app.github.dev
```

Replace the example with the actual forwarded HTTPS URL shown for port `8080` in the Codespaces Ports panel. Do not add a trailing slash.

Restart the Vite server after changing the environment file.

### 4. Start the Frontend

Open a second terminal:

```bash
cd frontend
npm install
npm run dev -- --host 0.0.0.0
```

Open the forwarded frontend port, usually `5173`, in the browser.

### 5. Verify the Backend

```bash
curl -i http://127.0.0.1:8080/api/health
```

Expected response:

```json
{"status":"ok"}
```

### Ports

| Port | Service |
|---|---|
| `5173` | Vite frontend |
| `8080` | C++ backend |

The backend port must be reachable from the browser serving the frontend. Use the forwarded backend URL in `frontend/.env`.

## Configuration and Security

- Keep local `.env` files out of version control.
- Never commit database credentials, tokens, or private connection strings.
- Do not expose MongoDB credentials through frontend environment variables.
- Use a sanitized `.env.example` with placeholder values if configuration documentation is needed.
- Check `git status` before committing to ensure `node_modules`, build outputs, and private environment files are excluded.

## Current Limitations

- Coordinate snapping approximates selected locations using nearby graph nodes.
- Route quality depends on the completeness and freshness of the OSM dataset.
- Live traffic, verified road closures, and authoritative emergency feeds are not integrated.
- The C++ HTTP server does not yet expose `/api/weather` or `/api/alerts`.
- Weather data is not yet proven to update graph edge costs and trigger automatic rerouting end to end.
- The `/api/roads` response can be large for a detailed road graph; filtering or pagination may be needed.
- The prototype has not been validated for operational emergency response and must not replace official emergency instructions.

## Future Improvements

- Implement backend weather and emergency-alert endpoints.
- Integrate verified road closures, hazard reports, and live traffic data.
- Apply validated hazard and road-condition weights to graph edges.
- Recalculate routes automatically when road conditions change.
- Improve coordinate snapping and exact coordinate-to-road connections.
- Optimize road-data delivery with filtering and pagination.
- Add automated tests for OSM loading, route correctness, blocked roads, invalid coordinates, and unreachable destinations.
- Improve deployment documentation and secure configuration management.

