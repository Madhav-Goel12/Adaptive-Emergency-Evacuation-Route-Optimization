import { ArrowDown, Navigation, RotateCcw } from 'lucide-react'
import type { Location, RouteResult } from '../types'
import { LOCATIONS } from '../data/locations'

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
}

function LocationSelect({ label, value, onChange }: { label: string; value: string; onChange: (id: string) => void }) {
  return (
    <label className="field">
      <span>{label}</span>
      <select value={value} onChange={(event) => onChange(event.target.value)}>
        <option value="">Select a landmark</option>
        {LOCATIONS.map((location: Location) => <option value={location.id} key={location.id}>{location.name}</option>)}
      </select>
    </label>
  )
}

export default function RoutePlanner(props: RoutePlannerProps) {
  return (
    <section className="panel planner-panel">
      <div className="panel-heading">
        <div><span className="eyebrow">Emergency navigation</span><h2>Plan evacuation route</h2></div>
        <Navigation size={20} />
      </div>
      <div className="route-fields">
        <span className="route-line"><i /><i /></span>
        <LocationSelect label="Starting location" value={props.startId} onChange={props.onStartChange} />
        <ArrowDown className="route-arrow" size={16} />
        <LocationSelect label="Destination" value={props.destinationId} onChange={props.onDestinationChange} />
      </div>
      {props.error && <p className="form-message">{props.error}</p>}
      <div className="button-row">
        <button className="primary-button" onClick={props.onFind} disabled={props.loading}>
          <Navigation size={17} />{props.loading ? 'Calculating…' : 'Find route'}
        </button>
        <button className="secondary-button" onClick={props.onClear}><RotateCcw size={16} />Clear</button>
      </div>
      {props.route && (
        <div className="route-summary">
          <div><strong>{props.route.distanceKm.toFixed(1)} km</strong><span>Distance</span></div>
          <div><strong>{props.route.durationMinutes} min</strong><span>Est. time</span></div>
          <div className="route-detail"><p>{props.route.details}</p></div>
        </div>
      )}
    </section>
  )
}
