import type { Alert, Road, RouteResult, Weather } from '../types'

const API_BASE_URL = import.meta.env.VITE_API_BASE_URL ?? ''

async function request<T>(path: string, options?: RequestInit): Promise<T> {
  const response = await fetch(`${API_BASE_URL}${path}`, {
    headers: { 'Content-Type': 'application/json', ...options?.headers },
    ...options,
  })
  if (!response.ok) throw new Error(`Request failed (${response.status})`)
  return response.json() as Promise<T>
}

export const api = {
  getRoads: () => request<Road[]>('/api/roads'),
  getWeather: () => request<Weather>('/api/weather'),
  getAlerts: () => request<Alert[]>('/api/alerts'),
  getRoute: (startId: string, destinationId: string) =>
    request<RouteResult>('/api/route', {
      method: 'POST',
      body: JSON.stringify({ startId, destinationId }),
    }),
}
