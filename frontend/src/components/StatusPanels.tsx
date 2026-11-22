import { AlertTriangle, CloudSun, Route, ShieldAlert } from 'lucide-react'
import type { Alert, Road, RouteResult, Weather } from '../types'

export function WeatherPanel({ data, loading }: { data: Weather | null; loading: boolean }) {
  return (
    <section className="panel info-card">
      <div className="info-icon weather"><CloudSun size={23} /></div>
      <div className="info-copy">
        <span>Weather conditions · Dehradun</span>
        {loading ? <strong>Loading weather…</strong> : data?.temperature != null ? (
          <>
            <strong>{data.temperature}°C</strong>
            <p>{data.condition ?? 'Current conditions'}{data.humidity != null ? ` · Humidity ${data.humidity}%` : ''}</p>
            {data.precipitation != null && <p>Precipitation: {data.precipitation} mm</p>}
            {data.windSpeed != null && <p>Wind: {data.windSpeed} km/h</p>}
            {data.updatedAt && <p>Updated: {data.updatedAt}</p>}
          </>
        ) : <strong>Weather data unavailable</strong>}
      </div>
    </section>
  )
}

export function AlertPanel({ data: _data, route, loading }: { data: Alert[] | null; route: RouteResult | null; loading: boolean }) {
  const avoided = route?.avoidedRoads ?? []
  return (
    <section className="panel info-card" style={{ alignItems: 'flex-start' }}>
      <div className="info-icon alert"><AlertTriangle size={23} /></div>
      <div className="info-copy" style={{ width: '100%' }}>
        <span>Emergency alerts · Route-specific analysis</span>
        {!route ? (
          <strong>Calculate a route to see avoided roads</strong>
        ) : loading ? (
          <strong>Checking route alternatives…</strong>
        ) : (
          <>
            <strong>{avoided.length} relevant road alternatives avoided</strong>
            <p><Route size={13} /> Reasons for this origin and destination</p>
            {avoided.length === 0 ? (
              <p>No risky or blocked alternatives were identified near this route.</p>
            ) : (
              <div style={{ display: 'grid', gap: 9, marginTop: 10 }}>
                {avoided.map((road) => (
                  <div key={road.id} style={{ padding: 10, borderRadius: 8, background: 'rgba(255,255,255,.045)', borderLeft: `3px solid ${road.status === 'blocked' ? '#ef4444' : '#f59e0b'}` }}>
                    <div style={{ display: 'flex', alignItems: 'center', gap: 7, flexWrap: 'wrap' }}>
                      <strong style={{ margin: 0 }}>{road.name}</strong>
                      <span className={`status-text ${road.status}`}>{road.status}</span>
                    </div>
                    <p style={{ display: 'block', lineHeight: 1.5 }}>{road.reason}</p>
                  </div>
                ))}
              </div>
            )}
          </>
        )}
      </div>
    </section>
  )
}

export function RoadStatusPanel({ data, loading }: { data: Road[] | null; loading: boolean }) {
  return (
    <section className="panel compact-panel">
      <div className="panel-heading">
        <div><span className="eyebrow">Network overview</span><h2>Road conditions</h2></div>
        <ShieldAlert size={20} />
      </div>
      {loading ? <p className="data-state">Loading road conditions…</p> : !data?.length ? <p className="data-state">Road conditions unavailable.</p> : (
        <div className="road-list">
          {data.map((road) => (
            <div className="road-row" key={road.id}>
              <i className={`status-dot ${road.status}`} />
              <strong>{road.name}</strong>
              <span className={`status-text ${road.status}`}>{road.status}</span>
            </div>
          ))}
        </div>
      )}
    </section>
  )
}
