import { useEffect } from 'react'
import { divIcon } from 'leaflet'
import { MapContainer, Marker, Polyline, Popup, TileLayer, useMap } from 'react-leaflet'
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

function MapFocus({ location, reducedMotion }: { location?: Location; reducedMotion?: boolean }) {
  const map = useMap()
  useEffect(() => {
    if (location) map.flyTo(location.coordinates, 14, { animate: !reducedMotion, duration: reducedMotion ? 0 : 0.8 })
  }, [location, map, reducedMotion])
  return null
}

interface EmergencyMapProps {
  route: RouteResult | null
  focusedLocation?: Location
  userLocation: [number, number] | null
  onSelect: (location: Location) => void
  reducedMotion?: boolean
}

export default function EmergencyMap({ route, focusedLocation, userLocation, onSelect, reducedMotion }: EmergencyMapProps) {
  return (
    <MapContainer center={DEHRADUN_CENTER} zoom={13} zoomControl className="map">
      <TileLayer
        attribution='&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors &copy; <a href="https://carto.com/attributions">CARTO</a>'
        url="https://{s}.basemaps.cartocdn.com/dark_all/{z}/{x}/{y}{r}.png"
        maxZoom={20}
      />
      <MapFocus location={focusedLocation} reducedMotion={reducedMotion} />
      {LOCATIONS.map((location) => (
        <Marker
          key={location.id}
          position={location.coordinates}
          icon={landmarkIcon}
          eventHandlers={{ click: () => onSelect(location) }}
        >
          <Popup><strong>{location.name}</strong><br />{location.address}<br /><small>Select for route planning</small></Popup>
        </Marker>
      ))}
      {userLocation && <Marker position={userLocation} icon={userIcon}><Popup>Your current location</Popup></Marker>}
      {route && <Polyline positions={route.coordinates} pathOptions={{ color: '#15d3ee', weight: 6, opacity: 0.9 }} />}
    </MapContainer>
  )
}
