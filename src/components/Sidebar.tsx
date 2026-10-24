import { CloudSun, LayoutDashboard, Map, Navigation, Route, Settings, TrafficCone, X } from 'lucide-react'

const navigation = [
  [LayoutDashboard, 'Dashboard', 'dashboard'],
  [Map, 'Live Map', 'map'],
  [Route, 'Route Planner', 'route'],
  [TrafficCone, 'Road Conditions', 'roads'],
  [CloudSun, 'Weather & Alerts', 'weather'],
  [Settings, 'Settings', 'settings'],
] as const

export type PageId = (typeof navigation)[number][2]

interface SidebarProps {
  open: boolean
  activePage: PageId
  onClose: () => void
  onNavigate: (page: PageId) => void
}

export default function Sidebar({ open, activePage, onClose, onNavigate }: SidebarProps) {
  return (
    <>
      {open && <div className="sidebar-backdrop" onClick={onClose} />}
      <aside className={`sidebar ${open ? 'sidebar-open' : ''}`}>
        <div className="brand">
          <div className="brand-mark"><Navigation size={18} /></div>
          <div className="brand-copy">
            <strong>Adaptive Emergency<br />Evacuation Route Optimization</strong>
          </div>
          <button className="icon-button sidebar-close" onClick={onClose} aria-label="Close navigation"><X size={20} /></button>
        </div>
        <nav aria-label="Primary navigation">
          <p className="nav-label">Operations</p>
          {navigation.map(([Icon, label, page]) => (
            <button
              className={`nav-item ${activePage === page ? 'active' : ''}`}
              key={page}
              onClick={() => { onNavigate(page); onClose() }}
            >
              <Icon size={19} /><span>{label}</span>{page === 'map' && <i>LIVE</i>}
            </button>
          ))}
        </nav>
        <div className="system-card">
          <div className="status-row"><span className="status-dot online" />Interface ready</div>
          <p>Awaiting C++ API responses</p>
          <span>v1.0.0</span>
        </div>
      </aside>
    </>
  )
}
