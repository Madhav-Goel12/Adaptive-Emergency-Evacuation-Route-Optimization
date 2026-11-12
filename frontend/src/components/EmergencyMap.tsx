import { useEffect, useState } from 'react'
import type { FormEvent } from 'react'
import { divIcon } from 'leaflet'
import {
  MapContainer,
  Marker,
  Polyline,
  Popup,
  TileLayer,
  useMap,
} from 'react-leaflet'
import 'leaflet/dist/leaflet.css'

import type { Location, RouteResult } from '../types'
import { DEHRADUN_CENTER, LOCATIONS } from '../data/locations'

const landmarkIcon = divIcon({
  className: 'custom-map-marker',
  html: '<span></span>',
  iconSize: [24, 24],
  iconAnchor: [12, 12],
})

const userIcon = divIcon({
  className: 'user-map-marker',
  html: '<span></span>',
  iconSize: [28, 28],
  iconAnchor: [14, 14],
})

interface WeatherData {
  temperature: number
  humidity: number
  rain: number
  windSpeed: number
  weatherCode: number
  time: string
}

interface WeatherResponse {
  current: {
    temperature_2m: number
    relative_humidity_2m: number
    precipitation: number
    wind_speed_10m: number
    weather_code: number
    time: string
  }
}

interface SearchResult {
  place_id: number | string
  display_name: string
  lat: string
  lon: string
}

function MapFocus({
  location,
  reducedMotion,
}: {
  location?: Location
  reducedMotion?: boolean
}) {
  const map = useMap()

  useEffect(() => {
    if (!location) return

    map.flyTo(location.coordinates, 14, {
      animate: !reducedMotion,
      duration: reducedMotion ? 0 : 0.8,
    })
  }, [location, map, reducedMotion])

  return null
}

function SearchMapFocus({
  position,
}: {
  position: [number, number] | null
}) {
  const map = useMap()

  useEffect(() => {
    if (!position) return

    map.flyTo(position, 16, {
      animate: true,
      duration: 0.8,
    })
  }, [position, map])

  return null
}

function WeatherOverlay() {
  const [weather, setWeather] = useState<WeatherData | null>(null)
  const [error, setError] = useState(false)

  useEffect(() => {
    let active = true

    async function fetchWeather() {
      try {
        const url =
          'https://api.open-meteo.com/v1/forecast' +
          '?latitude=30.3165' +
          '&longitude=78.0322' +
          '&current=temperature_2m,relative_humidity_2m,precipitation,wind_speed_10m,weather_code' +
          '&timezone=Asia%2FKolkata'

        const response = await fetch(url)

        if (!response.ok) {
          throw new Error('Weather request failed')
        }

        const data: WeatherResponse = await response.json()
        const current = data.current

        if (active) {
          setWeather({
            temperature: current.temperature_2m,
            humidity: current.relative_humidity_2m,
            rain: current.precipitation,
            windSpeed: current.wind_speed_10m,
            weatherCode: current.weather_code,
            time: current.time,
          })

          setError(false)
        }
      } catch {
        if (active) setError(true)
      }
    }

    void fetchWeather()

    const timer = window.setInterval(() => {
      void fetchWeather()
    }, 10 * 60 * 1000)

    return () => {
      active = false
      window.clearInterval(timer)
    }
  }, [])

  return (
    <div
      style={{
        position: 'absolute',
        top: 12,
        right: 12,
        zIndex: 1000,
        width: 205,
        boxSizing: 'border-box',
        padding: 14,
        borderRadius: 12,
        background: 'rgba(8, 23, 34, 0.95)',
        border: '1px solid #203947',
        color: '#e8f5fb',
        fontSize: 13,
        lineHeight: 1.8,
        boxShadow: '0 4px 16px rgba(0,0,0,0.25)',
      }}
    >
      <div
        style={{
          color: '#15d3ee',
          fontWeight: 700,
          marginBottom: 5,
        }}
      >
        DEHRADUN WEATHER
      </div>

      {weather ? (
        <>
          <div style={{ fontSize: 26, fontWeight: 700 }}>
            {weather.temperature}°C
          </div>

          <div>Humidity: {weather.humidity}%</div>
          <div>Precipitation: {weather.rain} mm</div>
          <div>Wind: {weather.windSpeed} km/h</div>
          <div>Weather code: {weather.weatherCode}</div>

          <div
            style={{
              color: '#91a9b8',
              fontSize: 11,
              marginTop: 5,
            }}
          >
            Updated: {weather.time} IST
          </div>

          <div style={{ color: '#34d399', marginTop: 3 }}>
            ● Open-Meteo connected
          </div>
        </>
      ) : (
        <div>
          {error
            ? 'Weather unavailable. Retrying in 10 minutes.'
            : 'Loading Dehradun weather...'}
        </div>
      )}
    </div>
  )
}

function LocationSearch({
  onChoose,
}: {
  onChoose: (location: Location) => void
}) {
  const [searchQuery, setSearchQuery] = useState('')
  const [searchResults, setSearchResults] = useState<SearchResult[]>([])
  const [searching, setSearching] = useState(false)
  const [searchError, setSearchError] = useState('')

  async function handleSearch(event: FormEvent<HTMLFormElement>) {
    event.preventDefault()

    const query = searchQuery.trim()

    if (query.length < 3) {
      setSearchError('Enter at least 3 characters.')
      setSearchResults([])
      return
    }

    setSearching(true)
    setSearchError('')
    setSearchResults([])

    try {
      const params = new URLSearchParams({
        q: `${query}, Dehradun, Uttarakhand, India`,
        format: 'jsonv2',
        addressdetails: '1',
        limit: '6',
        countrycodes: 'in',
        viewbox: '77.80,30.48,78.20,30.18',
        bounded: '1',
      })

      const response = await fetch(
        `https://nominatim.openstreetmap.org/search?${params}`,
        {
          headers: {
            Accept: 'application/json',
          },
        },
      )

      if (!response.ok) {
        throw new Error('Location search failed')
      }

      const results: SearchResult[] = await response.json()

      // Keep results inside the approximate Dehradun search area.
      const localResults = results.filter((result) => {
        const lat = Number(result.lat)
        const lon = Number(result.lon)

        return (
          Number.isFinite(lat) &&
          Number.isFinite(lon) &&
          lat >= 30.18 &&
          lat <= 30.48 &&
          lon >= 77.8 &&
          lon <= 78.2
        )
      })

      setSearchResults(localResults)

      if (localResults.length === 0) {
        setSearchError(
          'No matching places found in Dehradun. Try another name.',
        )
      }
    } catch {
      setSearchError(
        'Unable to search right now. Check your internet and try again.',
      )
    } finally {
      setSearching(false)
    }
  }

  function selectSearchResult(result: SearchResult) {
    const lat = Number(result.lat)
    const lon = Number(result.lon)

    const location: Location = {
      id: `search-${result.place_id}`,
      name: result.display_name.split(',')[0],
      address: result.display_name,
      coordinates: [lat, lon],
    }

    onChoose(location)
    setSearchQuery(location.name)
    setSearchResults([])
    setSearchError('')
  }

  return (
    <div
      style={{
        position: 'absolute',
        top: 12,
        left: 12,
        zIndex: 1000,
        width: 'min(340px, calc(100% - 250px))',
        minWidth: 200,
        maxHeight: '65%',
        overflowY: 'auto',
        boxSizing: 'border-box',
        padding: 12,
        borderRadius: 12,
        background: 'rgba(8, 23, 34, 0.96)',
        border: '1px solid #203947',
        color: '#e8f5fb',
        boxShadow: '0 4px 16px rgba(0,0,0,0.25)',
      }}
    >
      <div
        style={{
          color: '#15d3ee',
          fontWeight: 700,
          fontSize: 13,
          marginBottom: 8,
        }}
      >
        FIND A LOCATION
      </div>

      <form
        onSubmit={(event) => void handleSearch(event)}
        style={{
          display: 'flex',
          gap: 6,
        }}
      >
        <input
          type="search"
          value={searchQuery}
          onChange={(event) => setSearchQuery(event.target.value)}
          placeholder="Hospital, road, landmark..."
          aria-label="Search for a place in Dehradun"
          style={{
            flex: 1,
            minWidth: 0,
            padding: '10px 9px',
            borderRadius: 7,
            border: '1px solid #34505e',
            background: '#102b39',
            color: '#ffffff',
            outline: 'none',
            fontSize: 12,
          }}
        />

        <button
          type="submit"
          disabled={searching}
          style={{
            padding: '9px 11px',
            border: 'none',
            borderRadius: 7,
            background: '#15d3ee',
            color: '#08212d',
            fontWeight: 700,
            cursor: searching ? 'wait' : 'pointer',
            opacity: searching ? 0.7 : 1,
          }}
        >
          {searching ? '...' : 'Search'}
        </button>
      </form>

      {searchError && (
        <div
          role="status"
          style={{
            marginTop: 9,
            color: '#fca5a5',
            fontSize: 12,
            lineHeight: 1.5,
          }}
        >
          {searchError}
        </div>
      )}

      {searchResults.length > 0 && (
        <div style={{ marginTop: 9 }}>
          {searchResults.map((result) => (
            <button
              key={result.place_id}
              type="button"
              onClick={() => selectSearchResult(result)}
              style={{
                display: 'block',
                width: '100%',
                padding: '10px 8px',
                marginBottom: 5,
                textAlign: 'left',
                borderRadius: 7,
                border: '1px solid #203947',
                background: '#102b39',
                color: '#e8f5fb',
                cursor: 'pointer',
                overflowWrap: 'anywhere',
              }}
            >
              <div style={{ fontWeight: 600, fontSize: 12 }}>
                {result.display_name.split(',')[0]}
              </div>

              <div
                style={{
                  color: '#91a9b8',
                  fontSize: 11,
                  marginTop: 3,
                  lineHeight: 1.4,
                }}
              >
                {result.display_name}
              </div>
            </button>
          ))}
        </div>
      )}

      <div
        style={{
          marginTop: 8,
          color: '#91a9b8',
          fontSize: 10,
        }}
      >
        Search data © OpenStreetMap contributors
      </div>
    </div>
  )
}

interface EmergencyMapProps {
  route: RouteResult | null
  focusedLocation?: Location
  userLocation: [number, number] | null
  onSelect: (location: Location) => void
  reducedMotion?: boolean
}

export default function EmergencyMap({
  route,
  focusedLocation,
  userLocation,
  onSelect,
  reducedMotion,
}: EmergencyMapProps) {
  const [selectedSearch, setSelectedSearch] = useState<Location | null>(null)

  function handleSearchSelect(location: Location) {
    setSelectedSearch(location)
    onSelect(location)
  }

  return (
    <MapContainer
      center={DEHRADUN_CENTER}
      zoom={13}
      zoomControl
      className="map"
    >
      <TileLayer
        attribution='&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
        url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
        maxZoom={19}
      />

      <LocationSearch onChoose={handleSearchSelect} />

      <MapFocus
        location={focusedLocation}
        reducedMotion={reducedMotion}
      />

      <SearchMapFocus
        position={selectedSearch?.coordinates ?? null}
      />

      {LOCATIONS.map((location) => (
        <Marker
          key={location.id}
          position={location.coordinates}
          icon={landmarkIcon}
          eventHandlers={{
            click: () => onSelect(location),
          }}
        >
          <Popup>
            <strong>{location.name}</strong>
            <br />
            {location.address}
            <br />
            <small>Select for route planning</small>
          </Popup>
        </Marker>
      ))}

      {selectedSearch && (
        <Marker
          key={selectedSearch.id}
          position={selectedSearch.coordinates}
          icon={landmarkIcon}
        >
          <Popup>
            <strong>{selectedSearch.name}</strong>
            <br />
            {selectedSearch.address}
            <br />
            <small>Search result selected</small>
          </Popup>
        </Marker>
      )}

      {userLocation && (
        <Marker position={userLocation} icon={userIcon}>
          <Popup>Your current location</Popup>
        </Marker>
      )}

      {route && route.coordinates.length > 1 && (
        <Polyline
          positions={route.coordinates}
          pathOptions={{
            color: '#15d3ee',
            weight: 6,
            opacity: 0.9,
          }}
        />
      )}

      <WeatherOverlay />
    </MapContainer>
  )
}