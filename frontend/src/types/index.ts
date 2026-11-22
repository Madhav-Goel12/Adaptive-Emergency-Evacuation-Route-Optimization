
export type Coordinates = [number, number]

export interface Location {
  id: string
  name: string
  address: string
  coordinates: Coordinates
}

export type RoadStatus = 'safe' | 'risky' | 'congested' | 'blocked'

export interface Road {
  id: string
  name: string
  status: RoadStatus
  coordinates?: Coordinates[]
}

export interface Weather {
  temperature?: number
  humidity?: number
  precipitation?: number
  windSpeed?: number
  weatherCode?: number
  condition?: string
  visibility?: string
  updatedAt?: string
}

export interface Alert {
  id: string
  title: string
  description: string
  severity: 'low' | 'medium' | 'high'
}

export interface AvoidedRoad {
  id: string
  name: string
  status: 'risky' | 'congested' | 'blocked'
  reason: string
  hazardFactor: number
  coordinates: Coordinates[]
}

export interface RouteResult {
  distanceKm: number
  durationMinutes: number
  coordinates: Coordinates[]
  details: string
  avoidedRoads: AvoidedRoad[]
  alerts: Alert[]
}
