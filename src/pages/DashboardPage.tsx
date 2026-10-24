import { useEffect, useMemo, useState } from 'react'
import { LocateFixed, Menu, Search, Wifi } from 'lucide-react'
import EmergencyMap from '../components/EmergencyMap'
import RoutePlanner from '../components/RoutePlanner'
import Sidebar, { type PageId } from '../components/Sidebar'
import SettingsPanel from '../components/SettingsPanel'
import { AlertPanel, RoadStatusPanel, WeatherPanel } from '../components/StatusPanels'
import { LOCATIONS } from '../data/locations'
import { api } from '../services/api'
import type { Alert, Location, Road, RouteResult, Weather } from '../types'

export default function DashboardPage() {
  const [sidebarOpen, setSidebarOpen] = useState(false)
  const [activePage, setActivePage] = useState<PageId>('dashboard')
  const [startId, setStartId] = useState('')
  const [destinationId, setDestinationId] = useState('')
  const [route, setRoute] = useState<RouteResult | null>(null)
  const [routeLoading, setRouteLoading] = useState(false)
  const [routeError, setRouteError] = useState('')
  const [roads, setRoads] = useState<Road[] | null>(null)
  const [weather, setWeather] = useState<Weather | null>(null)
  const [alerts, setAlerts] = useState<Alert[] | null>(null)
  const [loadingData, setLoadingData] = useState({ roads: true, weather: true, alerts: true })
  const [query, setQuery] = useState('')
  const [focusedLocation, setFocusedLocation] = useState<Location>()
  const [userLocation, setUserLocation] = useState<[number, number] | null>(null)
  const [locationError, setLocationError] = useState('')
  const [reducedMotion, setReducedMotion] = useState(() => localStorage.getItem('aero-reduced-motion') === 'true')
  const [compactMode, setCompactMode] = useState(() => localStorage.getItem('aero-compact-mode') === 'true')

  useEffect(() => {
    api.getRoads().then(setRoads).catch(() => setRoads(null)).finally(() => setLoadingData((s) => ({ ...s, roads: false })))
    api.getWeather().then(setWeather).catch(() => setWeather(null)).finally(() => setLoadingData((s) => ({ ...s, weather: false })))
    api.getAlerts().then(setAlerts).catch(() => setAlerts(null)).finally(() => setLoadingData((s) => ({ ...s, alerts: false })))
  }, [])

  useEffect(() => {
    localStorage.setItem('aero-reduced-motion', String(reducedMotion))
    document.body.classList.toggle('reduce-motion', reducedMotion)
  }, [reducedMotion])

  useEffect(() => {
    localStorage.setItem('aero-compact-mode', String(compactMode))
    document.body.classList.toggle('compact-mode', compactMode)
  }, [compactMode])

  const searchMatches = useMemo(() => query.length > 1
    ? LOCATIONS.filter((location) => location.name.toLowerCase().includes(query.toLowerCase()))
    : [], [query])

  function selectMapLocation(location: Location) {
    setFocusedLocation(location)
    setQuery(location.name)
    if (!startId) setStartId(location.id)
    else if (location.id !== startId) setDestinationId(location.id)
  }

  function findCurrentLocation() {
    setLocationError('')
    if (!navigator.geolocation) return setLocationError('Geolocation is unavailable in this browser.')
    navigator.geolocation.getCurrentPosition(
      ({ coords }) => setUserLocation([coords.latitude, coords.longitude]),
      () => setLocationError('Location permission was denied or unavailable.'),
      { enableHighAccuracy: true, timeout: 8000 },
    )
  }

  async function findRoute() {
    setRouteError('')
    if (!startId || !destinationId) return setRouteError('Select both a starting point and destination.')
    if (startId === destinationId) return setRouteError('Choose two different locations.')
    setRouteLoading(true)
    try {
      setRoute(await api.getRoute(startId, destinationId))
    } catch {
      setRoute(null)
      setRouteError('Route calculation unavailable. Check the backend connection and try again.')
    } finally {
      setRouteLoading(false)
    }
  }

  const mapCard = (large = false) => (
    <section className={`map-card ${large ? 'map-card-large' : ''}`}>
      <EmergencyMap route={route} focusedLocation={focusedLocation} userLocation={userLocation} onSelect={selectMapLocation} reducedMotion={reducedMotion} />
    </section>
  )

  const planner = (
    <RoutePlanner startId={startId} destinationId={destinationId} route={route} loading={routeLoading} error={routeError} onStartChange={setStartId} onDestinationChange={setDestinationId} onFind={findRoute} onClear={() => { setStartId(''); setDestinationId(''); setRoute(null); setRouteError('') }} />
  )

  function renderPage() {
    if (activePage === 'map') {
      return <div className="single-view">{mapCard(true)}{locationError && <p className="location-error">{locationError}</p>}</div>
    }
    if (activePage === 'route') {
      return <div className="dashboard route-view"><div className="map-column">{mapCard(true)}{locationError && <p className="location-error">{locationError}</p>}</div><aside className="right-column">{planner}</aside></div>
    }
    if (activePage === 'roads') {
      return <div className="detail-view"><RoadStatusPanel data={roads} loading={loadingData.roads} /></div>
    }
    if (activePage === 'weather') {
      return <div className="detail-view"><div className="info-grid"><WeatherPanel data={weather} loading={loadingData.weather} /><AlertPanel data={alerts} loading={loadingData.alerts} /></div></div>
    }
    if (activePage === 'settings') {
      return <div className="detail-view"><SettingsPanel reducedMotion={reducedMotion} compactMode={compactMode} onReducedMotionChange={setReducedMotion} onCompactModeChange={setCompactMode} onReset={() => { setReducedMotion(false); setCompactMode(false) }} /></div>
    }
    return (
      <div className="dashboard">
        <div className="map-column">
          {mapCard()}
          {locationError && <p className="location-error">{locationError}</p>}
          <div className="info-grid">
            <WeatherPanel data={weather} loading={loadingData.weather} />
            <AlertPanel data={alerts} loading={loadingData.alerts} />
          </div>
        </div>
        <aside className="right-column">
          {planner}
          <RoadStatusPanel data={roads} loading={loadingData.roads} />
        </aside>
      </div>
    )
  }

  return (
    <div className="app-shell">
      <Sidebar open={sidebarOpen} activePage={activePage} onNavigate={setActivePage} onClose={() => setSidebarOpen(false)} />
      <main className="main-area">
        <header className="topbar">
          <button className="icon-button menu-button" onClick={() => setSidebarOpen(true)} aria-label="Open navigation"><Menu size={21} /></button>
          <div className="page-title"><span>{activePage === 'dashboard' ? 'Dehradun emergency operations' : activePage.replace('-', ' ')}</span><h1>Adaptive Emergency Evacuation Route Optimization</h1></div>
          <div className="search-wrap">
            <Search size={18} />
            <input value={query} onChange={(event) => setQuery(event.target.value)} placeholder="Search Dehradun landmarks..." aria-label="Search locations" />
            {searchMatches.length > 0 && <div className="search-results">{searchMatches.map((location) => <button key={location.id} onClick={() => selectMapLocation(location)}><strong>{location.name}</strong><span>{location.address}</span></button>)}</div>}
          </div>
          <button className="location-button" onClick={findCurrentLocation}><LocateFixed size={17} /><span>Current location</span></button>
          <div className="freshness"><Wifi size={15} /><div><strong>Map available</strong><span>Live map tiles</span></div></div>
        </header>

        {renderPage()}
      </main>
    </div>
  )
}
