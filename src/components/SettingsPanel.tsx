import { MonitorCog, RotateCcw } from 'lucide-react'

interface SettingsPanelProps {
  reducedMotion: boolean
  compactMode: boolean
  onReducedMotionChange: (value: boolean) => void
  onCompactModeChange: (value: boolean) => void
  onReset: () => void
}

export default function SettingsPanel(props: SettingsPanelProps) {
  return (
    <section className="panel settings-panel">
      <div className="panel-heading">
        <div><span className="eyebrow">Interface preferences</span><h2>Settings</h2></div>
        <MonitorCog size={21} />
      </div>
      <div className="setting-list">
        <label className="setting-row">
          <div><strong>Reduce motion</strong><span>Disable map and interface transition effects.</span></div>
          <input type="checkbox" checked={props.reducedMotion} onChange={(event) => props.onReducedMotionChange(event.target.checked)} />
        </label>
        <label className="setting-row">
          <div><strong>Compact panels</strong><span>Use denser spacing to show more operational information.</span></div>
          <input type="checkbox" checked={props.compactMode} onChange={(event) => props.onCompactModeChange(event.target.checked)} />
        </label>
      </div>
      <div className="settings-note">
        <strong>Backend connection</strong>
        <span>{import.meta.env.VITE_API_BASE_URL || 'Same origin'}</span>
        <p>The frontend communicates only with the configured C++ HTTP API.</p>
      </div>
      <button className="secondary-button reset-button" onClick={props.onReset}><RotateCcw size={16} />Reset preferences</button>
    </section>
  )
}
