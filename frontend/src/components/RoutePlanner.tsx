import { useState } from 'react'
import type { FormEvent } from 'react'
import { ArrowDown, MapPin, Navigation, RotateCcw, Search } from 'lucide-react'
import type { Location, RouteResult } from '../types'
import { LOCATIONS } from '../data/locations'

interface SearchResult {
  place_id: number | string
  display_name: string
  lat: string
  lon: string
}

interface RoutePlannerProps {
  startId: string
  destinationId: string
  route: RouteResult | null
  loading: boolean
  error: string
  onStartChange: (id: string) => void
  onDestinationChange: (id: string) => void
  onFind: () => void
  onClear: () => void
  onStartLocationSelect?: (location: Location) => void
  onDestinationLocationSelect?: (location: Location) => void
}

function LocationSelect({
  label,
  value,
  onChange,
  onLocationSelect,
}: {
  label: string
  value: string
  onChange: (id: string) => void
  onLocationSelect?: (location: Location) => void
}) {
  const [query, setQuery] = useState('')
  const [results, setResults] = useState<SearchResult[]>([])
  const [selectedLocation, setSelectedLocation] = useState<Location | null>(null)
  const [searching, setSearching] = useState(false)
  const [error, setError] = useState('')

  async function handleSearch(event: FormEvent<HTMLFormElement>) {
    event.preventDefault()

    if (query.trim().length < 3) {
      setError('Enter at least 3 characters.')
      setResults([])
      return
    }

    setSearching(true)
    setError('')
    setResults([])

    try {
      const params = new URLSearchParams({
        q: `${query.trim()}, Dehradun, Uttarakhand, India`,
        format: 'jsonv2',
        limit: '6',
        countrycodes: 'in',
        viewbox: '77.80,30.48,78.20,30.18',
        bounded: '1',
      })

      const response = await fetch(
        `https://nominatim.openstreetmap.org/search?${params}`,
        { headers: { Accept: 'application/json' } },
      )

      if (!response.ok) throw new Error('Search request failed')

      const data: SearchResult[] = await response.json()

      const localResults = data.filter((item) => {
        const lat = Number(item.lat)
        const lon = Number(item.lon)

        return (
          Number.isFinite(lat) &&
          Number.isFinite(lon) &&
          lat >= 30.18 &&
          lat <= 30.48 &&
          lon >= 77.8 &&
          lon <= 78.2
        )
      })

      setResults(localResults)

      if (localResults.length === 0) {
        setError('No matching place found in Dehradun.')
      }
    } catch {
      setError('Search failed. Check your internet and try again.')
    } finally {
      setSearching(false)
    }
  }

  function chooseResult(result: SearchResult) {
    const location: Location = {
      id: `search-${result.place_id}`,
      name: result.display_name.split(',')[0],
      address: result.display_name,
      coordinates: [Number(result.lat), Number(result.lon)],
    }

    setSelectedLocation(location)
    onChange(location.id)
    onLocationSelect?.(location)
    setQuery(location.name)
    setResults([])
    setError('')
  }

  function chooseLandmark(id: string) {
    setSelectedLocation(null)
    onChange(id)

    const location = LOCATIONS.find((item) => item.id === id)
    if (location) onLocationSelect?.(location)
  }

  return (
    <div className="field">
      <label>
        <span>{label}</span>
      </label>

      <form
        onSubmit={(event) => void handleSearch(event)}
        style={{
          display: 'flex',
          gap: 6,
          marginTop: 8,
          marginBottom: 8,
        }}
      >
        <input
          value={query}
          onChange={(event) => setQuery(event.target.value)}
          placeholder="Search a place in Dehradun..."
          aria-label={`Search ${label.toLowerCase()}`}
          style={{
            flex: 1,
            minWidth: 0,
            padding: '10px',
            borderRadius: 9,
            border: '1px solid #203947',
            background: '#0b1b25',
            color: '#e8f5fb',
          }}
        />

        <button
          type="submit"
          className="secondary-button"
          disabled={searching}
          aria-label={`Search ${label.toLowerCase()}`}
        >
          <Search size={16} />
          {searching ? 'Searching' : 'Search'}
        </button>
      </form>

      {error && (
        <p className="form-message" role="status">
          {error}
        </p>
      )}

      {results.length > 0 && (
        <div
          style={{
            border: '1px solid #203947',
            borderRadius: 9,
            overflow: 'hidden',
            marginBottom: 8,
          }}
        >
          {results.map((result) => (
            <button
              key={result.place_id}
              type="button"
              onClick={() => chooseResult(result)}
              style={{
                display: 'flex',
                alignItems: 'flex-start',
                gap: 8,
                width: '100%',
                padding: 10,
                border: 'none',
                borderBottom: '1px solid #203947',
                background: '#0b1b25',
                color: '#e8f5fb',
                textAlign: 'left',
                cursor: 'pointer',
              }}
            >
              <MapPin size={16} />
              <span>
                <strong>{result.display_name.split(',')[0]}</strong>
                <br />
                <small>{result.display_name}</small>
              </span>
            </button>
          ))}
        </div>
      )}

      <select
        value={
          LOCATIONS.some((location) => location.id === value)
            ? value
            : selectedLocation?.id === value
              ? value
              : ''
        }
        onChange={(event) => chooseLandmark(event.target.value)}
      >
        <option value="">Select a landmark</option>

        {selectedLocation && selectedLocation.id === value && (
          <option value={selectedLocation.id}>
            {selectedLocation.name}
          </option>
        )}

        {LOCATIONS.map((location) => (
          <option value={location.id} key={location.id}>
            {location.name}
          </option>
        ))}
      </select>

      <small style={{ display: 'block', marginTop: 5, opacity: 0.7 }}>
        Search results © OpenStreetMap contributors
      </small>
    </div>
  )
}

export default function RoutePlanner(props: RoutePlannerProps) {
  return (
    <section className="panel planner-panel">
      <div className="panel-heading">
        <div>
          <span className="eyebrow">Emergency navigation</span>
          <h2>Plan evacuation route</h2>
        </div>
        <Navigation size={20} />
      </div>

      <div className="route-fields">
        <span className="route-line">
          <i />
          <i />
        </span>

        <LocationSelect
          label="Starting location"
          value={props.startId}
          onChange={props.onStartChange}
          onLocationSelect={props.onStartLocationSelect}
        />

        <ArrowDown className="route-arrow" size={16} />

        <LocationSelect
          label="Destination"
          value={props.destinationId}
          onChange={props.onDestinationChange}
          onLocationSelect={props.onDestinationLocationSelect}
        />
      </div>

      {props.error && <p className="form-message">{props.error}</p>}

      <div className="button-row">
        <button
          className="primary-button"
          onClick={props.onFind}
          disabled={props.loading}
        >
          <Navigation size={17} />
          {props.loading ? 'Calculating…' : 'Find route'}
        </button>

        <button className="secondary-button" onClick={props.onClear}>
          <RotateCcw size={16} />
          Clear
        </button>
      </div>

      {props.route && (
        <div className="route-summary">
          <div>
            <strong>{props.route.distanceKm.toFixed(1)} km</strong>
            <span>Distance</span>
          </div>

          <div>
            <strong>{props.route.durationMinutes} min</strong>
            <span>Est. time</span>
          </div>

          <div className="route-detail">
            <p>{props.route.details}</p>
          </div>
        </div>
      )}
    </section>
  )
}