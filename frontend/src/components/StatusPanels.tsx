import { AlertCircle, CloudSun, Eye, ShieldCheck } from 'lucide-react'
import type { Alert, Road, Weather } from '../types'

const statusLabels = { safe: 'Safe', risky: 'Risky', congested: 'Congested', blocked: 'Blocked' }

interface DataPanelProps<T> {
  data: T | null
  loading: boolean
}

export function RoadStatusPanel({ data: roads, loading }: DataPanelProps<Road[]>) {
  return (
    <section className="panel compact-panel">
      <div className="panel-heading"><div><span className="eyebrow">Network overview</span><h2>Road conditions</h2></div></div>
      <div className="road-list">
        {loading && <p className="data-state">Loading road conditions…</p>}
        {!loading && roads === null && <p className="data-state">Road conditions unavailable</p>}
        {!loading && roads?.length === 0 && <p className="data-state">No road conditions reported</p>}
        {roads?.map((road) => <div className="road-row" key={road.id}><span className={`status-dot ${road.status}`} /><strong>{road.name}</strong><span className={`status-text ${road.status}`}>{statusLabels[road.status]}</span></div>)}
      </div>
      {roads && roads.length > 0 && <p className="safety-note"><ShieldCheck size={14} />Status is informational only; verify with local authorities.</p>}
    </section>
  )
}

export function WeatherPanel({ data: weather, loading }: DataPanelProps<Weather>) {
  return (
    <section className="panel info-card">
      <div className="info-icon weather"><CloudSun size={22} /></div>
      <div className="info-copy">
        <span>Weather conditions</span>
        <strong>{loading ? 'Loading weather…' : weather?.condition ?? 'Weather data unavailable'}</strong>
        {weather?.visibility && <p><Eye size={13} /> Visibility {weather.visibility}</p>}
      </div>
    </section>
  )
}

export function AlertPanel({ data: alerts, loading }: DataPanelProps<Alert[]>) {
  const alert = alerts?.[0]
  return (
    <section className="panel info-card">
      <div className="info-icon alert"><AlertCircle size={22} /></div>
      <div className="info-copy">
        <span>Emergency alerts</span>
        <strong>{loading ? 'Checking for alerts…' : alerts === null ? 'Alert data unavailable' : alert?.title ?? 'No active emergency alerts'}</strong>
        {alert?.description && <p>{alert.description}</p>}
      </div>
    </section>
  )
}
